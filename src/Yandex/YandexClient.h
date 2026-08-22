#pragma once

#include <QObject>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointer>
#include <QString>

#include "../Models/Account.h"
#include "Catalog/SearchResult.h"

class YandexClient : public QObject
{
    Q_OBJECT

public:
    explicit YandexClient(
        QObject *parent = nullptr);

    void setToken(
        const QString &token);

    bool hasToken() const;

    QNetworkReply *get(
        const QString &path);

    QNetworkReply *post(
        const QString &path,
        const QJsonObject &body);

    void getAccountStatus();

    void search(
        const QString &query);

    signals:
        void requestError(
            const QString &message);

    void accountReceived(
        const Account &account);

    void searchReceived(
        const SearchResults &results);

private:
    QNetworkRequest createRequest(
        const QString &path) const;

    QNetworkAccessManager m_networkManager;

    QString m_token;

    QPointer<QNetworkReply> m_searchReply;
};