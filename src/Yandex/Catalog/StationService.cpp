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

// TODO(#171): вынести общий парсинг трека из StationService и YandexPersonal
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