#include "../Player/PlayerService.h"
#include "PersonalController.h"

void PersonalController::connectPlayback() {
  if (m_playerService == nullptr) return;

  // Track started
  connect(m_playerService, &PlayerService::playbackStarted, this, [this]() {
    // Feedback нужен только для активной My Wave
    if (!m_myWaveQueueActive) return;
    if (m_currentMyWaveTrackId.isEmpty()) return;
    if (m_myWaveTrackStarted) return;
    const QString batchId = batchIdForTrack(m_currentMyWaveTrackId);
    if (batchId.isEmpty()) return;
    m_myWaveTrackStarted = true;
    sendMyWaveFeedback("trackStarted", m_currentMyWaveTrackId);
  });

  // Track finished
  connect(m_playerService, &PlayerService::playbackFinished, this, [this]() {
    // Сохраняем состояние трека, который только что закончился
    const QString finishedTrackId = m_currentMyWaveTrackId;
    const bool isMyWavePlayback = m_myWaveQueueActive && !finishedTrackId.isEmpty();
    const bool wasStarted = m_myWaveTrackStarted;
    const qint64 playedSeconds =
        m_playerService != nullptr ? m_playerService->position() / 1000 : 0;

    if (isMyWavePlayback && wasStarted) {
      sendMyWaveFeedback("trackFinished", finishedTrackId, playedSeconds);
    }

    // Если это My Wave, передаём управление очереди обработчику
    if (isMyWavePlayback) {
      handleMyWavePlaybackFinished();

      // handleMyWavePlaybackFinished() мог уже установить следующий трек
      if (m_currentMyWaveTrackId == finishedTrackId) {
        m_currentMyWaveTrackId.clear();
        m_myWaveTrackStarted = false;
      }
      return;
    }
  });
}