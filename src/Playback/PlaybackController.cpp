#include "PlaybackController.h"

#include "../MediaControls/MediaControlsFactory.h"
#include "../Player/PlayerService.h"
#include "../Player/StreamProxy.h"
#include "../Yandex/Catalog/TrackService.h"

#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>

#include <utility>

static QString coverCacheDir() {
  return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
         QStringLiteral("/covers");
}

static QString coverFilePath(const QString &trackId) {
  return coverCacheDir() +
         QStringLiteral("/") +
         trackId +
         QStringLiteral(".jpg");
}

PlaybackController::PlaybackController(
    TrackService *trackService,
    PlayerService *playerService,
    QueueService *queueService,
    QObject *parent)
    : QObject(parent),
      m_trackService(trackService),
      m_playerService(playerService),
      m_queueService(queueService),
      m_coverNetwork(new QNetworkAccessManager(this)),
      m_streamNetwork(new QNetworkAccessManager(this)) {
  Q_ASSERT(m_trackService != nullptr);
  Q_ASSERT(m_playerService != nullptr);
  Q_ASSERT(m_queueService != nullptr);

  setupSystemMediaControls();

  connect(
      m_playerService,
      &PlayerService::playbackStarted,
      this,
      [this]() {
        setState(Playing);
        releaseStaleProxies();
      });

  connect(
      m_playerService,
      &PlayerService::playbackPaused,
      this,
      [this]() {
        setState(Paused);
      });

  connect(
      m_playerService,
      &PlayerService::playbackStopped,
      this,
      [this]() {
        setState(Stopped);
      });

  connect(
      m_playerService,
      &PlayerService::playbackFinished,
      this,
      &PlaybackController::handlePlaybackFinished);

  connect(
      m_playerService,
      &PlayerService::errorOccurred,
      this,
      [this](const QString &message) {
        // Даже если воспроизведение завершилось ошибкой
        // (например, сетевой/TLS/FFmpeg error), нужно
        // отправить факт прослушивания, если был достигнут
        // минимальный порог.
        maybeReportPlayback();

        setState(Error);
        emit playbackError(message);
      });

  connect(
      m_trackService,
      &TrackService::streamUrlReceived,
      this,
      [this](
          const QString &trackId,
          const QString &url) {
        handleStreamUrl(trackId, url);
      });

  connect(
      m_trackService,
      &TrackService::errorOccurred,
      this,
      [this](const QString &message) {
        setState(Error);
        emit playbackError(message);
      });

  connect(
      m_queueService,
      &QueueService::repeatModeChanged,
      this,
      &PlaybackController::repeatModeChanged);

  connect(
      m_queueService,
      &QueueService::shuffleChanged,
      this,
      &PlaybackController::shuffleChanged);
}

PlaybackController::~PlaybackController() {
  for (StreamProxy *proxy : std::as_const(m_staleProxies)) {
    proxy->abort();
    proxy->deleteLater();
  }

  m_staleProxies.clear();
}

Track PlaybackController::currentTrack() const {
  return m_currentTrack;
}

PlaybackController::PlaybackState
PlaybackController::state() const {
  return m_state;
}

QueueService *PlaybackController::queueService() const {
  return m_queueService;
}

SystemMediaControls *
PlaybackController::systemMediaControls() const {
  return m_systemMediaControls.get();
}

void PlaybackController::playTrack(const Track &track) {
  if (track.id.isEmpty()) {
    setState(Error);
    emit playbackError("Некорректный трек");
    return;
  }

  const bool changingTrack =
      !m_currentTrack.id.isEmpty() &&
      m_currentTrack.id != track.id;

  if (changingTrack)
    maybeReportPlayback();

  resetReportState();

  const QList<Track> queueTracks =
      m_queueService->tracks();

  for (int i = 0; i < queueTracks.size(); ++i) {
    if (queueTracks.at(i).id == track.id) {
      m_queueService->setCurrentIndex(i);
      break;
    }
  }

  abortStreamProxy();

  m_currentTrack = track;
  m_playerService->setTrackDuration(track.durationMs);

  emit currentTrackChanged();

  setState(Loading);

  if (m_trackService == nullptr) {
    setState(Error);
    emit playbackError("TrackService недоступен");
    return;
  }

  m_trackService->loadStreamInfo(track.id);
}

