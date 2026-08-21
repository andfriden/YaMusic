#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

#include "../Models/Account.h"
#include "Catalog/SearchResult.h"

class YandexClient : public QObject
{
    Q_OBJECT

public:
    explicit YandexClient(QObject *parent = nullptr);

    // Sets the OAuth token used for Yandex Music API requests.
    void setToken(const QString &token);

    // Returns true when an OAuth token is available.
    bool hasToken() const;

    // Performs an authenticated GET request to the Yandex Music API.
    QNetworkReply *get(const QString &path);

    // Requests the current Yandex Music account status.
    void getAccountStatus();

    // Performs a Yandex Music search request.
    void search(const QString &query);

signals:
    // Emitted when an API request fails.
    void requestError(const QString &message);

    // Emitted when account information has been received.
    void accountReceived(const Account &account);

    // Emitted when search results have been received.
    void searchReceived(const SearchResults &results);

private:
    // Creates an authenticated HTTP request for the specified API path.
    QNetworkRequest createRequest(const QString &path) const;

    QNetworkAccessManager m_networkManager;
    QString m_token;
};
