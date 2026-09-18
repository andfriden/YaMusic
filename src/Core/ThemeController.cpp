#include "ThemeController.h"

ThemeController::ThemeController(QObject *parent) : QObject(parent) {
  QSettings settings;
  m_darkTheme = settings.value("theme/dark", false).toBool();
}

bool ThemeController::darkTheme() const {
  return m_darkTheme;
}

void ThemeController::setDarkTheme(bool dark) {
  if (m_darkTheme == dark) return;
  m_darkTheme = dark;
  QSettings settings;
  settings.setValue("theme/dark", dark);
  emit darkThemeChanged();
}