void PlaybackController::playFromSource(
    const QList<Track> &tracks,
    int index,
    const QString &sourceTitle,
    const QString &sourceType) {
  if (index < 0 || index >= tracks.size()) {
    setState(Error);
    emit playbackError("Некорректный индекс трека");
    return;
  }

  const Track track = tracks.at(index);

  if (track.id.isEmpty()) {
    setState(Error);
    emit playbackError("Некорректный трек");
    return;
  }

  m_queueService->clear();
  m_queueService->addTracks(tracks);
  m_queueService->setCurrentIndex(index);

  if (!sourceTitle.isEmpty()) {
    m_queueService->setSource(
        sourceTitle,
        sourceType);
  }

  playTrack(track);
}

void PlaybackController::playQueue() {
  const Track track =
      m_queueService->currentTrack();

  if (track.id.isEmpty()) {
    setState(Error);
    emit playbackError("Очередь пуста");
    return;
  }

  playTrack(track);
}

void PlaybackController::playCurrent() {
  if (m_currentTrack.id.isEmpty()) {
    playQueue();
    return;
  }

  playTrack(m_currentTrack);
}

void PlaybackController::pause() {
  maybeReportPlayback();
  m_playerService->pause();
}

void PlaybackController::resume() {
  m_playerService->play();
}

void PlaybackController::stop() {
  maybeReportPlayback();
  abortStreamProxy();
  m_playerService->stop();
}

bool PlaybackController::next() {
  if (m_queueService->hasNext()) {
    m_queueService->next();
    return playQueueCurrentTrack();
  }

  if (m_queueService->repeatMode() ==
          QueueService::RepeatAll &&
      m_queueService->count() > 0) {
    m_queueService->setCurrentIndex(0);
    return playQueueCurrentTrack();
  }

  return false;
}

bool PlaybackController::previous() {
  if (m_queueService->hasPrevious()) {
    m_queueService->previous();
    return playQueueCurrentTrack();
  }

  if (m_queueService->repeatMode() ==
          QueueService::RepeatAll &&
      m_queueService->count() > 0) {
    m_queueService->setCurrentIndex(
        m_queueService->count() - 1);

    return playQueueCurrentTrack();
  }

  return false;
}

void PlaybackController::setRepeatMode(
    QueueService::RepeatMode mode) {
  m_queueService->setRepeatMode(mode);
}

QueueService::RepeatMode
PlaybackController::repeatMode() const {
  return m_queueService->repeatMode();
}

void PlaybackController::cycleRepeatMode() {
  m_queueService->cycleRepeatMode();
}

bool PlaybackController::shuffleEnabled() const {
  return m_queueService->shuffleEnabled();
}

void PlaybackController::setShuffleEnabled(
    bool enabled) {
  m_queueService->setShuffleEnabled(enabled);
}

void PlaybackController::toggleShuffle() {
  m_queueService->toggleShuffle();
}

void PlaybackController::setState(
    PlaybackState state) {
  if (m_state == state)
    return;

  m_state = state;
  emit stateChanged();
}

void PlaybackController::handleStreamUrl(
    const QString &trackId,
    const QString &url) {
  if (trackId.isEmpty() ||
      trackId != m_currentTrack.id) {
    return;
  }

  if (url.trimmed().isEmpty()) {
    setState(Error);
    emit playbackError("Stream URL пуст");
    return;
  }

  if (m_playerService == nullptr) {
    setState(Error);
    emit playbackError(
        "PlayerService недоступен");
    return;
  }

  playStream(trackId, url);
}

void PlaybackController::playStream(
    const QString &trackId,
    const QString &streamUrl) {
  if (trackId.isEmpty() ||
      streamUrl.trimmed().isEmpty()) {
    setState(Error);
    emit playbackError(
        "Stream play: empty args");
    return;
  }

  abortStreamProxy();

  QNetworkRequest request{QUrl(streamUrl)};

  request.setHeader(
      QNetworkRequest::UserAgentHeader,
      QStringLiteral("YaMusic/1.0 (Qt)"));

  request.setAttribute(
      QNetworkRequest::RedirectPolicyAttribute,
      QNetworkRequest::NoLessSafeRedirectPolicy);

  QNetworkReply *reply =
      m_streamNetwork->get(request);

  reply->setReadBufferSize(1024 * 1024);

  startStreamProxy(trackId, reply);
}

