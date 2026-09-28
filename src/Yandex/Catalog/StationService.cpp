#include "StationService.h"
#include "../Auth/YandexAuth.h"
#include "../Parsers.h"
#include "../YandexClient.h"
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QUrlQuery>

StationService::StationService(YandexAuth *auth, QObject *parent)
    : QObject(parent), m_auth(auth), m_yandexClient(new YandexClient(this)) {
  Q_ASSERT(m_auth);
}

static QString stationTypeOf(const QJsonObject &id) {
  return id.value("type").toString().trimmed();
}

static QString stationTagOf(const QJsonObject &id) {
  return id.value("tag").toString().trimmed();
}

static QString imageUrlOf(const QJsonObject &icon) {
  const QString fullImageUrl = icon.value("fullImageUrl").toString();
  if (!fullImageUrl.trimmed().isEmpty()) {
    return fullImageUrl;
  }
  return icon.value("imageUrl").toString();
}

void StationService::loadStations() {
  if (!m_auth->isAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  m_yandexClient->setToken(m_auth->token());
  QUrlQuery query;
  query.addQueryItem("language", "ru");
  const auto path = QStringLiteral("/rotor/stations/list?%1")
                        .arg(query.toString(QUrl::FullyEncoded));
  QNetworkReply *reply = m_yandexClient->get(path);

  if (reply == nullptr) {
    emit errorOccurred("Не удалось создать запрос списка станций");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    auto fail = [&](const QString &msg) {
      emit errorOccurred(msg);
      reply->deleteLater();
    };
    const QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) return fail(reply->errorString());
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) return fail("Некорректный ответ списка станций");

    QJsonArray result;
    if (document.isArray()) {
      result = document.array();
    } else if (document.isObject()) {
      result = document.object().value("result").toArray();
    }
    if (result.isEmpty()) return fail("Список станций пуст");

    QList<Station> stations;

    for (const QJsonValue &value : result) {
      if (!value.isObject()) continue;
      const QJsonObject item = value.toObject();
      const QJsonObject stationObject = item.value("station").toObject();
      if (stationObject.isEmpty()) continue;
      const QJsonObject id = stationObject.value("id").toObject();
      if (id.isEmpty()) continue;

      Station station;
      station.type = stationTypeOf(id);
      station.tag = stationTagOf(id);
      station.name = stationObject.value("name").toString();
      const QJsonObject icon = stationObject.value("icon").toObject();
      station.imageUrl = imageUrlOf(icon);
      station.backgroundColor = icon.value("backgroundColor").toString();

      if (station.type.isEmpty() || station.tag.isEmpty()) continue;
      stations.append(station);
    }

    if (stations.isEmpty()) return fail("Список станций пуст");
    emit stationsReceived(stations);
    reply->deleteLater();
  });
}

void StationService::loadStationTracks(const QString &stationType, const QString &stationId,
                                       const QString &queueTrackId) {
  if (m_loading) return;

  if (!m_auth->isAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  m_loading = true;
  m_yandexClient->setToken(m_auth->token());
  QUrlQuery query;
  query.addQueryItem("settings2", "true");
  if (!queueTrackId.isEmpty()) query.addQueryItem("queue", queueTrackId);

  const auto path = QStringLiteral("/rotor/station/%1:%2/tracks?%3")
                        .arg(stationType)
                        .arg(stationId)
                        .arg(query.toString(QUrl::FullyEncoded));
  QNetworkReply *reply = m_yandexClient->get(path);

  if (reply == nullptr) {
    m_loading = false;
    emit errorOccurred("Не удалось создать запрос станции");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    auto fail = [&](const QString &msg) {
      emit errorOccurred(msg);
      reply->deleteLater();
    };
    m_loading = false;
    const QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) return fail(reply->errorString());
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
      return fail("Некорректный ответ станции");
    const QJsonObject result = unwrapResult(document);
    if (result.isEmpty()) return fail("Ответ станции пуст");
    const QString batchId = result.value("batchId").toString().trimmed();
    const QJsonArray sequence = result.value("sequence").toArray();
    if (sequence.isEmpty()) return fail("Станция не вернула треки");
    QList<Track> tracks;
    for (const QJsonValue &value : sequence) {
      if (!value.isObject()) continue;
      const QJsonObject item = value.toObject();
      const QJsonObject trackObject = item.value("track").toObject();
      if (trackObject.isEmpty()) continue;
      const QString type = item.value("type").toString();
      if (!type.isEmpty() && type != "track") continue;
      const Track track = parseTrack(trackObject);
      if (!track.id.isEmpty()) tracks.append(track);
    }

    if (tracks.isEmpty()) return fail("В ответе станции нет корректных треков");
    emit stationTracksReceived(tracks, batchId);
    reply->deleteLater();
  });
}

