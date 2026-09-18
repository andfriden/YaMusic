#include "PlaylistService.h"
#include "../Auth/YandexAuth.h"
#include "../Parsers.h"
#include "../YandexClient.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

namespace {
Playlist parsePlaylist(const QJsonObject &object) {
  Playlist playlist;
  playlist.uid = QString::number(object.value("uid").toInteger());
  playlist.kind = object.value("kind").toInt();
  playlist.uuid = object.value("playlistUuid").toString();

  if (playlist.uuid.isEmpty()) {
    playlist.uuid = object.value("uuid").toString();
  }

  playlist.title = object.value("title").toString();
  playlist.description = object.value("description").toString();
  playlist.trackCount = object.value("trackCount").toInt();
  playlist.revision = object.value("revision").toInt();
  const QJsonObject cover = object.value("cover").toObject();
  playlist.coverUri = cover.value("uri").toString();
  const QJsonArray tracks = object.value("tracks").toArray();

  for (const QJsonValue &value : tracks) {
    if (!value.isObject()) continue;
    const QJsonObject wrapper = value.toObject();
    QJsonObject trackObject = wrapper.value("track").toObject();

    if (trackObject.isEmpty()) {
      trackObject = wrapper;
    }

    if (trackObject.isEmpty()) continue;
    const Track track = ::parseTrack(trackObject);

    if (!track.id.isEmpty()) {
      playlist.tracks.append(track);
    }
  }
  return playlist;
}

bool parsePlaylistResponse(const QByteArray &data, Playlist &playlist) {
  QJsonParseError parseError;
  const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
    return false;
  }

  const QJsonObject playlistObject = unwrapResult(document);

  if (playlistObject.isEmpty()) {
    return false;
  }

  playlist = parsePlaylist(playlistObject);
  return (!playlist.uid.isEmpty() && playlist.kind > 0);
}

PersonalPlaylist parseUserPlaylist(const QJsonObject &object) {
  PersonalPlaylist playlist;
  playlist.id = object.value("id").toString();

  if (playlist.id.isEmpty()) {
    const qint64 id = object.value("id").toInteger();

    if (id > 0) {
      playlist.id = QString::number(id);
    }
  }

  playlist.uid = QString::number(object.value("uid").toInteger());
  playlist.kind = object.value("kind").toInt();
  playlist.revision = object.value("revision").toInt();
  playlist.title = object.value("title").toString();
  playlist.description = object.value("description").toString();
  playlist.previewDescription = object.value("description").toString();
  playlist.trackCount = object.value("trackCount").toInt();
  playlist.generatedPlaylistType = object.value("generatedPlaylistType").toString();
  playlist.coverUri = object.value("cover").toObject().value("uri").toString();
  return playlist;
}

Playlist parseSimilarPlaylist(const QJsonObject &object) {
  Playlist playlist;
  playlist.uid = QString::number(object.value("uid").toInteger());
  playlist.kind = object.value("kind").toInt();
  playlist.uuid = object.value("playlistUuid").toString();

  if (playlist.uuid.isEmpty()) {
    playlist.uuid = object.value("uuid").toString();
  }

  playlist.title = object.value("title").toString();
  playlist.coverUri = object.value("cover").toObject().value("uri").toString();

  if (playlist.coverUri.isEmpty()) {
    playlist.coverUri = object.value("coverUri").toString();
  }
  return playlist;
}

} // namespace

PlaylistService::PlaylistService(YandexAuth *auth, QObject *parent)
    : YandexServiceBase(auth, parent) {}

void PlaylistService::loadPlaylist(const QString &uid, int kind) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString trimmedUid = uid.trimmed();

  if (trimmedUid.isEmpty()) {
    emit errorOccurred("UID плейлиста не указан");
    return;
  }

  if (kind <= 0) {
    emit errorOccurred("Идентификатор плейлиста некорректен");
    return;
  }

  // L1-кэш: отдаём сразу, если данные ещё свежие.
  const QString cacheKey = trimmedUid + ":" + QString::number(kind);
  Playlist cached;

  if (m_playlistCache.get(cacheKey, cached)) {
    emit playlistReceived(cached);
    return;
  }

  const auto path = QStringLiteral("/users/%1/playlists/%2").arg(trimmedUid).arg(kind);
  QNetworkReply *reply = m_yandexClient->get(path);

  if (reply == nullptr) {
    emit errorOccurred("Не удалось загрузить плейлист");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply, cacheKey]() {
    const QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }

    Playlist playlist;

    if (!parsePlaylistResponse(data, playlist)) {
      emit errorOccurred("Некорректный ответ плейлиста");
      reply->deleteLater();
      return;
    }

    if (!playlist.uuid.isEmpty()) {
      loadSimilarPlaylists(playlist.uuid);
    }

    m_playlistCache.put(cacheKey, playlist);
    emit playlistReceived(playlist);
    reply->deleteLater();
  });
}

