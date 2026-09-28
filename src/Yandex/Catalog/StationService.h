#pragma once

#include "../../Models/Station.h"
#include "../../Models/Track.h"
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

class YandexAuth;
class YandexClient;

class StationService : public QObject {
  Q_OBJECT

public:
  explicit StationService(YandexAuth *auth, QObject *parent = nullptr);

  void loadStations();

  void loadStationTracks(const QString &stationType, const QString &stationId,
                         const QString &queueTrackId = {});

  void loadMoreStationTracks(const QString &stationType, const QString &stationId,
                             const QString &queueTrackId);

  // Сессионный ротор: POST /rotor/session/new и /rotor/session/<id>/tracks.
  void startStationSession(const QString &stationType, const QString &stationTag);

  // queueTokens — накопленная очередь вида "<trackId>:<albumId>" всех сыгранных
  // треков. Ротор не возвращает их повторно, если передать очередь целиком.
  void loadMoreStationSession(const QStringList &queueTokens);

  void sendFeedback(const QString &stationType, const QString &stationId, const QString &event,
                    const QString &trackId, const QString &batchId, qint64 totalPlayedSeconds = 0);

signals:
  void stationsReceived(const QList<Station> &stations);
  void stationTracksReceived(const QList<Track> &tracks, const QString &batchId);
  void sessionStarted(const QString &sessionId, const QString &batchId,
                      const QList<Track> &tracks);
  void moreSessionTracksReceived(const QString &batchId, const QList<Track> &tracks);
  void feedbackSent(const QString &event);
  void errorOccurred(const QString &message);

private:
  Track parseTrack(const QJsonObject &object) const;

  static QList<Track> parseSessionSequence(const QJsonArray &sequence,
                                           const StationService *self);

  YandexAuth *m_auth = nullptr;
  YandexClient *m_yandexClient = nullptr;
  bool m_loading = false;
  QString m_sessionId;
};