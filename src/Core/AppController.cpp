#include "AppController.h"

#include "../Yandex/Account/AccountService.h"
#include "../Yandex/Auth/YandexAuth.h"

#include <QDebug>

// Creates the application controller and initializes Yandex services.
AppController::AppController(QObject *parent)
    : QObject(parent)
    , m_yandexAuth(new YandexAuth(this))
    , m_accountService(new AccountService(m_yandexAuth, this))
{
    connect(
        m_accountService,
        &AccountService::accountReceived,
        this,
        [this](const Account &account) {
            const QString message =
                QString("Logged in as %1 (uid: %2)")
                    .arg(account.displayName)
                    .arg(account.uid);

            qDebug() << message;

            emit statusChanged(message);
        });

    connect(
        m_accountService,
        &AccountService::errorOccurred,
        this,
        [this](const QString &message) {
            qDebug() << "Yandex API error:" << message;

            emit statusChanged(
                "Yandex API error: " + message);
        });

    // Load the authentication token for the current session.
    m_yandexAuth->loadFromEnvironment();
}

// Performs a simple application controller test.
void AppController::testConnection()
{
    qDebug() << "AppController is working";

    emit statusChanged(
        "AppController is working");
}

// Requests the current Yandex Music account.
void AppController::testYandexApi()
{
    qDebug() << "testYandexApi() called";

    emit statusChanged(
        "Requesting Yandex account status...");

    m_accountService->loadAccount();
}
