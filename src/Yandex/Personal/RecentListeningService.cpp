#include "RecentListeningService.h"

#include "../Parsers.h"
#include "../YandexClient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkReply>
#include <QSet>

RecentListeningService::RecentListeningService(YandexAuth *auth,
                                               QObject *parent)
    : YandexServiceBase(auth, parent) {}

void RecentListeningService::setUserId(const QString &userId) {
  m_userId = userId;
}

void RecentListeningService::load(int trackCount, int contextCount) {
  Q_UNUSED(contextCount)

  if (m_loading)
    return;

  if (m_userId.isEmpty()) {
    emit errorOccurred(
        QStringLiteral("Пользователь не авторизован"));
    return;
  }

    if (!ensureAuthenticated()) {
        emit errorOccurred(
            QStringLiteral("Пользователь не авторизован"));
        return;
    }
  trackCount = qMax(1, trackCount);

  m_loading = true;
  emit loadingChanged(true);

  const QString path =
      QStringLiteral("/music-history?fullModelsCount=%1")
          .arg(trackCount);

  QNetworkReply *reply = m_yandexClient->get(path);

  if (!reply) {
    m_loading = false;
    emit loadingChanged(false);
    emit errorOccurred(
        QStringLiteral(
            "Не удалось выполнить запрос истории прослушивания"));
    return;
  }

  connect(reply, &QNetworkReply::finished, this,
          [this, reply, trackCount]() {
            const QByteArray data = reply->readAll();
            const QNetworkReply::NetworkError networkError =
                reply->error();
            const QString errorString = reply->errorString();

            reply->deleteLater();

            if (networkError != QNetworkReply::NoError) {
              m_loading = false;
              emit loadingChanged(false);
              emit errorOccurred(
                  QStringLiteral(
                      "Ошибка истории прослушивания: %1")
                      .arg(errorString));
              return;
            }

            const QList<Track> tracks =
                parseHistory(data, trackCount);

            m_loading = false;
            emit loadingChanged(false);
            emit tracksReceived(tracks);
          });
}

QList<Track> RecentListeningService::parseHistory(
    const QByteArray &data,
    int trackCount) const {

  QList<Track> tracks;
  QSet<QString> seenTrackIds;

  const QJsonDocument document =
      QJsonDocument::fromJson(data);

  if (!document.isObject())
    return tracks;

  const QJsonObject root = document.object();
  const QJsonObject result =
      root.value(QStringLiteral("result")).toObject();

  if (result.isEmpty())
    return tracks;

  const QJsonArray historyTabs =
      result.value(QStringLiteral("historyTabs")).toArray();

  for (const QJsonValue &tabValue : historyTabs) {
    if (!tabValue.isObject())
      continue;

    const QJsonObject tab = tabValue.toObject();
    const QJsonArray items =
        tab.value(QStringLiteral("items")).toArray();

    for (const QJsonValue &itemValue : items) {
      if (!itemValue.isObject())
        continue;

      const QJsonObject item = itemValue.toObject();
      const QJsonArray historyTracks =
          item.value(QStringLiteral("tracks")).toArray();

      for (const QJsonValue &historyTrackValue : historyTracks) {
        if (!historyTrackValue.isObject())
          continue;

        const QJsonObject historyTrack =
            historyTrackValue.toObject();

        if (historyTrack.value(QStringLiteral("type")).toString() !=
            QStringLiteral("track")) {
          continue;
        }

        const QJsonObject trackData =
            historyTrack.value(QStringLiteral("data")).toObject();

        if (trackData.isEmpty())
          continue;

        const QJsonObject fullModel =
            trackData.value(QStringLiteral("fullModel")).toObject();

        if (fullModel.isEmpty())
          continue;

        const Track track = parseTrack(fullModel);

        if (track.id.isEmpty())
          continue;

        if (seenTrackIds.contains(track.id))
          continue;

        seenTrackIds.insert(track.id);
        tracks.append(track);

        if (tracks.size() >= trackCount)
          return tracks;
      }
    }
  }

  return tracks;
}