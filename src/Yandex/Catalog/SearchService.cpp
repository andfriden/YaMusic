#include "SearchService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QDebug>

// Creates the search service and initializes its Yandex API client.
SearchService::SearchService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(new YandexClient(this))
{
    connect(
        m_yandexClient,
        &YandexClient::searchReceived,
        this,
        [this](const SearchResults &results) {
            emit searchReceived(results);
        });

    connect(
        m_yandexClient,
        &YandexClient::requestError,
        this,
        [this](const QString &message) {
            qDebug()
                << "SearchService error:"
                << message;

            emit errorOccurred(message);
        });
}

// Searches Yandex Music for the specified query.
void SearchService::search(const QString &query)
{
    if (m_auth == nullptr ||
        !m_auth->isAuthenticated()) {
        emit errorOccurred(
            "Yandex Music token is not set");
        return;
    }

    const QString trimmedQuery =
        query.trimmed();

    if (trimmedQuery.isEmpty()) {
        emit errorOccurred(
            "Search query is empty");
        return;
    }

    m_yandexClient->setToken(
        m_auth->token());

    m_yandexClient->search(
        trimmedQuery);
}
