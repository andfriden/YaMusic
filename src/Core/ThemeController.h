#pragma once

#include <QObject>
#include <QSettings>

// Тёмная тема — сохраняется в QSettings
class ThemeController : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool darkTheme READ darkTheme WRITE setDarkTheme NOTIFY darkThemeChanged)

public:
  explicit ThemeController(QObject *parent = nullptr);

  bool darkTheme() const;

  Q_INVOKABLE void setDarkTheme(bool dark);

signals:
  void darkThemeChanged();

private:
  bool m_darkTheme = false;
};