void StationService::loadMoreStationTracks(const QString &stationType, const QString &stationId,
                                           const QString &queueTrackId) {
  const QString trackId = queueTrackId.trimmed();
  if (trackId.isEmpty()) {
    emit errorOccurred("Идентификатор последнего трека для продолжения не задан");
    return;
  }
  loadStationTracks(stationType, stationId, trackId);
}

void StationService::sendFeedback(const QString &stationType, const QString &stationId,
                                  const QString &event, const QString &trackId,
                                  const QString &batchId, qint64 totalPlayedSeconds) {
  if (!m_auth->isAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString type = event.trimmed();
  const QString id = trackId.trimmed();
  const QString batch = batchId.trimmed();
  if (type.isEmpty()) return;
  if (id.isEmpty()) {
    emit errorOccurred("Для feedback не указан trackId");
    return;
  }
  if (batch.isEmpty()) {
    emit errorOccurred("Для feedback не указан batchId");
    return;
  }
  if (type != "trackStarted" && type != "trackFinished" && type != "skip") {
    emit errorOccurred("Неподдерживаемый тип feedback");
    return;
  }

  m_yandexClient->setToken(m_auth->token());
  QJsonObject body;
  body.insert("type", type);
  body.insert("timestamp", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
  body.insert("trackId", id);
  if (type == "trackFinished" || type == "skip") {
    body.insert("totalPlayedSeconds", qMax(0LL, totalPlayedSeconds));
  }

  QUrlQuery query;
  query.addQueryItem(QStringLiteral("batch-id"), batch);

  const auto path = QStringLiteral("/rotor/station/%1:%2/feedback?%3")
                        .arg(stationType)
                        .arg(stationId)
                        .arg(query.toString(QUrl::FullyEncoded));
  QNetworkReply *reply = m_yandexClient->post(path, body);

  if (reply == nullptr) {
    emit errorOccurred("Не удалось создать запрос feedback");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply, type]() {
    const QByteArray response = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
      QString message = reply->errorString();
      if (!response.isEmpty()) message += " | " + QString::fromUtf8(response);
      emit errorOccurred(message);
      reply->deleteLater();
      return;
    }
    emit feedbackSent(type);
    reply->deleteLater();
  });
}

QList<Track> StationService::parseSessionSequence(const QJsonArray &sequence,
                                                  const StationService *self) {
  QList<Track> tracks;
  for (const QJsonValue &value : sequence) {
    if (!value.isObject()) continue;
    const QJsonObject item = value.toObject();
    const QJsonObject trackObject = item.value("track").toObject();
    if (trackObject.isEmpty()) continue;
    const QString type = item.value("type").toString();
    if (!type.isEmpty() && type != "track") continue;
    const Track track = self->parseTrack(trackObject);
    if (!track.id.isEmpty()) tracks.append(track);
  }
  return tracks;
}

