#include "AppController.h"

#include "../Models/Account.h"
#include "../Yandex/Auth/YandexAuth.h"
#include "../Yandex/YandexClient.h"

#include <QDebug>

AppController::AppController(QObject *parent)
    : QObject(parent)
    , m_yandexAuth(new YandexAuth(this))
    , m_yandexClient(new YandexClient(this))
{
    connect(
        m_yandexClient,
        &YandexClient::requestError,
        this,
        [this](const QString &message) {
            qDebug() << "Yandex API error:" << message;
            emit statusChanged("Yandex API error: " + message);
        });

    connect(
        m_yandexClient,
        &YandexClient::accountReceived,
        this,
        [this](const Account &account) {
            const QString message =
                QString("Logged in as %1 (uid: %2)")
                    .arg(account.displayName)
                    .arg(account.uid);

            qDebug() << message;
            emit statusChanged(message);
        });

    m_yandexAuth->loadFromEnvironment();
}

void AppController::testConnection()
{
    qDebug() << "AppController is working";
    emit statusChanged("AppController is working");
}

void AppController::testYandexApi()
{
    qDebug() << "testYandexApi() called";

    if (!m_yandexAuth->isAuthenticated()) {
        emit statusChanged("Yandex Music token is not set");
        return;
    }

    m_yandexClient->setToken(m_yandexAuth->token());

    qDebug() << "Requesting Yandex account status...";

    emit statusChanged("Requesting Yandex account status...");

    m_yandexClient->getAccountStatus();
}
