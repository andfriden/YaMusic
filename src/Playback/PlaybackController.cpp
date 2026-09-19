#include "PlaybackController.h"

#include "../MediaControls/MediaControlsFactory.h"
#include "../Player/PlayerService.h"
#include "../Yandex/Catalog/TrackService.h"

#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>

static QString coverCacheDir() {
  return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
         QStringLiteral("/covers");
}

static QString coverFilePath(const QString &trackId) {
  return coverCacheDir() + QStringLiteral("/") + trackId + QStringLiteral(".jpg");
}

PlaybackController::PlaybackController(TrackService *trackService, PlayerService *playerService,
                                       QueueService *queueService, QObject *parent)
    : QObject(parent), m_trackService(trackService), m_playerService(playerService),
      m_queueService(queueService), m_coverNetwork(new QNetworkAccessManager(this)) {
  Q_ASSERT(m_trackService != nullptr);
  Q_ASSERT(m_playerService != nullptr);
  Q_ASSERT(m_queueService != nullptr);

  setupSystemMediaControls();

  connect(m_playerService, &PlayerService::playbackStarted, this, [this]() {
    // playbackStarted означает, что новый медиа-источник действительно
    // начал играть. Это правильный момент, чтобы восстановить позицию,
    // сохранённую перед сбоем сетевого стрима.

    if (m_recoveringPlayback && m_recoveryPositionPending &&
        m_currentTrack.id == m_recoveryTrackId) {
      const qint64 recoveryPosition = m_recoveryPosition;
      m_recoveringPlayback = false;
      m_recoveryPositionPending = false;
      m_recoveryPosition = 0;
      m_recoveryTrackId.clear();

      if (recoveryPosition > 0)
        m_playerService->seek(recoveryPosition);

    } else {
      m_recoveringPlayback = false;
      m_recoveryPositionPending = false;
      m_recoveryPosition = 0;
      m_recoveryTrackId.clear();
    }

    setState(Playing);
  });
  connect(m_playerService, &PlayerService::playbackPaused, this, [this]() { setState(Paused); });

  connect(m_playerService, &PlayerService::playbackStopped, this, [this]() {
    // При восстановлении сломанного сетевого стрима состояние контроллера
    // не трогаем: QMediaPlayer может временно уйти в StoppedState, пока
    // старый источник заменяется новым.

    if (!m_recoveringPlayback)
      setState(Stopped);
  });

  connect(m_playerService, &PlayerService::playbackFinished, this,
          &PlaybackController::handlePlaybackFinished);

  connect(m_playerService, &PlayerService::errorOccurred, this, [this](const QString &message) {
    // Без текущего трека восстанавливать нечего.

    if (m_currentTrack.id.isEmpty()) {
      setState(Error);
      emit playbackError(message);
      return;
    }

    // FFmpeg может несколько раз сообщить одну и ту же сетевую ошибку,
    // пока старый сокет закрывается. Восстановление запускаем только по первой.

    if (m_recoveringPlayback)
      return;

    // Запоминаем точную позицию перед заменой протухшего stream URL.

    m_recoveryPosition = m_playerService->position();
    m_recoveryTrackId = m_currentTrack.id;
    m_recoveryPositionPending = true;
    m_recoveringPlayback = true;
    setState(Loading);

    m_trackService->loadStreamInfo(m_currentTrack.id);
  });

  connect(m_trackService, &TrackService::streamUrlReceived, this,
          [this](const QString &trackId, const QString &url) {
            // Ответы для треков, которые уже не текущие, игнорируем.

            if (trackId.isEmpty() || trackId != m_currentTrack.id)
              return;

            if (url.trimmed().isEmpty()) {
              const bool recovering = m_recoveringPlayback;
              m_recoveringPlayback = false;
              m_recoveryPositionPending = false;
              m_recoveryPosition = 0;
              m_recoveryTrackId.clear();
              setState(Error);

              emit playbackError(recovering ? QStringLiteral("Не удалось обновить Stream URL")
                                            : QStringLiteral("Stream URL пуст"));
              return;
            }

            // Обычное воспроизведение.

            if (!m_recoveringPlayback) {
              handleStreamUrl(trackId, url);
              return;
            }

            // Восстановление. Здесь seek делать нельзя: QMediaPlayer только
            // получил новый источник, длительность и позиция могут быть ещё
            // недоступны. Держим состояние восстановления до playbackStarted.

            playStream(trackId, url);
          });

  connect(m_trackService, &TrackService::errorOccurred, this, [this](const QString &message) {
    const bool recovering = m_recoveringPlayback;
    m_recoveringPlayback = false;
    m_recoveryPositionPending = false;
    m_recoveryPosition = 0;
    m_recoveryTrackId.clear();
    setState(Error);

    emit playbackError(recovering
                           ? QStringLiteral("Не удалось восстановить воспроизведение: ") + message
                           : message);
  });

  connect(m_queueService, &QueueService::repeatModeChanged, this,
          &PlaybackController::repeatModeChanged);

  connect(m_queueService, &QueueService::shuffleChanged, this,
          &PlaybackController::shuffleChanged);
}