void StationService::startStationSession(const QString &stationType, const QString &stationTag) {
  const QString type = stationType.trimmed();
  const QString tag = stationTag.trimmed();
  if (type.isEmpty() || tag.isEmpty()) {
    emit errorOccurred("Некорректная станция для сессии");
    return;
  }
  if (!m_auth->isAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  m_yandexClient->setToken(m_auth->token());
  QJsonObject settings2;
  settings2.insert("language", "russian");
  settings2.insert("diversity", "high");
  settings2.insert("mood", 0.5);
  settings2.insert("energy", 0.25);

  QJsonObject body;
  body.insert("includeTracksInResponse", true);
  body.insert("interactive", true);
  body.insert("seeds", QJsonArray{QStringLiteral("%1:%2").arg(type, tag)});
  body.insert("settings2", settings2);

  QNetworkReply *reply = m_yandexClient->post(QStringLiteral("/rotor/session/new"), body);
  if (reply == nullptr) {
    emit errorOccurred("Не удалось создать запрос сессии станции");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    auto fail = [&](const QString &msg) {
      emit errorOccurred(msg);
      reply->deleteLater();
    };
    const QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) return fail(reply->errorString());
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
      return fail("Некорректный ответ начала сессии");
    const QJsonObject result = unwrapResult(document);
    if (result.isEmpty()) return fail("Ответ начала сессии пуст");

    const QString sessionId = result.value("radioSessionId").toString().trimmed();
    const QString batchId = result.value("batchId").toString().trimmed();
    const QList<Track> tracks =
        parseSessionSequence(result.value("sequence").toArray(), this);
    if (sessionId.isEmpty() || tracks.isEmpty())
      return fail("Яндекс Музыка не вернула сессию с треками");

    m_sessionId = sessionId;
    emit sessionStarted(sessionId, batchId, tracks);
    reply->deleteLater();
  });
}

void StationService::loadMoreStationSession(const QStringList &queueTokens) {
  if (m_sessionId.isEmpty()) {
    emit errorOccurred("Нет активной сессии станции");
    return;
  }
  if (!m_auth->isAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  QStringList tokenList;
  for (const QString &token : queueTokens) {
    if (!token.trimmed().isEmpty()) {
      tokenList.append(token.trimmed());
    }
  }
  if (tokenList.isEmpty()) {
    emit errorOccurred("Нет очереди для продолжения сессии станции");
    return;
  }

  m_yandexClient->setToken(m_auth->token());

  // Ротор не возвращает уже сыгранные треки, если передать всю очередь
  // вида "<trackId>:<albumId>", накопленную по сессии. Фидбек уходит
  // отдельными запросами, здесь достаточно пустого массива.
  QJsonArray queue;
  for (const QString &token : tokenList) {
    queue.append(token);
  }

  QJsonObject body;
  body.insert("feedbacks", QJsonArray());
  body.insert("queue", queue);

  const auto path = QStringLiteral("/rotor/session/%1/tracks").arg(m_sessionId);
  QNetworkReply *reply = m_yandexClient->post(path, body);
  if (reply == nullptr) {
    emit errorOccurred("Не удалось создать запрос продолжения сессии");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    auto fail = [&](const QString &msg) {
      emit errorOccurred(msg);
      reply->deleteLater();
    };
    const QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) return fail(reply->errorString());
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
      return fail("Некорректный ответ продолжения сессии");
    const QJsonObject result = unwrapResult(document);
    if (result.isEmpty()) return fail("Ответ продолжения сессии пуст");

    const QString batchId = result.value("batchId").toString().trimmed();
    const QList<Track> tracks =
        parseSessionSequence(result.value("sequence").toArray(), this);
    if (tracks.isEmpty()) return fail("Яндекс Музыка не вернула следующие треки");

    emit moreSessionTracksReceived(batchId, tracks);
    reply->deleteLater();
  });
}

Track StationService::parseTrack(const QJsonObject &object) const {
  Track track;
  track.id = parseId(object);
  track.title = object.value("title").toString();
  track.coverUri = object.value("coverUri").toString();
  track.durationMs = object.value("durationMs").toInt();

  const QJsonArray artists = object.value("artists").toArray();
  for (const QJsonValue &value : artists) {
    if (!value.isObject()) continue;
    const QJsonObject artistObject = value.toObject();
    Artist artist;
    artist.id = parseId(artistObject);
    artist.name = artistObject.value("name").toString();
    if (!artist.id.isEmpty() || !artist.name.isEmpty()) track.artists.append(artist);
  }

  const QJsonArray albums = object.value("albums").toArray();
  for (const QJsonValue &value : albums) {
    if (!value.isObject()) continue;
    const QJsonObject albumObject = value.toObject();
    Album album;
    album.id = parseId(albumObject);
    album.title = albumObject.value("title").toString();
    album.coverUri = albumObject.value("coverUri").toString();
    album.year = albumObject.value("year").toInt();
    if (!album.id.isEmpty() || !album.title.isEmpty()) track.albums.append(album);
  }
  return track;
}