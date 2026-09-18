#include "MediaControlsFactory.h"

#include "MediaControlsMPRIS.h"
#include "MediaControlsMacOS.h"
#include "MediaControlsSMTC.h"

std::unique_ptr<SystemMediaControls> MediaControlsFactory::create(QObject *parent) {
#if defined(Q_OS_MACOS)
  return std::make_unique<MediaControlsMacOS>(parent);
#elif defined(Q_OS_LINUX)
  return std::make_unique<MediaControlsMPRIS>(parent);
#elif defined(Q_OS_WIN)
  return std::make_unique<MediaControlsSMTC>(parent);
#else
  (void)parent;
  return nullptr;
#endif
}