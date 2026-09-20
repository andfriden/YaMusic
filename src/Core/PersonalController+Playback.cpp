#include "../Playback/PlaybackController.h"
#include "../Player/PlayerService.h"
#include "../Queue/QueueService.h"
#include "PersonalController.h"

void PersonalController::connectPlayback() {
  /*
   * PlaybackController является единственным владельцем
   * перехода по очереди.
   *
   * Здесь PersonalController только отслеживает начало
   * воспроизведения и отправляет feedback My Wave.
   */
  connect(m_playerService, &PlayerService::playbackStarted, this, [this]() {
    if (!m_myWaveQueueActive)
      return;

    if (m_currentMyWaveTrackId.isEmpty())
      return;

    if (m_myWaveTrackStarted)
      return;

    const QString batchId =
        batchIdForTrack(m_currentMyWaveTrackId);

    if (batchId.isEmpty())
      return;

    m_myWaveTrackStarted = true;

    sendMyWaveFeedback(
        "trackStarted",
        m_currentMyWaveTrackId);
  });

  /*
   * ВАЖНО:
   *
   * Не вызываем здесь handleMyWavePlaybackFinished().
   * Не вызываем queue->next().
   * Не вызываем playTrack().
   *
   * PlaybackController уже обработал playbackFinished:
   *
   *   текущий трек
   *        ↓
   *   queue.next()
   *        ↓
   *   следующий трек
   *
   * Если очередь закончилась, PlaybackController
   * самостоятельно отправит playlistExhausted.
   */
  connect(m_playbackController,
          &PlaybackController::currentTrackChanged,
          this,
          [this]() {
            if (!m_myWaveQueueActive)
              return;

            QueueService *queue =
                m_playbackController->queueService();

            if (queue == nullptr)
              return;

            const Track currentTrack =
                queue->currentTrack();

            if (currentTrack.id.isEmpty())
              return;

            if (m_currentMyWaveTrackId == currentTrack.id)
              return;

            /*
             * PlaybackController уже переключил очередь.
             * Просто синхронизируем состояние My Wave.
             */
            m_currentMyWaveTrackId = currentTrack.id;
            m_myWaveTrackStarted = false;
          });

  connect(m_playerService,
          &PlayerService::playbackFinished,
          this,
          [this]() {
            if (!m_myWaveQueueActive)
              return;

            if (m_currentMyWaveTrackId.isEmpty())
              return;

            const QString finishedTrackId =
                m_currentMyWaveTrackId;

            const bool wasStarted =
                m_myWaveTrackStarted;

            const qint64 playedSeconds =
                m_playerService->position() / 1000;

            if (wasStarted) {
              sendMyWaveFeedback(
                  "trackFinished",
                  finishedTrackId,
                  playedSeconds);
            }

            /*
             * Не очищаем здесь currentMyWaveTrackId.
             *
             * PlaybackController может уже переключить QueueService
             * на следующий трек. currentTrackChanged синхронизирует
             * m_currentMyWaveTrackId с новым треком.
             *
             * Если это последний трек, PlaybackController отправит
             * playlistExhausted, после чего My Wave загрузит следующий
             * batch.
             */
            m_myWaveTrackStarted = false;
          });
}