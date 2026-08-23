#pragma once

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointer>
#include <QString>
#include <QStringList>

#include "../Models/Account.h"
#include "../Models/Track.h"
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

    void getTracks(
        const QStringList &trackIds);

    signals:
        void requestError(
            const QString &message);

    void accountReceived(
        const Account &account);

    void searchReceived(
        const SearchResults &results);

    void tracksReceived(
        const QList<Track> &tracks);

private:
    QNetworkRequest createRequest(
        const QString &path) const;

    QList<Track> parseTracks(
        const QJsonObject &object) const;

    Track parseTrack(
        const QJsonObject &object) const;

    QNetworkAccessManager
        m_networkManager;

    QString m_token;

    QPointer<QNetworkReply>
        m_searchReply;

    QPointer<QNetworkReply>
        m_tracksReply;
};