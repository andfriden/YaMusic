#include "AppController.h"

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
            emit statusChanged("Yandex API error: " + message);
        });
}

void AppController::testConnection()
{
    qDebug() << "AppController is working";
    emit statusChanged("AppController is working");
}

void AppController::testYandexApi()
{
    const QString token =
        QProcessEnvironment::systemEnvironment()
            .value("YANDEX_MUSIC_TOKEN");

    if (token.isEmpty()) {
        emit statusChanged(
            "YANDEX_MUSIC_TOKEN is not set");
        return;
    }

    m_yandexClient->setToken(token);
    m_yandexClient->getAccountStatus();

    emit statusChanged("Requesting Yandex account status...");
}
