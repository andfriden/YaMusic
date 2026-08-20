#include "AppController.h"

#include "../Models/Account.h"
#include "../Yandex/YandexClient.h"

#include <QDebug>
#include <QProcessEnvironment>

AppController::AppController(QObject *parent)
    : QObject(parent)
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
}

void AppController::testConnection()
{
    qDebug() << "AppController is working";
    emit statusChanged("AppController is working");
}

void AppController::testYandexApi()
{
    qDebug() << "testYandexApi() called";

    const QString token =
        QProcessEnvironment::systemEnvironment()
            .value("YANDEX_MUSIC_TOKEN");

    qDebug() << "Token available:" << !token.isEmpty();

    if (token.isEmpty()) {
        emit statusChanged("YANDEX_MUSIC_TOKEN is not set");
        return;
    }

    m_yandexClient->setToken(token);

    qDebug() << "Requesting Yandex account status...";

    emit statusChanged("Requesting Yandex account status...");

    m_yandexClient->getAccountStatus();
}
