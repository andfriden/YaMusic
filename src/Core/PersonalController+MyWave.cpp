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
  m_replaceMyWaveQueue = false;
  m_loadingMyWave = true;
  emit loadingMyWaveChanged();
  m_myWaveTrackBatches.clear();
  m_myWaveModel->clear();
  // Новая сессия волны: недавно сыгранное больше не ограничение.
  m_recentMyWaveTrackIds.clear();
  m_recentMyWaveOrder.clear();

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

  // Пропуск или дизлайк пересчитали последовательность: свежая партия
  // заменяет очередь целиком, чтобы слышно было уже новое.
  if (m_replaceMyWaveQueue) {
    m_replaceMyWaveQueue = false;
    m_waitingForMoreMyWave = false;
    m_myWaveQueueActive = true;

    QueueService *queue = m_playbackController->queueService();
    m_myWaveModel->setTracks(tracks);
    queue->clear();
    queue->addTracks(tracks);
    queue->setSource("Моя волна", "myWave");

    emit statusChanged(QStringLiteral("Моя волна: %1 треков").arg(m_myWaveModel->count()));

    if (!queue->setCurrentIndex(0)) {
      emit statusChanged("Не удалось заменить очередь моей волны");
      return;
    }

    const Track firstTrack = queue->currentTrack();

    if (firstTrack.id.isEmpty()) {
      return;
    }

    m_currentMyWaveTrackId = firstTrack.id;
    m_myWaveTrackStarted = false;
    markMyWaveTrackPlayed(firstTrack.id);
    m_playbackController->playTrack(firstTrack);
    return;
  }

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

  // Пришедшее сыгранное не показываем снова, но партию без ничего
  // нового принимаем как есть — иначе волна оборвалась бы.
  QList<Track> batch;

  for (const Track &track : tracks) {
    if (track.id.isEmpty()) continue;
    if (m_recentMyWaveTrackIds.contains(track.id)) continue;
    batch.append(track);
  }

  if (batch.isEmpty()) {
    batch = tracks;
  }

  const int oldCount = m_myWaveModel->count();
  m_myWaveModel->appendTracks(batch);
  appendMyWaveTracksToQueue(batch);
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
  markMyWaveTrackPlayed(nextTrack.id);
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

  markMyWaveTrackPlayed(track.id);
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
    if (queue->count() >= kMaxQueueTracks) break;
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
    markMyWaveTrackPlayed(nextTrack.id);
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

void PersonalController::reportMyWaveDislike(const QString &trackId, bool disliked) {
  if (!m_myWaveQueueActive) {
    return;
  }

  const QString id = trackId.trimmed();

  if (id.isEmpty() || id != m_currentMyWaveTrackId) {
    return;
  }

  sendMyWaveFeedback(disliked ? "dislike" : "undislike", id, 0);

  // Дизлайк пересчитывает последовательность: оставшуюся очередь
  // заменяем свежей партией, как и на пропуске.
  if (disliked && !m_loadingMyWave && !m_loadingMoreMyWave) {
    replaceMyWaveQueue(id);
  }
}

void PersonalController::deactivateMyWave() {
  m_myWaveQueueActive = false;
  m_waitingForMoreMyWave = false;
  m_replaceMyWaveQueue = false;
  m_currentMyWaveTrackId.clear();
  m_myWaveTrackStarted = false;
}

bool PersonalController::skipMyWaveTrack() {
  if (!m_myWaveQueueActive) {
    return false;
  }

  // Запрос уже в пути: очередь сейчас пересобирается, лишний фидбек
  // исказил бы ответ ротора.
  if (m_loadingMyWave || m_loadingMoreMyWave) {
    return false;
  }

  // Только фидбек: очередь остаётся прежней, следующий трек берётся
  // из уже загруженной партии (обычный переход PlaybackController).
  stopCurrentMyWaveTrack("skip");
  return true;
}

void PersonalController::replaceMyWaveQueue(const QString &fromTrackId) {
  const QString id = fromTrackId.trimmed();

  if (id.isEmpty()) {
    return;
  }

  m_replaceMyWaveQueue = true;
  m_waitingForMoreMyWave = true;
  emit statusChanged("Моя волна: обновление очереди...");
  m_yandexPersonal->loadMoreMyWave(id);
}

void PersonalController::markMyWaveTrackPlayed(const QString &trackId) {
  const QString id = trackId.trimmed();

  if (id.isEmpty()) {
    return;
  }

  if (m_recentMyWaveTrackIds.contains(id)) {
    m_recentMyWaveOrder.removeAll(id);
  }

  m_recentMyWaveTrackIds.insert(id);
  m_recentMyWaveOrder.append(id);

  while (m_recentMyWaveOrder.size() > kRecentTrackLimit) {
    m_recentMyWaveTrackIds.remove(m_recentMyWaveOrder.takeFirst());
  }
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