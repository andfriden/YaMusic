#pragma once

#include <QObject>

#include "../../Models/Account.h"

class YandexAuth;
class YandexClient;

// Provides access to the authenticated Yandex Music account.
class AccountService : public QObject
{
    Q_OBJECT

public:
    explicit AccountService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    // Requests the current account information from Yandex Music.
    void loadAccount();

signals:
    // Emitted when account information has been loaded successfully.
    void accountReceived(const Account &account);

    // Emitted when loading the account fails.
    void errorOccurred(const QString &message);

private:
    YandexAuth *m_auth = nullptr;
    YandexClient *m_yandexClient = nullptr;
};
