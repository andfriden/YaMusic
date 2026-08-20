#include "YandexClient.h"

#include <QDebug>
#include <QNetworkRequest>

namespace
{
constexpr auto YandexApiBaseUrl = "https://api.music.yandex.net";
}

YandexClient::YandexClient(QObject *parent)
    : QObject(parent)
{
}

void YandexClient::setToken(const QString &token)
{
    m_token = token.trimmed();
}

bool YandexClient::hasToken() const
{
    return !m_token.isEmpty();
}

QNetworkRequest YandexClient::createRequest(const QString &path) const
{
    QNetworkRequest request(
        QUrl(QString(YandexApiBaseUrl) + path));

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json");


    request.setAttribute(
    QNetworkRequest::Http2AllowedAttribute,
    false);

    if (hasToken()) {
        request.setRawHeader(
            "Authorization",
            QByteArray("OAuth ") + m_token.toUtf8());
    }

    return request;
}

QNetworkReply *YandexClient::get(const QString &path)
{
    return m_networkManager.get(createRequest(path));
}

void YandexClient::getAccountStatus()
{
    auto *reply = get("/account/status");

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            emit requestError(reply->errorString());
            qDebug() << "Yandex API error:" << reply->errorString();
            qDebug() << "Response:" << data;
        } else {
            qDebug() << "Yandex API response:" << data;
        }

        reply->deleteLater();
    });
}