void PlaybackController::startStreamProxy(
    const QString &trackId,
    QNetworkReply *reply) {
  auto *proxy = new StreamProxy(this);

  m_streamProxy = proxy;
  m_pendingStreamTrackId = trackId;

  if (reply)
    proxy->setReply(reply);

  connect(
      proxy,
      &StreamProxy::networkError,
      this,
      [this](const QString &message) {
        if (m_state == Error)
          return;

        setState(Error);

        emit playbackError(
            "Stream download failed: " + message);
      });

  m_playerService->playDevice(proxy);
}

void PlaybackController::abortStreamProxy() {
  if (m_streamProxy) {
    m_streamProxy->abort();
    m_staleProxies.append(m_streamProxy);
    m_streamProxy = nullptr;
  }

  m_pendingStreamTrackId.clear();
}

void PlaybackController::releaseStaleProxies() {
  for (StreamProxy *proxy : std::as_const(m_staleProxies))
    proxy->deleteLater();

  m_staleProxies.clear();
}

bool PlaybackController::playQueueCurrentTrack() {
  const Track track =
      m_queueService->currentTrack();

  if (track.id.isEmpty())
    return false;

  playTrack(track);
  return true;
}

void PlaybackController::handlePlaybackFinished() {
  maybeReportPlayback();

  const QueueService::RepeatMode mode =
      m_queueService->repeatMode();

  if (mode == QueueService::RepeatOne) {
    if (playQueueCurrentTrack())
      return;

    setState(Stopped);
    return;
  }

  if (m_queueService->hasNext()) {
    m_queueService->next();

    if (playQueueCurrentTrack())
      return;
  }

  if (mode == QueueService::RepeatAll &&
      m_queueService->count() > 0) {
    m_queueService->setCurrentIndex(0);

    if (playQueueCurrentTrack())
      return;
  }

  const QString sourceType =
      m_queueService->sourceType();

  const QString sourceTitle =
      m_queueService->sourceTitle();

  setState(Stopped);

  emit playlistExhausted(
      sourceType,
      sourceTitle);
}

static QString normalizeCoverUri(
    const QString &uri) {
  if (uri.isEmpty())
    return {};

  QString result = uri;

  result.replace(
      QStringLiteral("%%"),
      QStringLiteral("1000x1000"));

  if (!result.contains(QStringLiteral("://")))
    result.prepend(QStringLiteral("https://"));

  return result;
}

static SystemMediaControls::Metadata
makeMediaMetadata(const Track &track) {
  SystemMediaControls::Metadata metadata;

  metadata.title = track.title;

  metadata.artist =
      track.artists.isEmpty()
          ? QString()
          : track.artists.first().name;

  metadata.album =
      track.albums.isEmpty()
          ? QString()
          : track.albums.first().title;

  metadata.durationMs = track.durationMs;
  metadata.trackId = track.id;

  const QString cached =
      coverFilePath(track.id);

  if (QFile::exists(cached)) {
    metadata.coverUrl =
        QUrl::fromLocalFile(cached).toString();
  } else {
    metadata.coverUrl =
        normalizeCoverUri(track.coverUri);
  }

  return metadata;
}

static QString mprisLoopStatus(
    QueueService::RepeatMode mode) {
  switch (mode) {
  case QueueService::RepeatOff:
    return QStringLiteral("None");

  case QueueService::RepeatOne:
    return QStringLiteral("Track");

  case QueueService::RepeatAll:
    return QStringLiteral("Playlist");
  }

  return QStringLiteral("None");
}

