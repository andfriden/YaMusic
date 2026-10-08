#include "CensorService.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

namespace {
const char *kCensorListUrl =
    "https://raw.githubusercontent.com/Hazzz895/FckCensorData/refs/heads/main/list.json";
constexpr qint64 kCacheTtlMs = 24 * 60 * 60 * 1000;  // сутки
} // namespace

CensorService::CensorService(QObject *parent)
    : QObject(parent), m_network(new QNetworkAccessManager(this)) {}

QString CensorService::replacementUrl(const QString &trackId) {
  const QString id = trackId.trimmed();
  if (id.isEmpty()) {
    return {};
  }

  if (!m_loaded && !m_loading) {
    ensureLoaded();
  }

  return m_replacements.value(id);
}

void CensorService::ensureLoaded() {
  // Повторно грузим, только если кэш протух.
  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  if (m_loaded && (now - m_loadedAt) < kCacheTtlMs) {
    return;
  }

  m_loading = true;
  QNetworkRequest request{QUrl(QString::fromLatin1(kCensorListUrl))};
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

    const QJsonObject root = doc.object();
    const QJsonObject tracks =
        root.value(QStringLiteral("tracks")).isObject()
            ? root.value(QStringLiteral("tracks")).toObject()
            : root;

    m_replacements.clear();
    for (auto it = tracks.begin(); it != tracks.end(); ++it) {
      if (it.value().isString() && !it.value().toString().isEmpty()) {
        m_replacements.insert(it.key(), it.value().toString());
      }
    }

    m_loaded = true;
    m_loadedAt = QDateTime::currentMSecsSinceEpoch();
  });
}