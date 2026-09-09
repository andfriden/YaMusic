#pragma once

#include <QObject>
#include <QString>

#include "../Models/Account.h"

class AccountService;
class YandexAuth;

class AuthController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool authenticated
        READ authenticated
        NOTIFY authenticatedChanged
    )

    Q_PROPERTY(
        QString displayName
        READ displayName
        NOTIFY accountChanged
    )

    Q_PROPERTY(
        QString login
        READ login
        NOTIFY accountChanged
    )

public:
    explicit AuthController(
        YandexAuth *auth,
        AccountService *accountService,
        QObject *parent = nullptr);

    bool authenticated() const;

    QString displayName() const;
    QString login() const;

    Q_INVOKABLE void loginWithToken(const QString &token);
    Q_INVOKABLE void logout();

    signals:
        void authenticatedChanged();
    void accountChanged();

    void errorOccurred(const QString &message);

private:
    void loadAccount();

    YandexAuth *m_auth = nullptr;
    AccountService *m_accountService = nullptr;

    Account m_account;
    bool m_authenticated = false;
};