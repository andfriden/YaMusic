#include "PlaybackSettingsController.h"
#include "../Player/PlayerService.h"
#include <QSettings>

namespace {

constexpr auto kCrossfadeKey = "playback/crossfadeSeconds";

} // namespace

PlaybackSettingsController::PlaybackSettingsController(PlayerService *playerService, QObject *parent)
    : QObject(parent), m_playerService(playerService) {
  Q_ASSERT(m_playerService != nullptr);

  QSettings settings;
  m_crossfadeSeconds =
      qBound(0, settings.value(kCrossfadeKey, m_crossfadeSeconds).toInt(), kMaxCrossfadeSeconds);

  m_playerService->setCrossfadeMs(m_crossfadeSeconds * 1000);
}

int PlaybackSettingsController::crossfadeSeconds() const {
  return m_crossfadeSeconds;
}

void PlaybackSettingsController::setCrossfadeSeconds(int seconds) {
  const int bounded = qBound(0, seconds, kMaxCrossfadeSeconds);

  if (m_crossfadeSeconds == bounded) return;

  m_crossfadeSeconds = bounded;
  m_playerService->setCrossfadeMs(bounded * 1000);

  QSettings settings;
  settings.setValue(kCrossfadeKey, bounded);

  emit crossfadeSecondsChanged();
}