void PlaylistService::loadPlaylists(const QList<QPair<QString, int>> &playlists) {
  m_playlistBatchQueue.clear();
  m_playlistBatchResults.clear();
  m_playlistBatchActive = 0;
  m_playlistBatchCompleted = 0;
  m_playlistBatchError = false;

  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  for (const QPair<QString, int> &playlist : playlists) {
    const QString uid = playlist.first.trimmed();
    const int kind = playlist.second;
    if (uid.isEmpty() || kind <= 0) continue;
    m_playlistBatchQueue.append(qMakePair(uid, kind));
  }

  if (m_playlistBatchQueue.isEmpty()) {
    emit playlistsReceived({});
    return;
  }

  startNextPlaylistBatchRequests();
}

void PlaylistService::startNextPlaylistBatchRequests() {
  while (m_playlistBatchActive < MaxConcurrentPlaylistRequests && !m_playlistBatchQueue.isEmpty()) {
    const QPair<QString, int> playlist = m_playlistBatchQueue.takeFirst();
    const QString uid = playlist.first;
    const int kind = playlist.second;
    const auto path = QStringLiteral("/users/%1/playlists/%2").arg(uid).arg(kind);
    QNetworkReply *reply = m_yandexClient->get(path);
    ++m_playlistBatchActive;

    if (reply == nullptr) {
      --m_playlistBatchActive;
      ++m_playlistBatchCompleted;
      m_playlistBatchError = true;
      continue;
    }

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
      const QByteArray data = reply->readAll();
      --m_playlistBatchActive;
      ++m_playlistBatchCompleted;

      if (reply->error() != QNetworkReply::NoError) {
        m_playlistBatchError = true;

      } else {
        Playlist playlist;

        if (parsePlaylistResponse(data, playlist)) {
          m_playlistBatchResults.append(playlist);

        } else {
          m_playlistBatchError = true;
        }
      }

      reply->deleteLater();
      startNextPlaylistBatchRequests();

      if (m_playlistBatchActive == 0 && m_playlistBatchQueue.isEmpty()) {
        finishPlaylistBatch();
      }
    });
  }

  if (m_playlistBatchActive == 0 && m_playlistBatchQueue.isEmpty() &&
      m_playlistBatchCompleted > 0) {
    finishPlaylistBatch();
  }
}

void PlaylistService::finishPlaylistBatch() {
  const QList<Playlist> results = m_playlistBatchResults;
  m_playlistBatchQueue.clear();
  m_playlistBatchResults.clear();
  m_playlistBatchActive = 0;
  m_playlistBatchCompleted = 0;
  m_playlistBatchError = false;
  emit playlistsReceived(results);
}

void PlaylistService::loadUserPlaylists(const QString &uid) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString trimmedUid = uid.trimmed();

  if (trimmedUid.isEmpty()) {
    emit errorOccurred("UID пользователя не указан");
    return;
  }

  const auto path = QStringLiteral("/users/%1/playlists/list").arg(trimmedUid);
  QNetworkReply *reply = m_yandexClient->get(path);

  if (reply == nullptr) {
    emit errorOccurred("Не удалось загрузить плейлисты пользователя");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    const QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
      emit errorOccurred("Некорректный ответ плейлистов пользователя");
      reply->deleteLater();
      return;
    }

    QJsonArray playlistArray;

    if (document.isArray()) {
      playlistArray = document.array();

    } else if (document.isObject()) {
      const QJsonObject root = document.object();

      if (root.value("result").isArray()) {
        playlistArray = root.value("result").toArray();
      }
    }

    if (playlistArray.isEmpty() && !document.isArray()) {
      emit errorOccurred("Некорректная структура плейлистов пользователя");
      reply->deleteLater();
      return;
    }

    QList<PersonalPlaylist> playlists;

    for (const QJsonValue &value : playlistArray) {
      if (!value.isObject()) continue;
      const PersonalPlaylist playlist = parseUserPlaylist(value.toObject());
      if (playlist.uid.isEmpty() || playlist.kind <= 0) continue;
      playlists.append(playlist);
    }

    emit userPlaylistsReceived(playlists);
    reply->deleteLater();
  });
}

