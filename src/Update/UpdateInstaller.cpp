#include "UpdateInstaller.h"
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTextStream>

namespace Update {

namespace {

// Временный каталог обновления живёт до перезапуска приложения.
QString updateTempPath() {
  const QString base = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

  if (base.isEmpty()) return QDir::tempPath() + QStringLiteral("/yamusic-update");

  return base + QStringLiteral("/yamusic-update");
}

// Путь приложения, в кавычках-одиночках для sh: одинарные кавычки внутри
// пути экранируются обычным для шелла способом.
QString shellQuote(const QString &value) {
  QString escaped = value;
  escaped.replace(QLatin1String("'"), QLatin1String("'\\''"));
  return QLatin1Char('\'') + escaped + QLatin1Char('\'');
}

} // namespace

UpdateInstaller::UpdateInstaller(QObject *parent)
    : QObject(parent), m_network(new QNetworkAccessManager(this)) {}

UpdateInstaller::Stage UpdateInstaller::stage() const {
  return m_stage;
}

double UpdateInstaller::progress() const {
  return m_progress;
}

QString UpdateInstaller::status() const {
  return m_status;
}

QString UpdateInstaller::bundlePath() const {
  // applicationDirPath() у бандла — это <App>.app/Contents/MacOS.
  const QString dir = QCoreApplication::applicationDirPath();
  return QDir::cleanPath(dir + QStringLiteral("/../.."));
}

bool UpdateInstaller::isSupported() const {
#if defined(Q_OS_MACOS) || defined(Q_OS_MAC)
  const QString bundle = bundlePath();

  if (!bundle.endsWith(QStringLiteral(".app"))) return false;

  // Сборка из каталога разработчика не обновляется: подменять её нечем.
  if (bundle.contains(QStringLiteral("/build")) ||
      bundle.contains(QStringLiteral("/cmake-build")) || bundle.startsWith(QDir::tempPath())) {
    return false;
  }

  const QFileInfo info(bundle);
  const QFileInfo parent(info.absolutePath());

  return info.isDir() && parent.isWritable();
#else
  return false;
#endif
}

QString UpdateInstaller::unsupportedReason() const {
#if defined(Q_OS_MACOS) || defined(Q_OS_MAC)
  const QString bundle = bundlePath();

  if (!bundle.endsWith(QStringLiteral(".app"))) {
    return QStringLiteral("Приложение запущено не из установленного бандла .app");
  }

  if (bundle.contains(QStringLiteral("/build")) ||
      bundle.contains(QStringLiteral("/cmake-build")) || bundle.startsWith(QDir::tempPath())) {
    return QStringLiteral("Приложение запущено из каталога сборки — обновление вручную");
  }

  if (!QFileInfo(bundle).isDir()) {
    return QStringLiteral("Бандл приложения не найден");
  }

  if (!QFileInfo(QFileInfo(bundle).absolutePath()).isWritable()) {
    return QStringLiteral("Нет прав на запись рядом с приложением");
  }

  return {};
#else
  return QStringLiteral("Автоматическая установка обновлений поддерживается только на macOS");
#endif
}

void UpdateInstaller::start(const ReleaseAsset &asset, const QString &version) {
  if (m_stage == Downloading || m_stage == Unpacking) return;

  m_asset = asset;
  m_version = version.trimmed().isEmpty() ? QStringLiteral("new") : version.trimmed();
  m_cancelled = false;
  m_progress = 0.0;

  if (!asset.url.isValid() || asset.name.isEmpty()) {
    fail(QStringLiteral("В релизе нет файла для этой системы"));
    return;
  }

  if (!isSupported()) {
    fail(unsupportedReason());
    return;
  }

  QString tempPath = updateTempPath();

  if (!QDir().mkpath(tempPath)) {
    fail(QStringLiteral("Не удалось создать временный каталог обновления"));
    return;
  }

  m_downloadPath = tempPath + QStringLiteral("/") + asset.name;

  QFile::remove(m_downloadPath);
  startDownload();
}

void UpdateInstaller::cancel() {
  m_cancelled = true;

  if (m_reply != nullptr && m_reply->isRunning()) m_reply->abort();

  setStage(Idle);
  setStatus({});
}

void UpdateInstaller::startDownload() {
  setStage(Downloading);
  m_progress = 0.0;
  emit progressChanged();
  setStatus(QStringLiteral("Скачивание обновления..."));

  QNetworkRequest request(m_asset.url);
  request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("YaMusic/%1").arg(m_version));
  request.setRawHeader("Accept", "application/octet-stream");
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                       QNetworkRequest::NoLessSafeRedirectPolicy);

  QNetworkReply *reply = m_network->get(request);
  m_reply = reply;

  auto file = std::make_shared<QFile>(m_downloadPath);

  if (!file->open(QIODevice::WriteOnly)) {
    reply->abort();
    reply->deleteLater();
    fail(QStringLiteral("Не удалось открыть файл для скачивания"));
    return;
  }

  connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
    if (file->isOpen()) file->write(reply->readAll());
  });

  connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 received, qint64 total) {
    if (total <= 0) {
      m_progress = 0.0;
      emit progressChanged();
      return;
    }

    m_progress = static_cast<double>(received) / static_cast<double>(total);
    emit progressChanged();
  });

  connect(reply, &QNetworkReply::finished, this, [this, reply, file]() {
    reply->deleteLater();
    file->close();

    if (m_cancelled) return;

    if (reply->error() != QNetworkReply::NoError) {
      QFile::remove(m_downloadPath);
      fail(reply->errorString());
      return;
    }

    if (file->size() <= 0) {
      QFile::remove(m_downloadPath);
      fail(QStringLiteral("Скачанный файл пуст"));
      return;
    }

    m_progress = 1.0;
    emit progressChanged();
    prepareReplacement();
  });
}

