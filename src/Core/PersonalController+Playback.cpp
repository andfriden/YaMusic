#include "../Player/PlayerService.h"
#include "PersonalController.h"

void PersonalController::connectPlayback() {
  connect(m_playerService, &PlayerService::playbackStarted, this, [this]() {
    if (!m_myWaveQueueActive) return;
    if (m_currentMyWaveTrackId.isEmpty()) return;
    if (m_myWaveTrackStarted) return;
    const QString batchId = batchIdForTrack(m_currentMyWaveTrackId);
    if (batchId.isEmpty()) return;
    m_myWaveTrackStarted = true;
    sendMyWaveFeedback("trackStarted", m_currentMyWaveTrackId);
  });

  connect(m_playerService, &PlayerService::playbackFinished, this, [this]() {
    const QString finishedTrackId = m_currentMyWaveTrackId;
    const bool isMyWavePlayback = m_myWaveQueueActive && !finishedTrackId.isEmpty();
    const bool wasStarted = m_myWaveTrackStarted;
    const qint64 playedSeconds = m_playerService->position() / 1000;

    if (isMyWavePlayback && wasStarted) {
      sendMyWaveFeedback("trackFinished", finishedTrackId, playedSeconds);
    }

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