#include "SearchService.h"
#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

SearchService::SearchService(
    YandexAuth *auth,
    QObject *parent)
    : YandexServiceBase(auth, parent)
{
    connect(
        m_yandexClient,
        &YandexClient::searchReceived,
        this,
        [this](const SearchResults &results)
        {
            emit searchReceived(
                results);
        });

    connect(
        m_yandexClient,
        &YandexClient::requestError,
        this,
        [this](const QString &message)
        {

            emit errorOccurred(
                message);
        });
}

void SearchService::search(
    const QString &query)
{
    if (!ensureAuthenticated()) {

        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
        }

    const QString trimmedQuery =
        query.trimmed();

    if (trimmedQuery.isEmpty()) {

        emit errorOccurred(
            "Search query is empty");

        return;
    }

    emit searchStarted();

    m_yandexClient->search(
        trimmedQuery);
}