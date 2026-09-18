#include "../Playback/PlaybackController.h"
#include "../Player/PlayerService.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Personal/YandexPersonal.h"
#include "PersonalController.h"

void PersonalController::connectMyWave() {
  connect(m_yandexPersonal, &YandexPersonal::myWaveReceived, this,
          [this](const QList<Track> &tracks, const QString &batchId) {
            handleMyWaveReceived(tracks, batchId);
          });

  connect(m_yandexPersonal, &YandexPersonal::errorOccurred, this, [this](const QString &message) {
    if (m_loadingMyWave) {
      m_loadingMyWave = false;
      emit loadingMyWaveChanged();
    }

    if (m_loadingMoreMyWave) {
      m_loadingMoreMyWave = false;
      emit loadingMoreMyWaveChanged();
    }

    m_waitingForMoreMyWave = false;
    emit statusChanged(message);
  });
}

void PersonalController::loadMyWave() {
  if (m_loadingMyWave || m_loadingMoreMyWave) {
    return;
  }

  m_myWaveQueueActive = true;
  m_waitingForMoreMyWave = false;
  m_currentMyWaveTrackId.clear();
  m_myWaveTrackStarted = false;
  m_loadingMyWave = true;
  emit loadingMyWaveChanged();
  m_myWaveTrackBatches.clear();
  m_myWaveModel->clear();

  QueueService *queue = m_playbackController->queueService();
  queue->clear();
  queue->clearSource();

  emit statusChanged("Загрузка моей волны...");
  m_yandexPersonal->loadMyWave();
}

void PersonalController::loadMoreMyWave() {
  if (m_loadingMyWave || m_loadingMoreMyWave) {
    return;
  }

  const Track lastTrack = m_myWaveModel->lastTrack();

  if (lastTrack.id.isEmpty()) {
    emit statusChanged("Нет трека для продолжения My Wave");
    return;
  }

  m_loadingMoreMyWave = true;
  emit loadingMoreMyWaveChanged();
  m_waitingForMoreMyWave = true;
  emit statusChanged("Загрузка следующей части моей волны...");
  m_yandexPersonal->loadMoreMyWave(lastTrack.id);
}

void PersonalController::handleMyWaveReceived(const QList<Track> &tracks, const QString &batchId) {
  const QString trimmedBatchId = batchId.trimmed();

  if (tracks.isEmpty()) {
    const bool wasLoadingMore = m_loadingMoreMyWave;
    m_loadingMyWave = false;
    m_loadingMoreMyWave = false;
    m_waitingForMoreMyWave = false;
    emit loadingMyWaveChanged();
    emit loadingMoreMyWaveChanged();

    emit statusChanged(wasLoadingMore ? "Моя волна вернула пустую следующую партию"
                                      : "Моя волна вернула пустую партию");
    return;
  }

  if (!trimmedBatchId.isEmpty()) {
    for (const Track &track : tracks) {
      if (track.id.isEmpty()) continue;
      m_myWaveTrackBatches.insert(track.id, trimmedBatchId);
    }
  }

  const bool isMoreBatch = m_loadingMoreMyWave;
  m_loadingMyWave = false;
  m_loadingMoreMyWave = false;
  emit loadingMyWaveChanged();
  emit loadingMoreMyWaveChanged();

  if (!isMoreBatch) {
    m_myWaveModel->setTracks(tracks);

    QueueService *queue = m_playbackController->queueService();
    queue->clear();
    queue->addTracks(tracks);
    queue->setSource("Моя волна", "myWave");

    m_myWaveQueueActive = true;
    m_waitingForMoreMyWave = false;
    emit statusChanged(QStringLiteral("Моя волна: %1 треков").arg(m_myWaveModel->count()));
    return;
  }

  const int oldCount = m_myWaveModel->count();
  m_myWaveModel->appendTracks(tracks);
  appendMyWaveTracksToQueue(tracks);
  const int appended = m_myWaveModel->count() - oldCount;
  emit statusChanged(QStringLiteral("Моя волна: %1 треков").arg(m_myWaveModel->count()));

  if (!m_waitingForMoreMyWave) {
    return;
  }

  m_waitingForMoreMyWave = false;

  QueueService *queue = m_playbackController->queueService();
  queue->setSource("Моя волна", "myWave");

  if (!queue->hasNext()) {
    return;
  }

  queue->next();
  const Track nextTrack = queue->currentTrack();

  if (nextTrack.id.isEmpty()) {
    return;
  }

  m_currentMyWaveTrackId = nextTrack.id;
  m_myWaveTrackStarted = false;
  m_playbackController->playTrack(nextTrack);
}

