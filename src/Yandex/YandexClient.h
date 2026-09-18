#pragma once

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QUrlQuery>
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

    QNetworkReply *
    get(
        const QString &path);

    QNetworkReply *
    post(
        const QString &path,
        const QJsonObject &body);

    QNetworkReply *
    postForm(
        const QString &path,
        const QUrlQuery &body);

    QNetworkReply *
    rawPost(
        const QNetworkRequest &request,
        const QByteArray &data);

    void getAccountStatus();

    QNetworkRequest createRequest(
        const QString &path) const;

    void search(
        const QString &query,
        int page = 0);

    void getTracks(
        const QStringList &trackIds);

    /*
     * Отправляет факт прослушивания трека на сервер
     * (POST /play-audio). Сервер сам добавляет трек
     * в «Недавно прослушаны» и учитывает в рекомендациях.
     *
     * Параметры передаются как form-urlencoded:
     *   track-id, album-id, uid, from, from-cache,
     *   timestamp, client-now, track-length-seconds,
     *   total-played-seconds, end-position-seconds.
     */
    void reportPlayback(
        const QString &trackId,
        const QString &albumId,
        const QString &uid,
        bool fromCache,
        int trackLengthSeconds,
        int playedSeconds,
        int endPositionSeconds);

    signals:

        void requestError(
            const QString &message);

    void accountReceived(
        const Account &account);

    void searchReceived(
        const SearchResults &results);

    void tracksReceived(
        const QList<Track> &tracks);

    /*
     * true — сервер принял факт прослушивания (result == "ok").
     */
    void playbackReported(
        bool ok);

private:

    QNetworkAccessManager
        m_networkManager;

    QString
        m_token;

    QPointer<QNetworkReply>
        m_searchReply;

    QPointer<QNetworkReply>
        m_tracksReply;
};