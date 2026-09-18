#pragma once

#include "../../Models/Account.h"
#include <QObject>
#include <QString>

class YandexAuth;
class YandexClient;

class AccountService : public QObject {
  Q_OBJECT

public:
  explicit AccountService(YandexAuth *auth, QObject *parent = nullptr);

  void loadAccount();

signals:
  void accountReceived(const Account &account);

  void errorOccurred(const QString &message);

private:
  YandexAuth *m_auth = nullptr;
  YandexClient *m_yandexClient = nullptr;
};
