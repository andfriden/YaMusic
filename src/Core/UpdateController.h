#pragma once

#include "../Update/ReleaseInfo.h"
#include <QObject>
#include <QString>

namespace Update {
class UpdateChecker;
class UpdateInstaller;
} // namespace Update

// Обновление приложения для интерфейса: проверка релизов GitHub, тихая
// проверка при запуске, скачивание и установка на macOS.
//
// Проверка при запуске ничего не показывает: обновление только становится
// доступным (updateAvailable), и окно само решает, показать ли предложение.
// Отклонённую версию помним в настройках и больше не предлагаем.
class UpdateController : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool checking READ checking NOTIFY stateChanged)
  Q_PROPERTY(bool updateAvailable READ updateAvailable NOTIFY stateChanged)
  Q_PROPERTY(QString currentVersion READ currentVersion NOTIFY stateChanged)
  Q_PROPERTY(QString latestVersion READ latestVersion NOTIFY stateChanged)
  Q_PROPERTY(QString releaseNotes READ releaseNotes NOTIFY stateChanged)
  Q_PROPERTY(QString releaseUrl READ releaseUrl NOTIFY stateChanged)

  // Установка доступна не всегда: из каталога сборки приложение себя не
  // подменит, и тогда предлагается скачать релиз вручную.
  Q_PROPERTY(bool canInstall READ canInstall NOTIFY stateChanged)
  Q_PROPERTY(QString installHint READ installHint NOTIFY stateChanged)
  Q_PROPERTY(bool installing READ installing NOTIFY stateChanged)
  Q_PROPERTY(double downloadProgress READ downloadProgress NOTIFY downloadProgressChanged)

  // Замена подготовлена: обновление встанет на место после выхода, и окно
  // предлагает перезапуститься.
  Q_PROPERTY(bool readyToRestart READ readyToRestart NOTIFY stateChanged)

  Q_PROPERTY(QString status READ status NOTIFY statusChanged)

  Q_PROPERTY(bool autoCheckEnabled READ autoCheckEnabled WRITE setAutoCheckEnabled NOTIFY
                 autoCheckEnabledChanged)

public:
  explicit UpdateController(QObject *parent = nullptr);

  bool checking() const;
  bool updateAvailable() const;
  QString currentVersion() const;
  QString latestVersion() const;
  QString releaseNotes() const;
  QString releaseUrl() const;
  bool canInstall() const;
  QString installHint() const;
  bool installing() const;
  bool readyToRestart() const;
  double downloadProgress() const;
  QString status() const;
  bool autoCheckEnabled() const;

  // Проверка при запуске: тихая, через несколько секунд после старта.
  void checkOnStartup();

  Q_INVOKABLE void checkForUpdates();
  Q_INVOKABLE void downloadAndInstall();
  Q_INVOKABLE void openReleasePage();
  Q_INVOKABLE void skipVersion();
  Q_INVOKABLE void dismiss();
  Q_INVOKABLE void restartApplication();

  void setAutoCheckEnabled(bool enabled);

signals:
  void stateChanged();
  void downloadProgressChanged();
  void statusChanged();
  void autoCheckEnabledChanged();

private:
  void handleUpdateAvailable(const Update::ReleaseInfo &release);
  void handleUpToDate();
  void handleCheckFailed(const QString &message);
  void setStatus(const QString &status);
  void persistSkippedVersion(const QString &version);

  Update::UpdateChecker *m_checker = nullptr;
  Update::UpdateInstaller *m_installer = nullptr;

  Update::ReleaseInfo m_release;
  QString m_skippedVersion;
  QString m_status;
  bool m_updateAvailable = false;
  bool m_manualCheck = false;
  bool m_autoCheckEnabled = true;
  bool m_readyToRestart = false;
};
