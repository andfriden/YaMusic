#include "DownloadService.h"

#include "TagWriter.h"

#include "../Yandex/Catalog/TrackService.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QBuffer>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QUrl>

namespace {
constexpr auto kDownloadUserAgent =
    "YaMusic/1.0 (Qt)";

// Нормализует coverUri: раскодируем процентное экранирование,
// подставляем %% → 1000x1000 и https:// (как в CoverImageProvider).
QString normalizeCoverUriForDownload(
    const QString &uri) {
  if (uri.isEmpty())
    return {};

  QString result =
      QUrl::fromPercentEncoding(uri.toUtf8());

  result = result.trimmed();

  result.replace(
      QStringLiteral("%%"),
      QStringLiteral("1000x1000"));

  // Также обрабатываем закодированную форму %% (%25%25),
  // которая приходит из QML без предварительного декодирования.
  result.replace(
      QStringLiteral("%25%25"),
      QStringLiteral("1000x1000"));

  if (!result.contains(QStringLiteral("://")))
    result.prepend(QStringLiteral("https://"));

  return result;
}

// Расширение по кодеку, который отдаёт API.
QString extensionForCodec(
    const QString &codec) {
  const QString c = codec.trimmed().toLower();

  if (c == QStringLiteral("mp3"))
    return QStringLiteral("mp3");
  if (c == QStringLiteral("aac"))
    return QStringLiteral("m4a");
  if (c == QStringLiteral("flac"))
    return QStringLiteral("flac");
  if (c == QStringLiteral("ogg") ||
      c == QStringLiteral("opus"))
    return QStringLiteral("ogg");

  // На случай неизвестного кодека — безопасный дефолт.
  return QStringLiteral("mp3");
}

// Делает имя файла без запрещённых символов.
QString sanitizeFileNamePart(
    const QString &part) {
  QString result = part.trimmed();

  for (QChar &c : result) {
    if (c == QLatin1Char('/') ||
        c == QLatin1Char('\\') ||
        c == QLatin1Char(':') ||
        c == QLatin1Char('*') ||
        c == QLatin1Char('?') ||
        c == QLatin1Char('"') ||
        c == QLatin1Char('<') ||
        c == QLatin1Char('>') ||
        c == QLatin1Char('|'))
      c = QLatin1Char(' ');
  }

  while (result.contains(QStringLiteral("  ")))
    result.replace(QStringLiteral("  "), QStringLiteral(" "));

  result = result.trimmed();

  if (result.isEmpty())
    return QStringLiteral("track");

  return result;
}

} // namespace

DownloadService::DownloadService(
    TrackService *trackService,
    QObject *parent)
    : QObject(parent),
      m_trackService(trackService),
      m_network(new QNetworkAccessManager(this)) {
  Q_ASSERT(m_trackService != nullptr);

  connect(
      m_trackService,
      &TrackService::downloadUrlReceived,
      this,
      &DownloadService::onDownloadUrl);
}

DownloadService::DownloadState
DownloadService::state() const {
  return m_state;
}

double DownloadService::progress() const {
  return m_progress;
}

bool DownloadService::isCurrentDownloaded() const {
  if (m_pendingTrack.id.isEmpty())
    return false;

  return QFile::exists(
      buildDestinationPath(
          m_pendingTrack,
          m_pendingCodec));
}

const Track &DownloadService::pendingTrack() const {
  return m_pendingTrack;
}

void DownloadService::downloadCurrentTrack(
    const Track &track) {
  if (track.id.isEmpty())
    return;

  // Если качаем тот же трек — не перезапускаем.
  if (m_state == Downloading &&
      m_startedTrackId == track.id)
    return;

  m_pendingTrack = track;
  m_audioData.clear();

  m_progress = 0.0;
  m_state = Idle;
  m_startedTrackId = track.id;

  emit trackChanged();
  emit stateChanged();

  m_trackService->loadDownloadUrl(track.id);
}

void DownloadService::reset() {
  if (m_activeReply)
    m_activeReply->abort();

  m_activeReply = nullptr;
  m_audioData.clear();
  m_pendingCodec.clear();
  m_startedTrackId.clear();

  m_progress = 0.0;
  m_pendingTrack = Track{};

  setState(Idle);
}

void DownloadService::onDownloadUrl(
    const QString &trackId,
    const QString &url,
    const QString &codec) {
  if (url.trimmed().isEmpty()) {
    emit statusChanged("Не удалось получить ссылку на скачивание");
    return;
  }

  if (trackId != m_startedTrackId) {
    return;
  }

  m_pendingCodec = codec;

  downloadAudio(
      m_pendingTrack,
      url,
      codec);
}

