#pragma once

#include "ReleaseInfo.h"
#include <QObject>
#include <QPointer>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

namespace Update {

// Спрашивает у GitHub список релизов и решает, есть ли версия новее
// запущенной. Проверка тихая: об ошибке и об отсутствии обновления
// сообщают сигналы, а показывать это пользователю или нет — решает
// UpdateController.
//
// Релизы проекта помечены как pre-release, поэтому список берётся целиком:
// отдельный эндпоинт /releases/latest такие релизы отбрасывает и всегда
// отвечал бы «обновлений нет».
class UpdateChecker : public QObject {
  Q_OBJECT

public:
  explicit UpdateChecker(QObject *parent = nullptr);

  // Версия запущенного приложения; по умолчанию — версия приложения Qt
  // (её выставляет main.cpp из PROJECT_VERSION).
  void setCurrentVersion(const QString &version);

  QString currentVersion() const;

  bool isChecking() const;

  // Релиз, найденный последней удачной проверкой; пустой, если новее нет.
  const ReleaseInfo &availableRelease() const;

  // Файл найденного релиза для текущей системы и разрядности. Пустой,
  // если для этой системы сборки в релизе нет.
  ReleaseAsset assetForThisSystem() const;

  // Имя файла, который нужен этой системе для версии version.
  static QString expectedAssetName(const QString &version);

  void check();

signals:
  void checkingChanged();
  void updateAvailable(const ReleaseInfo &release);
  void upToDate(const QString &currentVersion);
  void checkFailed(const QString &message);

private:
  void handleReply(QNetworkReply *reply);
  ReleaseInfo releaseFromJson(const QJsonObject &object) const;

  QNetworkAccessManager *m_network = nullptr;
  QPointer<QNetworkReply> m_reply;
  QString m_repository = QStringLiteral("andfriden/YaMusic");
  QString m_currentVersion;
  ReleaseInfo m_release;
  bool m_checking = false;
};

} // namespace Update
