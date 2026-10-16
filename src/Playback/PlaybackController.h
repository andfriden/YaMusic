#pragma once

#include <QObject>
#include <QPointer>
#include <QString>
#include <QTimer>
#include <functional>
#include <memory>

#include "../MediaControls/SystemMediaControls.h"
#include "../Models/Track.h"
#include "../Queue/QueueService.h"

class PlayerService;
class TrackService;
class TrackCache;
class StreamProxy;
class AudioQualityController;
class CensorService;
class QNetworkAccessManager;
class QNetworkReply;

namespace Playback {
class Preloader;
}

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
      AudioQualityController *qualityController,
      CensorService *censorService,
      QObject *parent = nullptr);

  Track currentTrack() const;
  PlaybackState state() const;
  QueueService *queueService() const;
  SystemMediaControls *systemMediaControls() const;

  // Кэш скачанных стримов и предзагрузка: настройками и интерфейсом
  // управляет PreloadController.
  TrackCache *trackCache() const;
  Playback::Preloader *preloader() const;

  // Кодек и битрейт текущего стрима (для отображения качества).
  QString currentCodec() const;
  int currentBitrate() const;

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
  void streamQualityInfoChanged();

private:
  void setState(PlaybackState state);

  void handlePlaybackFinished();

  // Воспроизведение трека из кэша: без обращения к сети.
  void playCachedFile(const Track &track, const QString &path);

  // Пора ли переходить в следующий трек перекрёстным затуханием, и сам
  // переход: заводим следующий трек до того, как текущий доиграет.
  void checkCrossfade();
  void beginCrossfade(const Track &nextTrack, const QString &path);

  // Что держать готовым: очередь после текущего трека.
  void updatePreloadWindow();

  // Заменён ли бы этот трек на версию без цензуры: такие в кэш не годятся.
  bool wouldReplaceForCensor(const Track &track) const;

  void handleStreamUrl(
      const QString &trackId,
      const QString &url);

  bool playQueueCurrentTrack();

  void setupSystemMediaControls();

  void fetchCurrentCover();

  void playStream(
      const QString &trackId,
      const QString &streamUrl);

  void startStreamProxy(
      const QString &trackId,
      QNetworkReply *reply);

  void abortStreamProxy();

  void releaseStaleProxies();

  void maybeReportPlayback();
  void resetReportState();

  ~PlaybackController() override;

private:
  TrackService *m_trackService = nullptr;
  PlayerService *m_playerService = nullptr;
  QueueService *m_queueService = nullptr;
  AudioQualityController *m_qualityController = nullptr;
  CensorService *m_censorService = nullptr;

  Track m_currentTrack;
  PlaybackState m_state = Idle;
  QString m_currentCodec;
  int m_currentBitrate = 0;

  std::unique_ptr<SystemMediaControls> m_systemMediaControls;

  QNetworkAccessManager *m_coverNetwork = nullptr;
  QNetworkAccessManager *m_streamNetwork = nullptr;

  std::unique_ptr<TrackCache> m_trackCache;
  Playback::Preloader *m_preloader = nullptr;

  // Проверка «пора переходить» и признак идущего перехода.
  QTimer m_crossfadeTimer;
  bool m_crossfading = false;
  QString m_pendingCoverUri;

  // Буферизованный сетевой поток (обход TLS-проблем FFmpeg на macOS).
  // Старые прокси не удаляются сразу: ffmpeg может ещё читать из них
  // в своём потоке. Гарантированно освобождаются при destroy() и
  // при старте реального воспроизведения следующего трека.
  StreamProxy *m_streamProxy = nullptr;
  QList<StreamProxy *> m_staleProxies;
  QString m_pendingStreamTrackId;

  // Отправка статистики прослушивания.
  std::function<QString()> m_uidProvider;
  bool m_reportSubmitted = false;
};