void DownloadService::downloadAudio(
    const Track &track,
    const QString &url,
    const QString &codec) {
  setState(Downloading);

  QNetworkRequest request{QUrl(url)};

  request.setHeader(
      QNetworkRequest::UserAgentHeader,
      QString::fromLatin1(kDownloadUserAgent));

  request.setAttribute(
      QNetworkRequest::RedirectPolicyAttribute,
      QNetworkRequest::NoLessSafeRedirectPolicy);

  QNetworkReply *reply =
      m_network->get(request);

  m_activeReply = reply;

  connect(
      reply,
      &QNetworkReply::downloadProgress,
      this,
      [this](qint64 received, qint64 total) {
        if (total > 0) {
          m_progress =
              qBound(0.0, double(received) / double(total) * 0.9, 0.9);
          emit stateChanged();
        }
      });

  connect(
      reply,
      &QNetworkReply::finished,
      this,
      [this, reply, track, codec]() {
        if (reply->error() !=
            QNetworkReply::NoError) {
          finishWithError(
              "Ошибка скачивания: " + reply->errorString());
          reply->deleteLater();
          return;
        }

        const QByteArray data =
            reply->readAll();

        if (data.isEmpty()) {
          finishWithError("Получен пустой аудиофайл");
          reply->deleteLater();
          return;
        }

        m_audioData = data;
        m_progress = 1.0;

        emit stateChanged();

        reply->deleteLater();

        // После аудио — обложка.
        downloadCover(track);
      });
}

void DownloadService::downloadCover(
    const Track &track) {
  const QString coverUrl =
      normalizeCoverUriForDownload(track.coverUri);

  // Нет обложки — пишем теги без неё.
  if (coverUrl.isEmpty()) {
    writeTags(track, {});
    return;
  }

  QNetworkRequest request{QUrl(coverUrl)};

  request.setHeader(
      QNetworkRequest::UserAgentHeader,
      QString::fromLatin1(kDownloadUserAgent));

  request.setAttribute(
      QNetworkRequest::RedirectPolicyAttribute,
      QNetworkRequest::NoLessSafeRedirectPolicy);

  QNetworkReply *reply =
      m_network->get(request);

  m_activeReply = reply;

  connect(
      reply,
&QNetworkReply::finished,
      this,
      [this, reply, track]() {
        const bool okReply =
            reply->error() == QNetworkReply::NoError;

        QByteArray cover =
            okReply ? reply->readAll() : QByteArray();

        reply->deleteLater();

        // Приводим обложку к JPEG независимо от исходного формата
        // (сервер может вернуть WebP/PNG). Это гарантирует, что
        // тег-фрейм обложки читается всеми плеерами.
        if (!cover.isEmpty()) {
          QImage image;
          image.loadFromData(cover);

          if (!image.isNull()) {
            QByteArray jpegData;
            QBuffer buffer(&jpegData);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, "JPG", 92);
            buffer.close();

            if (!jpegData.isEmpty())
              cover = jpegData;
          }
        }

        writeTags(track, cover, QStringLiteral("image/jpeg"));
      });
}

void DownloadService::writeTags(
    const Track &track,
    const QByteArray &coverData,
    const QString &mime) {
  const QString destPath =
      buildDestinationPath(track, m_pendingCodec);

  QDir().mkpath(QFileInfo(destPath).absolutePath());

  // Пишем в тот же путь через временный файл: сначала рядом,
  // затем move-ом в финальное имя, чтобы не оставлять битый файл.
  // ВАЖНО: временному файлу нужен корректный суффикс (например .mp3),
  // иначе TagWriter не определит формат по расширению и не запишет
  // теги/обложку.
  QTemporaryFile tmpFile(
      QFileInfo(destPath).absolutePath() +
      QStringLiteral("/yamusic-dl-XXXXXX") +
      QStringLiteral(".") +
      extensionForCodec(m_pendingCodec));

  if (!tmpFile.open()) {
    finishWithError("Не удалось создать временный файл");
    return;
  }

  tmpFile.write(m_audioData);
  tmpFile.flush();

  const QString tmpPath = tmpFile.fileName();
  tmpFile.close(); // закрываем до TagLib (ему нужен закрытый хендл)

  QString artist;
  QString album;

  if (!track.artists.isEmpty())
    artist = track.artists.first().name;

  if (!track.albums.isEmpty())
    album = track.albums.first().title;

  const bool ok =
      TagWriter::writeTags(
          tmpPath,
          track.title,
          artist,
          album,
          0,
          false,
          track.durationMs / 1000,
          coverData,
          mime);

  if (!ok) {
    QFile::remove(tmpPath);
    finishWithError("Не удалось записать метаданные");
    return;
  }

  // Если целевой файл уже существует — перезаписываем.
  QFile::remove(destPath);

  if (!QFile::rename(tmpPath, destPath)) {
    QFile::remove(tmpPath);
    finishWithError("Не удалось сохранить файл");
    return;
  }

  m_audioData.clear();
  m_activeReply = nullptr;

  setState(Done);

  emit statusChanged(
      QStringLiteral("Трек сохранён: %1").arg(destPath));
}

QString DownloadService::buildDestinationPath(
    const Track &track,
    const QString &codec) const {
  const QString downloadsDir =
      QStandardPaths::writableLocation(
          QStandardPaths::DownloadLocation);

  const QString artistName =
      track.artists.isEmpty()
          ? QString()
          : track.artists.first().name;

  const QString fileName =
      sanitizeFileNamePart(artistName) +
      QStringLiteral(" - ") +
      sanitizeFileNamePart(track.title) +
      QStringLiteral(".") +
      extensionForCodec(codec);

  return downloadsDir +
         QStringLiteral("/") +
         fileName;
}

void DownloadService::finishWithError(
    const QString &message) {
  m_audioData.clear();
  m_activeReply = nullptr;

  emit statusChanged(message);

  setState(Error);
}

void DownloadService::setState(DownloadState state) {
  if (m_state == state)
    return;

  m_state = state;
  emit stateChanged();
}