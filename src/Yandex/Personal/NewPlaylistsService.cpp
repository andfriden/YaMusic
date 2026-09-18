#include "NewPlaylistsService.h"
#include "../Auth/YandexAuth.h"
#include "../Parsers.h"
#include "../YandexClient.h"
#include "PlaylistService.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QPair>

NewPlaylistsService::NewPlaylistsService(YandexAuth *auth, PlaylistService *playlistService,
                                         QObject *parent)
    : YandexServiceBase(auth, parent), m_playlistService(playlistService) {
  if (m_playlistService != nullptr) {
    connect(m_playlistService, &PlaylistService::playlistsReceived, this,
            [this](const QList<Playlist> &playlists) { emit playlistsReceived(playlists); });
  }
}

void NewPlaylistsService::load() {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  if (m_playlistService == nullptr) {
    emit errorOccurred("PlaylistService недоступен");
    return;
  }

  const QString path = "/landing3/new-playlists";
  QNetworkReply *reply = m_yandexClient->get(path);

  if (reply == nullptr) {
    emit errorOccurred("Не удалось выполнить запрос новых плейлистов");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    const QByteArray data = reply->readAll();
    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
      emit errorOccurred("Некорректный ответ /landing3/new-playlists");
      reply->deleteLater();
      return;
    }

    const QJsonObject result = unwrapResult(document);

    if (result.isEmpty()) {
      emit errorOccurred("Ответ /landing3/new-playlists не содержит result");
      reply->deleteLater();
      return;
    }

    const QJsonArray newPlaylists = result.value("newPlaylists").toArray();
    QList<QPair<QString, int>> references;
    references.reserve(newPlaylists.size());

    for (const QJsonValue &value : newPlaylists) {
      if (!value.isObject()) continue;
      const QJsonObject obj = value.toObject();
      const qint64 uid = obj.value("uid").toInteger();
      const int kind = obj.value("kind").toInt();
      if (uid <= 0 || kind <= 0) continue;
      references.append(qMakePair(QString::number(uid), kind));
    }

    reply->deleteLater();

    if (references.isEmpty()) {
      emit playlistsReceived({});
      return;
    }

    m_playlistService->loadPlaylists(references);
  });
}