Track PlaybackController::currentTrack() const {
  return m_currentTrack;
}

PlaybackController::PlaybackState PlaybackController::state() const {
  return m_state;
}

QueueService *PlaybackController::queueService() const {
  return m_queueService;
}

SystemMediaControls *PlaybackController::systemMediaControls() const {
  return m_systemMediaControls.get();
}

void PlaybackController::playTrack(const Track &track) {
  if (track.id.isEmpty()) {
    setState(Error);
    emit playbackError("Некорректный трек");
    return;
  }

  // Явный выбор трека отменяет незавершённое восстановление.

  m_recoveringPlayback = false;
  m_recoveryPositionPending = false;
  m_recoveryPosition = 0;
  m_recoveryTrackId.clear();

  // Новый трек — новый отчёт прослушивания.

  resetReportState();

  const QList<Track> queueTracks = m_queueService->tracks();

  for (int i = 0; i < queueTracks.size(); ++i) {
    if (queueTracks.at(i).id == track.id) {
      m_queueService->setCurrentIndex(i);
      break;
    }
  }

  m_currentTrack = track;
  emit currentTrackChanged();
  setState(Loading);

  m_trackService->loadStreamInfo(track.id);
}

void PlaybackController::playFromSource(const QList<Track> &tracks, int index,
                                        const QString &sourceTitle, const QString &sourceType) {
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

  if (!sourceTitle.isEmpty())
    m_queueService->setSource(sourceTitle, sourceType);

  playTrack(track);
}

void PlaybackController::playQueue() {
  const Track track = m_queueService->currentTrack();

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
  // Явный стоп отменяет незавершённое восстановление.

  m_recoveringPlayback = false;
  m_recoveryPositionPending = false;
  m_recoveryPosition = 0;
  m_recoveryTrackId.clear();
  maybeReportPlayback();
  m_playerService->stop();
}