void PlaybackController::setupSystemMediaControls() {
  m_systemMediaControls =
      MediaControlsFactory::create(this);

  if (m_systemMediaControls == nullptr)
    return;

  connect(
      m_systemMediaControls.get(),
      &SystemMediaControls::playRequested,
      this,
      &PlaybackController::resume);

  connect(
      m_systemMediaControls.get(),
      &SystemMediaControls::pauseRequested,
      this,
      &PlaybackController::pause);

  connect(
      m_systemMediaControls.get(),
      &SystemMediaControls::togglePlayPauseRequested,
      this,
      [this]() {
        if (state() == Playing)
          pause();
        else
          resume();
      });

  connect(
      m_systemMediaControls.get(),
      &SystemMediaControls::nextRequested,
      this,
      &PlaybackController::next);

  connect(
      m_systemMediaControls.get(),
      &SystemMediaControls::previousRequested,
      this,
      &PlaybackController::previous);

  connect(
      m_systemMediaControls.get(),
      &SystemMediaControls::seekRequested,
      this,
      [this](qint64 positionMs) {
        m_playerService->seek(positionMs);
      });

  connect(
      this,
      &PlaybackController::currentTrackChanged,
      this,
      [this]() {
        if (!m_systemMediaControls->isEnabled())
          m_systemMediaControls->setEnabled(true);

        const auto metadata =
            makeMediaMetadata(m_currentTrack);

        m_systemMediaControls->setMetadata(metadata);
        m_systemMediaControls->setDuration(
            metadata.durationMs);

        fetchCurrentCover();
      });

  connect(
      this,
      &PlaybackController::stateChanged,
      this,
      [this]() {
        switch (m_state) {
        case Playing:
          m_systemMediaControls->setPlaybackStatus(
              SystemMediaControls::PlaybackStatus::Playing);
          break;

        case Paused:
          m_systemMediaControls->setPlaybackStatus(
              SystemMediaControls::PlaybackStatus::Paused);
          break;

        default:
          m_systemMediaControls->setPlaybackStatus(
              SystemMediaControls::PlaybackStatus::Stopped);
          break;
        }
      });

  connect(
      this,
      &PlaybackController::repeatModeChanged,
      this,
      [this]() {
        m_systemMediaControls->setLoopStatus(
            mprisLoopStatus(
                m_queueService->repeatMode()));
      });

  connect(
      this,
      &PlaybackController::shuffleChanged,
      this,
      [this]() {
        m_systemMediaControls->setShuffle(
            m_queueService->shuffleEnabled());
      });

  m_systemMediaControls->setEnabled(true);
}

void PlaybackController::fetchCurrentCover() {
  if (m_currentTrack.id.isEmpty() ||
      m_currentTrack.coverUri.isEmpty()) {
    return;
  }

  const QString cached =
      coverFilePath(m_currentTrack.id);

  if (QFile::exists(cached))
    return;

  const QString url =
      normalizeCoverUri(m_currentTrack.coverUri);

  m_pendingCoverUri =
      m_currentTrack.coverUri;

  QNetworkRequest request{QUrl(url)};

  request.setHeader(
      QNetworkRequest::UserAgentHeader,
      QStringLiteral("YaMusic/1.0 (Qt)"));

  request.setAttribute(
      QNetworkRequest::RedirectPolicyAttribute,
      QNetworkRequest::NoLessSafeRedirectPolicy);

  QNetworkReply *reply =
      m_coverNetwork->get(request);

  connect(
      reply,
      &QNetworkReply::finished,
      this,
      [this, reply]() {
        reply->deleteLater();

        if (reply->error() !=
            QNetworkReply::NoError) {
          return;
        }

        const QByteArray data =
            reply->readAll();

        if (data.isEmpty())
          return;

        QDir dir;

        if (!dir.mkpath(coverCacheDir()))
          return;

        QFile file(
            coverFilePath(
                m_currentTrack.id));

        if (!file.open(QIODevice::WriteOnly))
          return;

        file.write(data);
        file.close();

        const auto metadata =
            makeMediaMetadata(m_currentTrack);

        m_systemMediaControls->setMetadata(metadata);
      });
}

void PlaybackController::setUidProvider(
    const std::function<QString()> &provider) {
  m_uidProvider = provider;
}

void PlaybackController::maybeReportPlayback() {
  if (m_reportSubmitted ||
      m_currentTrack.id.isEmpty()) {
    return;
  }

  if (!m_uidProvider)
    return;

  const QString uid =
      m_uidProvider();

  if (uid.isEmpty())
    return;

  const qint64 positionMs =
      m_playerService->position();

  const qint64 durationMs =
      m_playerService->duration();

  if (durationMs <= 0)
    return;

  const bool reachedThreshold =
      positionMs >= 30000 ||
      positionMs * 2 >= durationMs;

  if (!reachedThreshold)
    return;

  m_reportSubmitted = true;

  QString albumId;

  if (!m_currentTrack.albums.isEmpty())
    albumId =
        m_currentTrack.albums.first().id;

  m_trackService->reportPlayback(
      m_currentTrack.id,
      albumId,
      uid,
      int(durationMs / 1000),
      int(positionMs / 1000),
      int(positionMs / 1000));
}

void PlaybackController::resetReportState() {
  m_reportSubmitted = false;
}