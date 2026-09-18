#include "AccentController.h"
#include "../Playback/PlaybackController.h"

AccentController::AccentController(PlayerAccentService *service,
                                   PlaybackController *playbackController, QObject *parent)
    : QObject(parent), m_service(service), m_playbackController(playbackController) {
  Q_ASSERT(m_service != nullptr);
  Q_ASSERT(m_playbackController != nullptr);

  connect(m_service, &PlayerAccentService::accentColorChanged, this,
          &AccentController::playerAccentChanged);

  connect(m_playbackController, &PlaybackController::currentTrackChanged, this, [this]() {
    const Track track = m_playbackController->currentTrack();
    if (track.id.isEmpty()) return;
    m_service->updateForCover(track.coverUri);
  });
}

QColor AccentController::playerAccent() const {
  return m_service->accentColor();
}