void PersonalController::selectMyWaveTrack(int index) {
  const Track track = m_myWaveModel->trackAt(index);

  if (track.id.isEmpty()) {
    emit statusChanged("Некорректный трек моей волны");
    return;
  }

  if (m_myWaveQueueActive && m_myWaveTrackStarted) {
    stopCurrentMyWaveTrack("skip");
  }

  startMyWaveQueue(index);

  if (!m_myWaveQueueActive) {
    return;
  }

  emit myWaveTrackSelected(track);
  emit statusChanged(QStringLiteral("Выбран трек: %1").arg(track.title));
  m_playbackController->playTrack(track);
}

void PersonalController::startMyWaveQueue(int index) {
  QueueService *queue = m_playbackController->queueService();

  const QList<Track> tracks = m_myWaveModel->tracks();

  if (tracks.isEmpty()) {
    emit statusChanged("Моя волна пока пуста");
    return;
  }

  if (index < 0 || index >= tracks.size()) {
    emit statusChanged("Некорректный индекс трека My Wave");
    return;
  }

  queue->clear();
  queue->addTracks(tracks);
  queue->setSource("Моя волна", "myWave");

  if (!queue->setCurrentIndex(index)) {
    emit statusChanged("Не удалось выбрать трек My Wave");
    return;
  }

  m_myWaveQueueActive = true;
  m_waitingForMoreMyWave = false;
  m_currentMyWaveTrackId = queue->currentTrack().id;
  m_myWaveTrackStarted = false;
}

void PersonalController::appendMyWaveTracksToQueue(const QList<Track> &tracks) {
  QueueService *queue = m_playbackController->queueService();
  const QList<Track> queuedTracks = queue->tracks();

  // TODO(#503): на больших очередях линейный поиск дубликатов становится
  // узким местом — перевести на QSet idшников.
  for (const Track &track : tracks) {
    if (track.id.isEmpty()) continue;
    bool exists = false;

    for (const Track &queuedTrack : queuedTracks) {
      if (queuedTrack.id == track.id) {
        exists = true;
        break;
      }
    }

    if (exists) continue;
    queue->addTrack(track);
  }

  queue->setSource("Моя волна", "myWave");
}

void PersonalController::handleMyWavePlaybackFinished() {
  if (!m_myWaveQueueActive) {
    return;
  }

  QueueService *queue = m_playbackController->queueService();

  if (queue->hasNext()) {
    queue->next();
    const Track nextTrack = queue->currentTrack();

    if (nextTrack.id.isEmpty()) {
      return;
    }

    m_currentMyWaveTrackId = nextTrack.id;
    m_myWaveTrackStarted = false;
    m_playbackController->playTrack(nextTrack);
    return;
  }

  if (m_loadingMoreMyWave) {
    return;
  }

  const Track lastTrack = m_myWaveModel->lastTrack();

  if (lastTrack.id.isEmpty()) {
    m_myWaveQueueActive = false;
    return;
  }

  m_waitingForMoreMyWave = true;
  loadMoreMyWave();
}

QString PersonalController::batchIdForTrack(const QString &trackId) const {
  return m_myWaveTrackBatches.value(trackId);
}

void PersonalController::stopCurrentMyWaveTrack(const QString &event) {
  if (m_currentMyWaveTrackId.isEmpty()) {
    return;
  }

  if (!m_myWaveTrackStarted) {
    m_currentMyWaveTrackId.clear();
    return;
  }

  const qint64 playedSeconds = m_playerService->position() / 1000;
  sendMyWaveFeedback(event, m_currentMyWaveTrackId, playedSeconds);
  m_currentMyWaveTrackId.clear();
  m_myWaveTrackStarted = false;
}

void PersonalController::sendMyWaveFeedback(const QString &event, const QString &trackId,
                                            qint64 totalPlayedSeconds) {
  const QString batchId = batchIdForTrack(trackId);

  if (batchId.isEmpty()) {
    return;
  }

  m_yandexPersonal->sendMyWaveFeedback(event, trackId, batchId, totalPlayedSeconds);
}