#include "AiService.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>

namespace {
const char *kAiListUrl =
    "https://raw.githubusercontent.com/alexeyfv/slopless/main/data/yandex_music.json";
constexpr qint64 kCacheTtlMs = 24 * 60 * 60 * 1000;  // сутки
} // namespace

AiService::AiService(QObject *parent)
    : QObject(parent), m_network(new QNetworkAccessManager(this)) {}

bool AiService::isAiArtist(const QString &artistId) {
  const QString id = artistId.trimmed();
  if (id.isEmpty()) {
    return false;
  }

  if (!m_loaded && !m_loading) {
    ensureLoaded();
  }

  return m_aiArtists.contains(id);
}

void AiService::refresh() {
  // Поднимаем то, что есть на диске, даже просроченное: бейджи должны
  // работать сразу и без сети.
  if (!m_loaded && loadFromDisk(/*allowStale=*/true)) {
    emit listChanged();
  }

  // И всегда тянем свежий список при запуске.
  fetchFromNetwork();
}

void AiService::ensureLoaded() {
  // Ленивая загрузка — на случай, если refresh ещё не вызывался.
  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  if (m_loaded && (now - m_loadedAt) < kCacheTtlMs) {
    return;
  }

  if (!m_loaded && loadFromDisk(/*allowStale=*/false)) {
    // Уведомляем после возврата из текущего вызова: getter не должен
    // синхронно пересобирать списки.
    QMetaObject::invokeMethod(this, [this]() { emit listChanged(); },
                              Qt::QueuedConnection);
    return;
  }

  fetchFromNetwork();
}

void AiService::fetchFromNetwork() {
  if (m_loading) {
    return;
  }

  m_loading = true;
  QNetworkRequest request{QUrl(QString::fromLatin1(kAiListUrl))};
  request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
  QNetworkReply *reply = m_network->get(request);

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    const QByteArray data = reply->readAll();
    const bool ok = (reply->error() == QNetworkReply::NoError);
    reply->deleteLater();
    m_loading = false;

    if (!ok || data.isEmpty()) {
      return;
    }

    if (!applyJson(data)) {
      return;
    }

    saveToDisk(data);
    m_loaded = true;
    m_loadedAt = QDateTime::currentMSecsSinceEpoch();
    emit listChanged();
  });
}

bool AiService::applyJson(const QByteArray &data) {
  QJsonParseError parseError;
  const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    return false;
  }

  const QJsonArray artists = doc.object().value(QStringLiteral("artists")).toArray();
  if (artists.isEmpty()) {
    return false;
  }

  QSet<QString> parsed;
  for (const QJsonValue &value : artists) {
    if (value.isString() && !value.toString().isEmpty()) {
      parsed.insert(value.toString().trimmed());
    } else if (value.isDouble()) {
      const QString id = QString::number(value.toVariant().toLongLong());
      if (!id.isEmpty()) {
        parsed.insert(id);
      }
    }
  }

  if (parsed.isEmpty()) {
    return false;
  }

  m_aiArtists = parsed;
  return true;
}

QString AiService::cacheFilePath() const {
  const QString dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
  return dir + QStringLiteral("/ai_artists.json");
}

bool AiService::loadFromDisk(bool allowStale) {
  const QString path = cacheFilePath();
  QFileInfo info(path);
  if (!info.exists()) {
    return false;
  }

  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  if (!allowStale &&
      (now - info.lastModified().toMSecsSinceEpoch()) >= kCacheTtlMs) {
    return false;
  }

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }
  const QByteArray data = file.readAll();
  file.close();

  if (!applyJson(data)) {
    return false;
  }

  m_loaded = true;
  m_loadedAt = info.lastModified().toMSecsSinceEpoch();
  return true;
}

void AiService::saveToDisk(const QByteArray &data) const {
  const QString path = cacheFilePath();
  QDir().mkpath(QFileInfo(path).absolutePath());

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    return;
  }
  file.write(data);
  file.close();
}