#pragma once

#include <QObject>
#include <QString>

#include "SearchResult.h"

class YandexAuth;
class YandexClient;

// Provides search functionality for Yandex Music.
class SearchService : public QObject
{
    Q_OBJECT

public:
    explicit SearchService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    // Searches Yandex Music for the specified query.
    void search(const QString &query);

signals:

    void searchStarted();

    // Emitted when search results have been loaded successfully.
    void searchReceived(const SearchResults &results);

    // Emitted when a search request fails.
    void errorOccurred(const QString &message);

private:
    YandexAuth *m_auth = nullptr;
    YandexClient *m_yandexClient = nullptr;
};