void UpdateInstaller::prepareReplacement() {
#if defined(Q_OS_MACOS) || defined(Q_OS_MAC)
  setStage(Unpacking);
  setStatus(QStringLiteral("Подготовка новой версии..."));

  const QString tempPath = QFileInfo(m_downloadPath).absolutePath();
  m_mountPath = tempPath + QStringLiteral("/mnt");
  QDir().mkpath(m_mountPath);

  auto runTool = [](const QString &program, const QStringList &arguments, QString *error) {
    QProcess process;
    process.start(program, arguments);

    if (!process.waitForStarted(10000)) {
      if (error != nullptr) *error = QStringLiteral("Не удалось запустить %1").arg(program);
      return false;
    }

    if (!process.waitForFinished(120000)) {
      process.kill();
      if (error != nullptr) *error = QStringLiteral("%1 не ответил вовремя").arg(program);
      return false;
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
      if (error != nullptr) {
        QString message = QString::fromUtf8(process.readAllStandardError()).trimmed();
        if (message.isEmpty()) message = QStringLiteral("%1 завершился с ошибкой").arg(program);
        *error = message;
      }

      return false;
    }

    return true;
  };

  QString error;

  if (!runTool(QStringLiteral("/usr/bin/hdiutil"),
               {QStringLiteral("attach"), QStringLiteral("-nobrowse"), QStringLiteral("-noverify"),
                QStringLiteral("-mountpoint"), m_mountPath, m_downloadPath},
               &error)) {
    fail(error);
    return;
  }

  QString mountedApp;

  QDirIterator iterator(m_mountPath, {QStringLiteral("*.app")}, QDir::Dirs | QDir::NoDotAndDotDot);

  while (iterator.hasNext()) {
    mountedApp = iterator.next();
    break;
  }

  if (mountedApp.isEmpty()) {
    runTool(QStringLiteral("/usr/bin/hdiutil"),
            {QStringLiteral("detach"), m_mountPath, QStringLiteral("-quiet")}, nullptr);
    fail(QStringLiteral("В образе не найден бандл приложения"));
    return;
  }

  m_stagedAppPath = tempPath + QStringLiteral("/YaMusic-") + m_version + QStringLiteral(".app");

  QDir().remove(m_stagedAppPath);

  if (!runTool(QStringLiteral("/usr/bin/ditto"), {mountedApp, m_stagedAppPath}, &error)) {
    runTool(QStringLiteral("/usr/bin/hdiutil"),
            {QStringLiteral("detach"), m_mountPath, QStringLiteral("-quiet")}, nullptr);
    fail(error);
    return;
  }

  runTool(QStringLiteral("/usr/bin/hdiutil"),
          {QStringLiteral("detach"), m_mountPath, QStringLiteral("-quiet")}, nullptr);

  const QString bundle = bundlePath();
  m_scriptPath = tempPath + QStringLiteral("/install-update.sh");

  QFile script(m_scriptPath);

  if (!script.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    fail(QStringLiteral("Не удалось записать скрипт обновления"));
    return;
  }

  const qint64 pid = QCoreApplication::applicationPid();
  const QString backup = bundle + QStringLiteral(".old-") + m_version;

  QTextStream stream(&script);
  stream << "#!/bin/sh\n"
         << "# Ждёт выхода приложения, подменяет бандл и запускает новую версию.\n"
         << "pid=" << pid << "\n"
         << "while kill -0 \"$pid\" 2>/dev/null; do sleep 0.5; done\n"
         << "sleep 1\n"
         << "if [ -d " << shellQuote(bundle) << " ]; then mv " << shellQuote(bundle) << " "
         << shellQuote(backup) << " || exit 1; fi\n"
         << "if /usr/bin/ditto " << shellQuote(m_stagedAppPath) << " " << shellQuote(bundle)
         << "; then\n"
         << "  rm -rf " << shellQuote(backup) << "\n"
         << "else\n"
         << "  if [ -d " << shellQuote(backup) << " ]; then /usr/bin/ditto " << shellQuote(backup)
         << " " << shellQuote(bundle) << "; fi\n"
         << "  exit 1\n"
         << "fi\n"
         << "rm -rf " << shellQuote(tempPath) << "\n"
         << "/usr/bin/open " << shellQuote(bundle) << "\n";
  stream.flush();
  script.close();

  if (!QFile::setPermissions(m_scriptPath, QFileDevice::ReadOwner | QFileDevice::WriteOwner |
                                               QFileDevice::ExeOwner)) {
    fail(QStringLiteral("Не удалось сделать скрипт обновления исполняемым"));
    return;
  }

  if (!QProcess::startDetached(QStringLiteral("/bin/sh"), {m_scriptPath})) {
    fail(QStringLiteral("Не удалось запустить скрипт обновления"));
    return;
  }

  setStage(Ready);
  setStatus(QStringLiteral("Обновление до %1 установится после выхода").arg(m_version));
  emit finished(m_version);
#else
  fail(unsupportedReason());
#endif
}

void UpdateInstaller::setStage(Stage stage) {
  if (m_stage == stage) return;

  m_stage = stage;
  emit stageChanged();
}

void UpdateInstaller::setStatus(const QString &status) {
  if (m_status == status) return;

  m_status = status;
  emit statusChanged();
}

void UpdateInstaller::fail(const QString &message) {
  setStage(Failed);
  setStatus(message);
  emit failed(message);
}

} // namespace Update
