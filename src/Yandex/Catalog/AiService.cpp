#include "AiService.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
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

void AiService::ensureLoaded() {
  // Повторно грузим, только если кэш протух.
  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  if (m_loaded && (now - m_loadedAt) < kCacheTtlMs) {
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

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
      return;
    }

    const QJsonArray artists = doc.object().value(QStringLiteral("artists")).toArray();
    if (artists.isEmpty()) {
      return;
    }

    m_aiArtists.clear();
    for (const QJsonValue &value : artists) {
      if (value.isString() && !value.toString().isEmpty()) {
        m_aiArtists.insert(value.toString().trimmed());
      } else if (value.isDouble()) {
        const QString id = QString::number(value.toVariant().toLongLong());
        if (!id.isEmpty()) {
          m_aiArtists.insert(id);
        }
      }
    }

    m_loaded = true;
    m_loadedAt = QDateTime::currentMSecsSinceEpoch();
    emit listChanged();
  });
}