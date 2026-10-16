#include "PreloadController.h"
#include "../Playback/Preloader.h"
#include "../Player/TrackCache.h"
#include <QSettings>

namespace {

constexpr qint64 kBytesInMb = 1024LL * 1024LL;

constexpr auto kEnabledKey = "playback/preload";
constexpr auto kAheadKey = "playback/preloadAhead";
constexpr auto kLimitKey = "playback/cacheLimitMb";

// Границы, чтобы настройка не превратилась в «держать всю библиотеку»
// или в ноль треков вперёд.
constexpr int kMaxAheadCount = 10;
constexpr int kMinLimitMb = 100;
constexpr int kMaxLimitMb = 20000;

} // namespace

PreloadController::PreloadController(TrackCache *cache, Playback::Preloader *preloader,
                                     QObject *parent)
    : QObject(parent), m_cache(cache), m_preloader(preloader) {
  Q_ASSERT(m_cache != nullptr);
  Q_ASSERT(m_preloader != nullptr);

  QSettings settings;
  m_enabled = settings.value(kEnabledKey, true).toBool();
  m_aheadCount = qBound(0, settings.value(kAheadKey, 3).toInt(), kMaxAheadCount);
  m_limitMb = qBound(kMinLimitMb, settings.value(kLimitKey, 500).toInt(), kMaxLimitMb);

  m_preloader->setEnabled(m_enabled);
  m_preloader->setAheadCount(m_aheadCount);
  applyLimit();

  connect(m_preloader, &Playback::Preloader::readyChanged, this, &PreloadController::cacheChanged);
  connect(m_preloader, &Playback::Preloader::cached, this, [this](const QString &) {
    emit cacheChanged();
  });
}

bool PreloadController::enabled() const {
  return m_enabled;
}

int PreloadController::aheadCount() const {
  return m_aheadCount;
}

int PreloadController::limitMb() const {
  return m_limitMb;
}

double PreloadController::usedMb() const {
  return static_cast<double>(m_cache->totalBytes()) / static_cast<double>(kBytesInMb);
}

int PreloadController::windowCount() const {
  return m_preloader->windowCount();
}

int PreloadController::readyCount() const {
  return m_preloader->readyCount();
}

void PreloadController::setEnabled(bool enabled) {
  if (m_enabled == enabled) return;

  m_enabled = enabled;
  m_preloader->setEnabled(enabled);

  QSettings settings;
  settings.setValue(kEnabledKey, enabled);

  emit enabledChanged();
  emit cacheChanged();
}

void PreloadController::setAheadCount(int count) {
  const int bounded = qBound(0, count, kMaxAheadCount);

  if (m_aheadCount == bounded) return;

  m_aheadCount = bounded;
  m_preloader->setAheadCount(bounded);

  QSettings settings;
  settings.setValue(kAheadKey, bounded);

  emit aheadCountChanged();
  emit cacheChanged();
}

void PreloadController::setLimitMb(int megabytes) {
  const int bounded = qBound(kMinLimitMb, megabytes, kMaxLimitMb);

  if (m_limitMb == bounded) return;

  m_limitMb = bounded;

  QSettings settings;
  settings.setValue(kLimitKey, bounded);

  applyLimit();

  emit limitMbChanged();
  emit cacheChanged();
}

void PreloadController::clearCache() {
  m_cache->clear();
  emit cacheChanged();
}

void PreloadController::applyLimit() {
  m_cache->setLimitBytes(static_cast<qint64>(m_limitMb) * kBytesInMb);
  m_cache->evictIfNeeded();
}
