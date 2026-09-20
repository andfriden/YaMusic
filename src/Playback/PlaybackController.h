#pragma once

#include <QObject>
#include <QString>
#include <functional>
#include <memory>

#include "../MediaControls/SystemMediaControls.h"
#include "../Models/Track.h"
#include "../Queue/QueueService.h"

class PlayerService;
class TrackService;
class QNetworkAccessManager;

class PlaybackController : public QObject {
  Q_OBJECT

public:
  enum PlaybackState {
    Idle,
    Loading,
    Playing,
    Paused,
    Stopped,
    Error
  };

  Q_ENUM(PlaybackState)

  explicit PlaybackController(
      TrackService *trackService,
      PlayerService *playerService,
      QueueService *queueService,
      QObject *parent = nullptr);

  Track currentTrack() const;
  PlaybackState state() const;
  QueueService *queueService() const;
  SystemMediaControls *systemMediaControls() const;

  void playTrack(const Track &track);

  void playFromSource(
      const QList<Track> &tracks,
      int index,
      const QString &sourceTitle = {},
      const QString &sourceType = {});

  void playQueue();
  void playCurrent();

  void pause();
  void resume();
  void stop();

  bool next();
  bool previous();

  void setRepeatMode(QueueService::RepeatMode mode);
  QueueService::RepeatMode repeatMode() const;
  void cycleRepeatMode();

  bool shuffleEnabled() const;
  void setShuffleEnabled(bool enabled);
  void toggleShuffle();

  // UID-провайдер для отправки статистики прослушивания.
  void setUidProvider(const std::function<QString()> &provider);

signals:
  void playlistExhausted(
      const QString &sourceType,
      const QString &sourceTitle);

  void currentTrackChanged();
  void stateChanged();

  void playbackError(const QString &message);

  void repeatModeChanged();
  void shuffleChanged();

private:
  void setState(PlaybackState state);

  void handlePlaybackFinished();

  void handleStreamUrl(
      const QString &trackId,
      const QString &url);

  bool playQueueCurrentTrack();

  void setupSystemMediaControls();

  void fetchCurrentCover();

  void playStream(
      const QString &trackId,
      const QString &streamUrl);

  void maybeReportPlayback();
  void resetReportState();

private:
  TrackService *m_trackService = nullptr;
  PlayerService *m_playerService = nullptr;
  QueueService *m_queueService = nullptr;

  Track m_currentTrack;
  PlaybackState m_state = Idle;

  std::unique_ptr<SystemMediaControls> m_systemMediaControls;

  QNetworkAccessManager *m_coverNetwork = nullptr;
  QString m_pendingCoverUri;

  // Отправка статистики прослушивания.
  std::function<QString()> m_uidProvider;
  bool m_reportSubmitted = false;
};