bool PlaybackController::next() {
  if (m_queueService->hasNext()) {
    m_queueService->next();
    return playQueueCurrentTrack();
  }

  if (m_queueService->repeatMode() == QueueService::RepeatAll && m_queueService->count() > 0) {
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

  if (m_queueService->repeatMode() == QueueService::RepeatAll && m_queueService->count() > 0) {
    m_queueService->setCurrentIndex(m_queueService->count() - 1);
    return playQueueCurrentTrack();
  }
  return false;
}

void PlaybackController::setRepeatMode(QueueService::RepeatMode mode) {
  m_queueService->setRepeatMode(mode);
}

QueueService::RepeatMode PlaybackController::repeatMode() const {
  return m_queueService->repeatMode();
}

void PlaybackController::cycleRepeatMode() {
  m_queueService->cycleRepeatMode();
}

bool PlaybackController::shuffleEnabled() const {
  return m_queueService->shuffleEnabled();
}

void PlaybackController::setShuffleEnabled(bool enabled) {
  m_queueService->setShuffleEnabled(enabled);
}

void PlaybackController::toggleShuffle() {
  m_queueService->toggleShuffle();
}

void PlaybackController::setState(PlaybackState state) {
  if (m_state == state)
    return;

  m_state = state;
  emit stateChanged();
}

void PlaybackController::handleStreamUrl(const QString &trackId, const QString &url) {
  if (trackId.isEmpty() || trackId != m_currentTrack.id)
    return;

  if (url.trimmed().isEmpty()) {
    setState(Error);
    emit playbackError("Stream URL пуст");
    return;
  }

  playStream(trackId, url);
}

// Играем стрим напрямую из сети. Раньше аудио качалось в локальный файл
// кэша (обход TLS-проблем FFmpeg на macOS), но это держало плеер в
// состоянии Loading до полной загрузки трека и ломало авто-подгрузку
// следующих партий My Wave. Теперь источник отдаётся сразу плееру.
void PlaybackController::playStream(const QString &trackId, const QString &streamUrl) {
  if (trackId.isEmpty() || streamUrl.isEmpty()) {
    setState(Error);
    emit playbackError("Stream play: empty args");
    return;
  }

  if (streamUrl != m_playerService->currentUrl())
    m_playerService->playUrl(streamUrl);
  else
    m_playerService->play();
}

bool PlaybackController::playQueueCurrentTrack() {
  const Track track = m_queueService->currentTrack();

  if (track.id.isEmpty())
    return false;

  playTrack(track);
  return true;
}

void PlaybackController::handlePlaybackFinished() {
  maybeReportPlayback();

  const QueueService::RepeatMode mode = m_queueService->repeatMode();

  // RepeatOne: переигрываем текущий трек.

  if (mode == QueueService::RepeatOne) {
    if (playQueueCurrentTrack())
      return;

    setState(Stopped);
    return;
  }

  // Есть следующий трек в очереди — играем его.

  if (m_queueService->hasNext()) {
    m_queueService->next();

    if (playQueueCurrentTrack())
      return;
  }

  // RepeatAll: зацикливаемся на начало.

  if (mode == QueueService::RepeatAll && m_queueService->count() > 0) {
    m_queueService->setCurrentIndex(0);

    if (playQueueCurrentTrack())
      return;
  }

  // Очередь кончилась.

  const QString sourceType = m_queueService->sourceType();
  const QString sourceTitle = m_queueService->sourceTitle();
  setState(Stopped);
  emit playlistExhausted(sourceType, sourceTitle);
}

static QString normalizeCoverUri(const QString &uri) {
  if (uri.isEmpty())
    return {};

  QString result = uri;
  result.replace(QStringLiteral("%%"), QStringLiteral("1000x1000"));

  if (!result.contains(QStringLiteral("://")))
    result.prepend(QStringLiteral("https://"));
  return result;
}

static SystemMediaControls::Metadata makeMediaMetadata(const Track &track) {
  SystemMediaControls::Metadata md;
  md.title = track.title;
  md.artist = track.artists.isEmpty() ? QString() : track.artists.first().name;
  md.album = track.albums.isEmpty() ? QString() : track.albums.first().title;
  md.durationMs = track.durationMs;
  md.trackId = track.id;

  // MPRIS требует file:// — используем локальный кэш обложки.

  const QString cached = coverFilePath(track.id);

  if (QFile::exists(cached))
    md.coverUrl = QUrl::fromLocalFile(cached).toString();
  else
    md.coverUrl = normalizeCoverUri(track.coverUri);
  return md;
}

static QString mprisLoopStatus(QueueService::RepeatMode mode) {
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
  m_systemMediaControls = MediaControlsFactory::create(this);

  // Медиа-контролы не обязательны (нет реализации под платформу).
  if (m_systemMediaControls == nullptr)
    return;

  // Управление из системы → PlaybackController.

  connect(m_systemMediaControls.get(), &SystemMediaControls::playRequested, this,
          &PlaybackController::resume);

  connect(m_systemMediaControls.get(), &SystemMediaControls::pauseRequested, this,
          &PlaybackController::pause);

  connect(m_systemMediaControls.get(), &SystemMediaControls::togglePlayPauseRequested, this,
          [this]() {
            if (state() == Playing)
              pause();
            else
              resume();
          });

  connect(m_systemMediaControls.get(), &SystemMediaControls::nextRequested, this,
          &PlaybackController::next);

  connect(m_systemMediaControls.get(), &SystemMediaControls::previousRequested, this,
          &PlaybackController::previous);

  connect(m_systemMediaControls.get(), &SystemMediaControls::seekRequested, this,
          [this](qint64 positionMs) { m_playerService->seek(positionMs); });

  // Состояние воспроизведения → система.

  connect(this, &PlaybackController::currentTrackChanged, this, [this]() {
    if (!m_systemMediaControls->isEnabled())
      m_systemMediaControls->setEnabled(true);

    const auto md = makeMediaMetadata(m_currentTrack);
    m_systemMediaControls->setMetadata(md);
    m_systemMediaControls->setDuration(md.durationMs);
    fetchCurrentCover();
  });

  connect(this, &PlaybackController::stateChanged, this, [this]() {
    switch (m_state) {
    case Playing:
      m_systemMediaControls->setPlaybackStatus(SystemMediaControls::PlaybackStatus::Playing);
      break;
    case Paused:
      m_systemMediaControls->setPlaybackStatus(SystemMediaControls::PlaybackStatus::Paused);
      break;
    default:
      m_systemMediaControls->setPlaybackStatus(SystemMediaControls::PlaybackStatus::Stopped);
      break;
    }
  });

  connect(this, &PlaybackController::repeatModeChanged, this, [this]() {
    m_systemMediaControls->setLoopStatus(mprisLoopStatus(m_queueService->repeatMode()));
  });

  connect(this, &PlaybackController::shuffleChanged, this, [this]() {
    m_systemMediaControls->setShuffle(m_queueService->shuffleEnabled());
  });

  // Позиция — от PlayerService к системе, обновляется примерно
  // 4 раза в секунду из AppController.

  m_systemMediaControls->setEnabled(true);
}

void PlaybackController::fetchCurrentCover() {
  if (m_currentTrack.id.isEmpty() || m_currentTrack.coverUri.isEmpty())
    return;

  const QString cached = coverFilePath(m_currentTrack.id);

  // костыль: повторные запуски не обновляют уже скачанные обложки,
  // если сервер поменял картинку — завёл баг
  if (QFile::exists(cached))
    return;

  const QString url = normalizeCoverUri(m_currentTrack.coverUri);
  m_pendingCoverUri = m_currentTrack.coverUri;
  QNetworkRequest request{QUrl(url)};
  request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("YaMusic/1.0 (Qt)"));
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                       QNetworkRequest::NoLessSafeRedirectPolicy);
  QNetworkReply *reply = m_coverNetwork->get(request);

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
      return;

    const QByteArray data = reply->readAll();

    if (data.isEmpty())
      return;

    QDir dir;

    if (!dir.mkpath(coverCacheDir()))
      return;

    QFile file(coverFilePath(m_currentTrack.id));

    if (!file.open(QIODevice::WriteOnly))
      return;

    file.write(data);
    file.close();

    // Перепубликуем метаданные, чтобы MPRIS-клиенты подхватили file:// обложку.

    const auto md = makeMediaMetadata(m_currentTrack);
    m_systemMediaControls->setMetadata(md);
  });
}

void PlaybackController::setUidProvider(const std::function<QString()> &provider) {
  m_uidProvider = provider;
}

// Отправка факта прослушивания, когда трек дослушан до порога:
// не менее 30 секунд или 50% длительности. Вызывается при паузе,
// остановке и окончании трека.
void PlaybackController::maybeReportPlayback() {
  if (m_reportSubmitted || m_currentTrack.id.isEmpty())
    return;

  if (!m_uidProvider)
    return;

  const QString uid = m_uidProvider();

  if (uid.isEmpty())
    return;

  const qint64 positionMs = m_playerService->position();
  const qint64 durationMs = m_playerService->duration();

  if (durationMs <= 0)
    return;

  const bool reachedThreshold = positionMs >= 30000 || positionMs * 2 >= durationMs;

  if (!reachedThreshold)
    return;

  m_reportSubmitted = true;
  QString albumId;

  if (!m_currentTrack.albums.isEmpty())
    albumId = m_currentTrack.albums.first().id;

  m_trackService->reportPlayback(m_currentTrack.id, albumId, uid,
                                 int(durationMs / 1000), int(positionMs / 1000),
                                 int(positionMs / 1000));
}

void PlaybackController::resetReportState() {
  m_reportSubmitted = false;
}