#include "YandexClient.h"

#include "AccountParser.h"
#include "Catalog/SearchParser.h"

#include <QDebug>
#include <QJsonArray>
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
    m_token =
        token.trimmed();
}

bool YandexClient::hasToken() const
{
    return !m_token.isEmpty();
}

QNetworkRequest
YandexClient::createRequest(
    const QString &path) const
{
    QUrl url;

    if (
        path.startsWith("http://") ||
        path.startsWith("https://")
    ) {
        url =
            QUrl(path);
    } else {
        url =
            QUrl(
                QString(
                    YandexApiBaseUrl) +
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

QNetworkReply *
YandexClient::get(
    const QString &path)
{
    return m_networkManager.get(
        createRequest(path));
}

QNetworkReply *
YandexClient::post(
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
    QNetworkReply *reply =
        get(
            "/account/status");

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]() {

            const QByteArray data =
                reply->readAll();

            if (
                reply->error() !=
                QNetworkReply::NoError
            ) {

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

            if (
                parseError.error !=
                    QJsonParseError::NoError ||
                !document.isObject()
            ) {

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

        m_searchReply
            ->abort();

        m_searchReply
            ->deleteLater();

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

    QNetworkReply *reply =
        get(path);

    m_searchReply =
        reply;

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]() {

            if (
                reply != m_searchReply
            ) {

                reply->deleteLater();

                return;
            }

            m_searchReply.clear();

            const QByteArray data =
                reply->readAll();

            if (
                reply->error() !=
                QNetworkReply::NoError
            ) {

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

            if (
                parseError.error !=
                    QJsonParseError::NoError ||
                !document.isObject()
            ) {

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

void YandexClient::getTracks(
    const QStringList &trackIds)
{
    if (m_tracksReply) {

        m_tracksReply
            ->abort();

        m_tracksReply
            ->deleteLater();

        m_tracksReply.clear();
    }

    QStringList normalizedIds;

    for (
        const QString &trackId :
        trackIds
    ) {

        const QString id =
            trackId.trimmed();

        if (id.isEmpty()) {
            continue;
        }

        if (
            !normalizedIds.contains(id)
        ) {

            normalizedIds.append(id);
        }
    }

    if (normalizedIds.isEmpty()) {

        emit tracksReceived({});

        return;
    }

    QNetworkRequest request{
        QUrl(
            QString(
                YandexApiBaseUrl) +
            "/tracks")
    };

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/x-www-form-urlencoded");

    request.setAttribute(
        QNetworkRequest::Http2AllowedAttribute,
        false);

    if (hasToken()) {

        request.setRawHeader(
            "Authorization",
            QByteArray("OAuth ") +
                m_token.toUtf8());
    }

    QUrlQuery body;

    body.addQueryItem(
        "track-ids",
        normalizedIds.join(","));

    body.addQueryItem(
        "with-positions",
        "false");

    const QByteArray bodyData =
        body.query(
            QUrl::FullyEncoded)
            .toUtf8();

    QNetworkReply *reply =
        m_networkManager.post(
            request,
            bodyData);

    m_tracksReply =
        reply;

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, normalizedIds]() {

            if (
                reply != m_tracksReply
            ) {

                reply->deleteLater();

                return;
            }

            m_tracksReply.clear();

            const QByteArray data =
                reply->readAll();

            if (
                reply->error() !=
                QNetworkReply::NoError
            ) {

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

            if (
                parseError.error !=
                    QJsonParseError::NoError ||
                !document.isObject()
            ) {

                emit requestError(
                    "Некорректный ответ списка треков");

                reply->deleteLater();

                return;
            }

            const QList<Track> tracks =
                parseTracks(
                    document.object());

            qDebug()
                << "Tracks loaded:"
                << tracks.size()
                << "/ requested:"
                << normalizedIds.size();

            emit tracksReceived(
                tracks);

            reply->deleteLater();
        });
}

QList<Track>
YandexClient::parseTracks(
    const QJsonObject &object) const
{
    QList<Track> tracks;

    const QJsonArray results =
        object
            .value("result")
            .toArray();

    for (
        const QJsonValue &value :
        results
    ) {

        if (!value.isObject()) {
            continue;
        }

        const Track track =
            parseTrack(
                value.toObject());

        if (
            track.id.isEmpty()
        ) {
            continue;
        }

        tracks.append(
            track);
    }

    return tracks;
}

Track YandexClient::parseTrack(
    const QJsonObject &trackObject) const
{
    Track track;

    /*
     * Yandex may return ID as either
     * a JSON string or a number.
     */
    const QJsonValue idValue =
        trackObject.value("id");

    if (
        idValue.isString()
    ) {

        track.id =
            idValue.toString();

    } else if (
        idValue.isDouble()
    ) {

        const qint64 id =
            idValue.toInteger();

        if (id > 0) {

            track.id =
                QString::number(
                    id);
        }
    }

    track.title =
        trackObject
            .value("title")
            .toString();

    track.coverUri =
        trackObject
            .value("coverUri")
            .toString();

    track.durationMs =
        trackObject
            .value("durationMs")
            .toInt();

    const QJsonArray artists =
        trackObject
            .value("artists")
            .toArray();

    for (
        const QJsonValue &artistValue :
        artists
    ) {

        if (
            !artistValue.isObject()
        ) {
            continue;
        }

        const QJsonObject artistObject =
            artistValue.toObject();

        Artist artist;

        const QJsonValue artistIdValue =
            artistObject.value("id");

        if (
            artistIdValue.isString()
        ) {

            artist.id =
                artistIdValue.toString();

        } else if (
            artistIdValue.isDouble()
        ) {

            const qint64 id =
                artistIdValue.toInteger();

            if (id > 0) {

                artist.id =
                    QString::number(id);
            }
        }

        artist.name =
            artistObject
                .value("name")
                .toString();

        if (
            !artist.id.isEmpty() ||
            !artist.name.isEmpty()
        ) {

            track.artists.append(
                artist);
        }
    }

    const QJsonArray albums =
        trackObject
            .value("albums")
            .toArray();

    for (
        const QJsonValue &albumValue :
        albums
    ) {

        if (
            !albumValue.isObject()
        ) {
            continue;
        }

        const QJsonObject albumObject =
            albumValue.toObject();

        Album album;

        const QJsonValue albumIdValue =
            albumObject.value("id");

        if (
            albumIdValue.isString()
        ) {

            album.id =
                albumIdValue.toString();

        } else if (
            albumIdValue.isDouble()
        ) {

            const qint64 id =
                albumIdValue.toInteger();

            if (id > 0) {

                album.id =
                    QString::number(id);
            }
        }

        album.title =
            albumObject
                .value("title")
                .toString();

        album.coverUri =
            albumObject
                .value("coverUri")
                .toString();

        album.year =
            albumObject
                .value("year")
                .toInt();

        if (
            !album.id.isEmpty() ||
            !album.title.isEmpty()
        ) {

            track.albums.append(
                album);
        }
    }

    return track;
}