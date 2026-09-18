#include "CoverImageProvider.h"
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QPainterPath>
#include <QStandardPaths>
#include <QUrl>

namespace {
constexpr auto kCirclePrefix = "circle/";

// Кэш обложек: память (QHash) + диск (CacheLocation).
// Храним СЫРОЕ изображение по ключу URL — до круговой
// обрезки, поэтому одна запись обслуживает и обычные,
// и круглые запросы одного и того же арта.
// Кэш защищён мьютексом: QQuickAsyncImageProvider
// вызывает requestImageResponse в рабочих потоках.
class CoverCache {
public:
  static CoverCache &instance() {
    static CoverCache cache;
    return cache;
  }

  // Пытается достать сырое изображение из памяти или с диска.
  // Возвращает null-QImage при промахе.
  QImage find(const QString &url) {
    QMutexLocker locker(&m_mutex);
    const auto it = m_memory.find(url);

    if (it != m_memory.end()) {
      return it.value();
    }
    const QString path = filePathFor(url);
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
      return {};
    }
    QImage image;

    if (!image.loadFromData(file.readAll())) {
      return {};
    }
    m_memory.insert(url, image);
    return image;
  }

  void store(const QString &url, const QImage &image) {
    if (image.isNull()) {
      return;
    }
    QMutexLocker locker(&m_mutex);
    m_memory.insert(url, image);
    const QString path = filePathFor(url);
    QDir().mkpath(QFileInfo(path).absolutePath());

    // Записываем побайтово, чтобы не тянуть PNG-энкодер
    // на каждый кадр; читается через image.loadFromData.
    if (!image.save(path, "PNG")) {
      return;
    }
  }

private:
  QString filePathFor(const QString &url) const {
    const QByteArray hash = QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Sha1);
    return QDir(m_cacheDir).filePath(QString::fromLatin1(hash.toHex()) + ".png");
  }

  CoverCache() {
    m_cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/covers";
    QDir().mkpath(m_cacheDir);
  }

  QMutex m_mutex;
  QHash<QString, QImage> m_memory;
  QString m_cacheDir;
};

} // namespace

CoverImageResponse::CoverImageResponse(const QString &url, const QSize &requestedSize,
                                       bool circular)
    : m_url(url), m_requestedSize(requestedSize), m_circular(circular) {
  load();
}

QQuickTextureFactory *CoverImageResponse::textureFactory() const {
  if (m_image.isNull()) {
    return nullptr;
  }
  return QQuickTextureFactory::textureFactoryForImage(m_image);
}

// Каждый запрос живёт независимо, отменять не нужно.
void CoverImageResponse::cancel() {}

// TODO(YM-2250): загружать изображение через кэширующий QNetworkDiskCache,
// чтобы сетевые запросы обложек не дублировались между провайдерами.
QImage CoverImageResponse::makeCircular(const QImage &image) const {
  if (image.isNull()) {
    return {};
  }
  // Сначала приводим изображение к квадрату: берём меньшую сторону,
  // чтобы круг всегда целиком помещался внутри исходной обложки.
  const int side = qMin(image.width(), image.height());

  if (side <= 0) {
    return {};
  }
  const int x = (image.width() - side) / 2;
  const int y = (image.height() - side) / 2;
  QImage square = image.copy(x, y, side, side);

  if (square.isNull()) {
    return {};
  }
  // Масштабируем под запрошенный размер, если он задан.
  QSize targetSize = square.size();

  if (m_requestedSize.width() > 0 && m_requestedSize.height() > 0) {
    const int target = qMin(m_requestedSize.width(), m_requestedSize.height());

    if (target > 0) {
      targetSize = QSize(target, target);
    }
  }
  if (square.size() != targetSize) {
    square = square.scaled(targetSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  }
  // Создаём прозрачное ARGB-изображение.
  QImage result(square.size(), QImage::Format_ARGB32_Premultiplied);
  result.fill(Qt::transparent);

  // Рисуем обложку через круговой путь обрезки.
  QPainter painter(&result);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  const QRectF targetRect(0, 0, result.width(), result.height());
  const qreal radius = qMin(targetRect.width(), targetRect.height()) / 2.0;
  QPainterPath path;
  path.addEllipse(targetRect.center(), radius, radius);
  painter.setClipPath(path);
  painter.drawImage(targetRect, square);
  painter.end();
  return result;
}

void CoverImageResponse::load() {
  if (m_url.isEmpty()) {
    emit finished();
    return;
  }
  // 1) Пытаемся достать из кэша (память + диск).
  const QImage cached = CoverCache::instance().find(m_url);

  if (!cached.isNull()) {
    m_image = m_circular ? makeCircular(cached) : cached;
    emit finished();
    return;
  }
  // 2) Промах — сетевой запрос.
  auto *networkManager = new QNetworkAccessManager();
  QNetworkRequest request{QUrl(m_url)};
  request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("YaMusic/1.0 (Qt)"));
  request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
  QNetworkReply *reply = networkManager->get(request);

  QObject::connect(reply, &QNetworkReply::finished, this, [this, reply, networkManager]() {
    if (reply->error() != QNetworkReply::NoError) {
      reply->deleteLater();
      networkManager->deleteLater();
      emit finished();
      return;
    }
    const QByteArray data = reply->readAll();
    QImage image;

    if (!image.loadFromData(data)) {
      reply->deleteLater();
      networkManager->deleteLater();
      emit finished();
      return;
    }
    // Сохраняем сырое изображение в кэш —
    // и для обычных, и для круглых запросов.
    CoverCache::instance().store(m_url, image);
    m_image = m_circular ? makeCircular(image) : image;

    if (m_image.isNull()) {
      reply->deleteLater();
      networkManager->deleteLater();
      emit finished();
      return;
    }
    reply->deleteLater();
    networkManager->deleteLater();
    emit finished();
  });
}

CoverImageProvider::CoverImageProvider() : QQuickAsyncImageProvider() {}

QQuickImageResponse *CoverImageProvider::requestImageResponse(const QString &id,
                                                              const QSize &requestedSize) {
  const bool circular = isCircularRequest(id);
  const QString cleanId = stripRequestPrefix(id);
  const QString url = createUrl(cleanId);
  return new CoverImageResponse(url, requestedSize, circular);
}

bool CoverImageProvider::isCircularRequest(const QString &id) const {
  return id.startsWith(QString::fromLatin1(kCirclePrefix));
}

QString CoverImageProvider::stripRequestPrefix(QString id) const {
  const QString prefix = QString::fromLatin1(kCirclePrefix);

  if (id.startsWith(prefix)) {
    id.remove(0, prefix.size());
  }
  return id;
}

QString CoverImageProvider::createUrl(QString uri) const {
  if (uri.isEmpty()) {
    return {};
  }
  // QML может передать avatars.yandex.net/.../%%
  // в виде avatars.yandex.net/.../%25%25 —
  // сначала раскодируем процентное экранирование.
  QString normalized = QUrl::fromPercentEncoding(uri.toUtf8());
  normalized = normalized.trimmed();

  // Яндекс Музыка использует %% как плейсхолдер размера картинки.
  normalized.replace("%%", "200x200");

  // Также обрабатываем закодированную форму на случай,
  // если сюда пришло значение без декодирования выше.
  normalized.replace("%25%25", "200x200");

  if (normalized.startsWith("http://") || normalized.startsWith("https://")) {
    return normalized;
  }
  return QStringLiteral("https://") + normalized;
}