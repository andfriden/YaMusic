#pragma once

#include <QtGlobal>

namespace Playback {

// За сколько до конца текущего трека готовить следующий на свободном
// голосе. Подготовка заранее открывает декодер, поэтому в конце трека
// не остаётся паузы на загрузку источника.
inline constexpr int kGaplessPrepareLeadMs = 15000;

inline bool isGaplessPrepareDue(qint64 positionMs, qint64 durationMs) {
  if (durationMs <= 0 || positionMs < 0) return false;

  return positionMs >= durationMs - kGaplessPrepareLeadMs;
}

} // namespace Playback
