#pragma once

#include "SystemMediaControls.h"
#include <memory>

// Фабрика платформенных реализаций SystemMediaControls.
// Платформа определяется на этапе компиляции через Q_OS_* макросы Qt.
class MediaControlsFactory {
public:
  static std::unique_ptr<SystemMediaControls> create(QObject *parent = nullptr);
};