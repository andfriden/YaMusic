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
constexpr auto YandexApiBaseUrl = "https://api.music.yandex.net";
}

// Creates the Yandex Music API client.
YandexClient::YandexClient(QObject *parent)
    : QObject(parent)
{
}

// Sets the OAuth token used for API requests.
void YandexClient::setToken(const QString &token)
{
    m_token = token.trimmed();
}

// Returns true when an OAuth token is available.
bool YandexClient::hasToken() const
{
    return !m_token.isEmpty();
}

// Creates an authenticated request for the specified API path.
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

// Performs an authenticated GET request.
QNetworkReply *YandexClient::get(const QString &path)
{
    return m_networkManager.get(createRequest(path));
}

// Requests the current Yandex Music account status.
void YandexClient::getAccountStatus()
{
    auto *reply = get("/account/status");

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]() {
            const QByteArray data = reply->readAll();

            if (reply->error() != QNetworkReply::NoError) {
                emit requestError(reply->errorString());
                reply->deleteLater();
                return;
            }

            QJsonParseError parseError;

            const QJsonDocument document =
                QJsonDocument::fromJson(data, &parseError);

            if (parseError.error != QJsonParseError::NoError ||
                !document.isObject()) {
                emit requestError(
                    "Invalid JSON response from Yandex Music");
                reply->deleteLater();
                return;
            }

            const Account account =
                AccountParser::parse(document.object());

            emit accountReceived(account);

            reply->deleteLater();
        });
}

// Performs a Yandex Music search request.
void YandexClient::search(const QString &query)
{
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
        "/search?" + queryParameters.toString(QUrl::FullyEncoded);

    auto *reply = get(path);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]() {
            const QByteArray data = reply->readAll();

            if (reply->error() != QNetworkReply::NoError) {
                emit requestError(reply->errorString());
                reply->deleteLater();
                return;
            }

            QJsonParseError parseError;

            const QJsonDocument document =
                QJsonDocument::fromJson(data, &parseError);

            if (parseError.error != QJsonParseError::NoError ||
                !document.isObject()) {
                emit requestError(
                    "Invalid search response from Yandex Music");
                reply->deleteLater();
                return;
            }

            const SearchResults results =
                SearchParser::parse(document.object());

            emit searchReceived(results);

            reply->deleteLater();
        });
}