void PlaylistService::loadSimilarPlaylists(const QString &uuid) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString trimmedUuid = uuid.trimmed();

  if (trimmedUuid.isEmpty()) {
    return;
  }

  const auto path = QStringLiteral("/playlist/%1/similar-entities").arg(trimmedUuid);
  QNetworkReply *reply = m_yandexClient->get(path);

  if (reply == nullptr) {
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    const QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
      reply->deleteLater();
      return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
      reply->deleteLater();
      return;
    }

    const QJsonObject root = document.object();
    const QJsonObject result = root.value("result").toObject();
    const QJsonArray items = result.value("items").toArray();
    QList<Playlist> playlists;

    for (const QJsonValue &value : items) {
      if (!value.isObject()) continue;
      const QJsonObject item = value.toObject();
      if (item.value("type").toString() != "playlist_item") continue;
      const QJsonObject playlistObject = item.value("data").toObject().value("playlist").toObject();
      if (playlistObject.isEmpty()) continue;
      const Playlist playlist = parseSimilarPlaylist(playlistObject);

      if (playlist.uid.isEmpty() || playlist.uuid.isEmpty() || playlist.kind <= 0 ||
          playlist.title.isEmpty()) {
        continue;
      }

      playlists.append(playlist);
    }

    emit similarPlaylistsReceived(playlists);
    reply->deleteLater();
  });
}

void PlaylistService::createPlaylist(const QString &uid, const QString &title) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString userId = uid.trimmed();
  const QString playlistTitle = title.trimmed();

  if (userId.isEmpty()) {
    emit errorOccurred("UID не указан");
    return;
  }

  if (playlistTitle.isEmpty()) {
    emit errorOccurred("Название плейлиста не указано");
    return;
  }

  QUrlQuery formBody;
  formBody.addQueryItem("title", playlistTitle);
  formBody.addQueryItem("visibility", "public");
  const auto path = QStringLiteral("/users/%1/playlists/create").arg(userId);
  QNetworkReply *reply = m_yandexClient->postForm(path, formBody);

  if (reply == nullptr) {
    emit errorOccurred("Не удалось создать плейлист");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply, playlistTitle]() {
    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }

    reply->deleteLater();
    emit playlistCreated(playlistTitle);
  });
}

void PlaylistService::deletePlaylist(const QString &uid, int kind) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString userId = uid.trimmed();

  if (userId.isEmpty() || kind <= 0) {
    emit errorOccurred("Некорректный плейлист");
    return;
  }

  const auto path = QStringLiteral("/users/%1/playlists/%2/delete").arg(userId).arg(kind);
  QNetworkReply *reply = m_yandexClient->postForm(path, {});

  if (reply == nullptr) {
    emit errorOccurred("Не удалось удалить плейлист");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply, userId, kind]() {
    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }

    reply->deleteLater();
    m_playlistCache.remove(cacheKeyFor(userId, kind));
    emit playlistDeleted(kind);
  });
}

QString PlaylistService::cacheKeyFor(const QString &uid, int kind) const {
  return QString("%1:%2").arg(uid.trimmed()).arg(kind);
}

void PlaylistService::renamePlaylist(const QString &uid, int kind, const QString &newTitle) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString userId = uid.trimmed();
  const QString title = newTitle.trimmed();

  if (userId.isEmpty() || kind <= 0) {
    emit errorOccurred("Некорректный плейлист");
    return;
  }

  if (title.isEmpty()) {
    emit errorOccurred("Новое название не указано");
    return;
  }

  QUrlQuery body;
  body.addQueryItem("value", title);
  const auto path = QStringLiteral("/users/%1/playlists/%2/name").arg(userId).arg(kind);
  QNetworkReply *reply = m_yandexClient->postForm(path, body);

  if (reply == nullptr) {
    emit errorOccurred("Не удалось переименовать плейлист");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply, title, userId, kind]() {
    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }

    reply->deleteLater();
    m_playlistCache.remove(cacheKeyFor(userId, kind));
    emit playlistRenamed(title);
  });
}

