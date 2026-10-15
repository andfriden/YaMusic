#include "UpdateController.h"
#include "../Update/UpdateChecker.h"
#include "../Update/UpdateInstaller.h"
#include <QCoreApplication>
#include <QDesktopServices>
#include <QSettings>
#include <QTimer>
#include <QUrl>

namespace {

// Отсрочка тихой проверки: она не должна мешать запуску и первому экрану.
constexpr int kStartupCheckDelayMs = 5000;

constexpr auto kAutoCheckKey = "updates/autoCheck";
constexpr auto kSkippedVersionKey = "updates/skippedVersion";

} // namespace

UpdateController::UpdateController(QObject *parent)
    : QObject(parent), m_checker(new Update::UpdateChecker(this)),
      m_installer(new Update::UpdateInstaller(this)) {
  QSettings settings;
  m_autoCheckEnabled = settings.value(kAutoCheckKey, true).toBool();
  m_skippedVersion = settings.value(kSkippedVersionKey).toString().trimmed();

  connect(m_checker, &Update::UpdateChecker::checkingChanged, this,
          &UpdateController::stateChanged);
  connect(m_checker, &Update::UpdateChecker::updateAvailable, this,
          &UpdateController::handleUpdateAvailable);
  connect(m_checker, &Update::UpdateChecker::upToDate, this, [this]() { handleUpToDate(); });
  connect(m_checker, &Update::UpdateChecker::checkFailed, this,
          &UpdateController::handleCheckFailed);

  connect(m_installer, &Update::UpdateInstaller::stageChanged, this,
          &UpdateController::stateChanged);
  connect(m_installer, &Update::UpdateInstaller::statusChanged, this,
          [this]() { setStatus(m_installer->status()); });
  connect(m_installer, &Update::UpdateInstaller::progressChanged, this, [this]() {
    emit downloadProgressChanged();
    emit stateChanged();
  });
  connect(m_installer, &Update::UpdateInstaller::failed, this,
          &UpdateController::handleCheckFailed);
  connect(m_installer, &Update::UpdateInstaller::finished, this, [this](const QString &version) {
    m_readyToRestart = true;
    setStatus(QStringLiteral("Версия %1 установится после выхода из приложения").arg(version));
    emit stateChanged();
  });
}

bool UpdateController::checking() const {
  return m_checker->isChecking();
}

bool UpdateController::updateAvailable() const {
  return m_updateAvailable;
}

QString UpdateController::currentVersion() const {
  return m_checker->currentVersion();
}

QString UpdateController::latestVersion() const {
  return m_release.version;
}

QString UpdateController::releaseNotes() const {
  return m_release.notes;
}

QString UpdateController::releaseUrl() const {
  return m_release.pageUrl.toString();
}

bool UpdateController::canInstall() const {
  return m_installer->isSupported();
}

QString UpdateController::installHint() const {
  return m_installer->unsupportedReason();
}

bool UpdateController::installing() const {
  const Update::UpdateInstaller::Stage stage = m_installer->stage();

  return stage == Update::UpdateInstaller::Downloading ||
         stage == Update::UpdateInstaller::Unpacking;
}

bool UpdateController::readyToRestart() const {
  return m_readyToRestart;
}

double UpdateController::downloadProgress() const {
  return m_installer->progress();
}

QString UpdateController::status() const {
  return m_status;
}

bool UpdateController::autoCheckEnabled() const {
  return m_autoCheckEnabled;
}

void UpdateController::setAutoCheckEnabled(bool enabled) {
  if (m_autoCheckEnabled == enabled) return;

  m_autoCheckEnabled = enabled;
  QSettings settings;
  settings.setValue(kAutoCheckKey, enabled);
  emit autoCheckEnabledChanged();
}

void UpdateController::checkOnStartup() {
  if (!m_autoCheckEnabled) return;

  QTimer::singleShot(kStartupCheckDelayMs, this, [this]() {
    if (m_checker->isChecking()) return;
    m_manualCheck = false;
    m_checker->check();
  });
}

void UpdateController::checkForUpdates() {
  if (m_checker->isChecking()) return;

  m_manualCheck = true;
  setStatus(QStringLiteral("Проверка обновлений..."));
  m_checker->check();
}

void UpdateController::downloadAndInstall() {
  if (!m_release.isValid()) {
    setStatus(QStringLiteral("Сначала нужно проверить обновления"));
    return;
  }

  if (!canInstall()) {
    setStatus(installHint());
    openReleasePage();
    return;
  }

  const Update::ReleaseAsset asset = m_checker->assetForThisSystem();

  if (asset.name.isEmpty()) {
    setStatus(
        QStringLiteral("В релизе нет сборки для этого компьютера — открываю страницу релиза"));
    openReleasePage();
    return;
  }

  m_readyToRestart = false;
  emit stateChanged();
  m_installer->start(asset, m_release.version);
}

void UpdateController::openReleasePage() {
  if (!m_release.pageUrl.isValid()) {
    setStatus(QStringLiteral("Страница релиза недоступна"));
    return;
  }

  QDesktopServices::openUrl(m_release.pageUrl);
}

void UpdateController::skipVersion() {
  if (!m_release.isValid()) return;

  persistSkippedVersion(m_release.version);
  m_updateAvailable = false;
  setStatus(QStringLiteral("Версия %1 больше не предлагается").arg(m_release.version));
  emit stateChanged();
}

void UpdateController::dismiss() {
  if (!m_updateAvailable) return;

  m_updateAvailable = false;
  setStatus({});
  emit stateChanged();
}

void UpdateController::restartApplication() {
  QCoreApplication::quit();
}

void UpdateController::handleUpdateAvailable(const Update::ReleaseInfo &release) {
  m_release = release;

  const bool skipped = !m_skippedVersion.isEmpty() && m_skippedVersion == release.version;
  m_updateAvailable = !skipped;

  if (skipped) {
    setStatus(QStringLiteral("Есть версия %1, но она пропущена").arg(release.version));
  } else {
    setStatus(QStringLiteral("Доступна версия %1").arg(release.version));
  }

  emit stateChanged();
}

void UpdateController::handleUpToDate() {
  m_release = {};
  m_updateAvailable = false;

  if (m_manualCheck) {
    setStatus(QStringLiteral("Установлена последняя версия (%1)").arg(m_checker->currentVersion()));
  } else {
    setStatus({});
  }

  emit stateChanged();
}

void UpdateController::handleCheckFailed(const QString &message) {
  // Тихая проверка не должна показывать ошибку: о неудаче узнаём молча,
  // следующая проверка пройдёт при запуске.
  if (m_manualCheck) {
    setStatus(QStringLiteral("Не удалось проверить обновления: %1").arg(message));
  } else {
    setStatus({});
  }

  emit stateChanged();
}

void UpdateController::setStatus(const QString &status) {
  if (m_status == status) return;

  m_status = status;
  emit statusChanged();
}

void UpdateController::persistSkippedVersion(const QString &version) {
  m_skippedVersion = version;

  QSettings settings;
  settings.setValue(kSkippedVersionKey, version);
}
