#include "YandexClient.h"

#include "AccountParser.h"
#include "Catalog/SearchParser.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrlQuery>

namespace
{
constexpr auto YandexApiBaseUrl =
    "https://api.music.yandex.net";
}

YandexClient::YandexClient(
    QObject *parent)
    : QObject(parent)
{
}

void YandexClient::setToken(
    const QString &token)
{
    m_token = token.trimmed();
}

bool YandexClient::hasToken() const
{
    return !m_token.isEmpty();
}

QNetworkRequest YandexClient::createRequest(
    const QString &path) const
{
    QUrl url;

    if (path.startsWith("http://") ||
        path.startsWith("https://")) {

        url = QUrl(path);

    } else {

        url = QUrl(
            QString(YandexApiBaseUrl) +
            path);
    }

    QNetworkRequest request{
        url
    };

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json");

    request.setAttribute(
        QNetworkRequest::Http2AllowedAttribute,
        false);

    if (hasToken()) {

        request.setRawHeader(
            "Authorization",
            QByteArray("OAuth ") +
                m_token.toUtf8());
    }

    return request;
}

QNetworkReply *YandexClient::get(
    const QString &path)
{
    return m_networkManager.get(
        createRequest(path));
}

QNetworkReply *YandexClient::post(
    const QString &path,
    const QJsonObject &body)
{
    const QJsonDocument document(
        body);

    return m_networkManager.post(
        createRequest(path),
        document.toJson(
            QJsonDocument::Compact));
}

void YandexClient::getAccountStatus()
{
    auto *reply =
        get("/account/status");

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]() {

            const QByteArray data =
                reply->readAll();

            if (reply->error() !=
                QNetworkReply::NoError) {

                emit requestError(
                    reply->errorString());

                reply->deleteLater();

                return;
            }

            QJsonParseError parseError;

            const QJsonDocument document =
                QJsonDocument::fromJson(
                    data,
                    &parseError);

            if (parseError.error !=
                    QJsonParseError::NoError ||
                !document.isObject()) {

                emit requestError(
                    "Некорректный ответ от Яндекс Музыки");

                reply->deleteLater();

                return;
            }

            const Account account =
                AccountParser::parse(
                    document.object());

            emit accountReceived(
                account);

            reply->deleteLater();
        });
}

void YandexClient::search(
    const QString &query)
{
    if (m_searchReply) {

        m_searchReply->abort();

        m_searchReply->deleteLater();

        m_searchReply.clear();
    }

    QUrlQuery queryParameters;

    queryParameters.addQueryItem(
        "text",
        query);

    queryParameters.addQueryItem(
        "page",
        "0");

    queryParameters.addQueryItem(
        "type",
        "all");

    const QString path =
        "/search?" +
        queryParameters.toString(
            QUrl::FullyEncoded);

    auto *reply =
        get(path);

    m_searchReply = reply;

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]() {

            if (reply != m_searchReply) {

                reply->deleteLater();

                return;
            }

            m_searchReply.clear();

            const QByteArray data =
                reply->readAll();

            if (reply->error() !=
                QNetworkReply::NoError) {

                emit requestError(
                    reply->errorString());

                reply->deleteLater();

                return;
            }

            QJsonParseError parseError;

            const QJsonDocument document =
                QJsonDocument::fromJson(
                    data,
                    &parseError);

            if (parseError.error !=
                    QJsonParseError::NoError ||
                !document.isObject()) {

                emit requestError(
                    "Некорректный ответ поиска");

                reply->deleteLater();

                return;
            }

            const SearchResults results =
                SearchParser::parse(
                    document.object());

            emit searchReceived(
                results);

            reply->deleteLater();
        });
}