void PlaylistService::addTracksToPlaylist(const QString &uid, int kind, const QStringList &trackIds,
                                          const QStringList &albumIds, int revision) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString userId = uid.trimmed();

  if (userId.isEmpty() || kind <= 0) {
    emit errorOccurred("Некорректный плейлист");
    return;
  }

  QJsonArray operations;
  QJsonObject op;
  op["op"] = "insert";
  op["at"] = 0;
  QJsonArray trackList;
  const int count = qMin(trackIds.size(), albumIds.size());
  for (int i = 0; i < count; ++i) {
    const QString tid = trackIds.at(i).trimmed();
    const QString aid = albumIds.at(i).trimmed();
    if (tid.isEmpty() || aid.isEmpty()) continue;
    QJsonObject t;
    t["id"] = tid.toInt();
    t["albumId"] = aid.toInt();
    trackList.append(t);
  }
  op["tracks"] = trackList;
  operations.append(op);

  const QString diffStr =
      QString::fromUtf8(QJsonDocument(operations).toJson(QJsonDocument::Compact));
  QUrlQuery formBody;
  formBody.addQueryItem("kind", QString::number(kind));
  formBody.addQueryItem("revision", QString::number(revision));
  formBody.addQueryItem("diff", diffStr);
  const auto path = QStringLiteral("/users/%1/playlists/%2/change").arg(userId).arg(kind);
  QNetworkReply *reply = m_yandexClient->postForm(path, formBody);

  if (reply == nullptr) {
    emit errorOccurred("Не удалось добавить треки");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply, trackList, kind, userId]() {
    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }

    const QByteArray data = reply->readAll();

    // Ответ /change описывает целевой плейлист, а не тот,
    // что открыт сейчас — не эмитим playlistReceived, чтобы
    // не затереть текущую модель. LibraryController сам
    // перезагрузит открытый плейлист и обновит список.
    qWarning() << "[add] success, kind=" << kind << "added=" << trackList.size()
               << "response=" << QString::fromUtf8(data).left(200);
    reply->deleteLater();
    m_playlistCache.remove(cacheKeyFor(userId, kind));
    emit tracksAdded(kind, trackList.size());
  });
}

void PlaylistService::removeTrackFromPlaylist(const QString &uid, int kind, int trackIndex,
                                              int revision) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString userId = uid.trimmed();

  if (userId.isEmpty() || kind <= 0) {
    emit errorOccurred("Некорректный плейлист");
    return;
  }

  if (trackIndex < 0) {
    emit errorOccurred("Некорректный индекс трека");
    return;
  }

  QJsonArray operations;
  QJsonObject op;
  op["op"] = "delete";
  op["from"] = trackIndex;
  op["to"] = trackIndex + 1;
  operations.append(op);

  const QString diffStr =
      QString::fromUtf8(QJsonDocument(operations).toJson(QJsonDocument::Compact));
  QUrlQuery formBody;
  formBody.addQueryItem("kind", QString::number(kind));
  formBody.addQueryItem("revision", QString::number(revision));
  formBody.addQueryItem("diff", diffStr);
  const auto path = QStringLiteral("/users/%1/playlists/%2/change").arg(userId).arg(kind);
  QNetworkReply *reply = m_yandexClient->postForm(path, formBody);

  if (reply == nullptr) {
    emit errorOccurred("Не удалось удалить трек");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply, kind, userId]() {
    const QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(
          QString("Удаление трека: %1 (HTTP %2) %3")
              .arg(reply->errorString())
              .arg(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt())
              .arg(QString::fromUtf8(data).left(300)));
      reply->deleteLater();
      return;
    }

    // Ответ /change описывает изменённый плейлист, но треки
    // в нём могут отсутствовать, а главное — открытая модель
    // может быть другим плейлистом. Не эмитим playlistReceived,
    // чтобы не затереть текущий открытый плейлист.
    qWarning() << "[remove] success, kind=" << kind
               << "response=" << QString::fromUtf8(data).left(200);
    reply->deleteLater();
    m_playlistCache.remove(cacheKeyFor(userId, kind));
    emit tracksRemoved(kind, 1);
  });
}