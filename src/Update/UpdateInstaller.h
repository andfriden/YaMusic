#pragma once

#include "ReleaseInfo.h"
#include <QObject>
#include <QPointer>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

namespace Update {

// Скачивает файл релиза и готовит замену запущенного приложения.
//
// На macOS обновление ставится, только когда приложение запущено из
// установленного бандла (.app), а не из каталога сборки: DMG монтируется,
// новая копия кладётся рядом с временными файлами, а затем отдельный
// скрипт-помощник дожидается выхода приложения, подменяет бандл и
// запускает новую версию. Запущенное приложение не может переписать
// собственный бандл, поэтому подмена делается после выхода.
//
// На остальных системах установка не поддерживается: вызывающий должен
// открыть страницу релиза.
class UpdateInstaller : public QObject {
  Q_OBJECT

public:
  enum Stage {
    Idle = 0,    // Ничего не делаем.
    Downloading, // Качаем DMG.
    Unpacking,   // Монтируем и копируем новую версию.
    Ready,       // Замена подготовлена: после выхода приложение обновится.
    Failed       // Не удалось скачать или подготовить замену.
  };

  Q_ENUM(Stage)

  explicit UpdateInstaller(QObject *parent = nullptr);

  Stage stage() const;
  double progress() const;
  QString status() const;

  // Можно ли ставить обновление на этой системе и из этого места запуска.
  bool isSupported() const;

  // Почему установка недоступна (для интерфейса).
  QString unsupportedReason() const;

  // Скачивает файл и готовит замену. По завершении — finished().
  void start(const ReleaseAsset &asset, const QString &version);

  void cancel();

signals:
  void stageChanged();
  void progressChanged();
  void statusChanged();
  void finished(const QString &version);
  void failed(const QString &message);

private:
  void setStage(Stage stage);
  void setStatus(const QString &status);
  void startDownload();
  void handleDownloaded();
  void prepareReplacement();
  void fail(const QString &message);
  QString bundlePath() const;

  QNetworkAccessManager *m_network = nullptr;
  QPointer<QNetworkReply> m_reply;

  ReleaseAsset m_asset;
  QString m_version;
  QString m_downloadPath;
  QString m_mountPath;
  QString m_stagedAppPath;
  QString m_scriptPath;
  Stage m_stage = Idle;
  double m_progress = 0.0;
  QString m_status;
  bool m_cancelled = false;
};

} // namespace Update
