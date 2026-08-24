#include "AlbumService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

namespace
{

Track parseTrack(
    const QJsonObject &object)
{
    Track track;

    /*
     * Track ID
     */

    track.id =
        object.value("id")
            .toString();

    if (
        track.id.isEmpty()
    ) {

        const qint64 realId =
            object.value("realId")
                .toInteger();

        if (
            realId > 0
        ) {

            track.id =
                QString::number(
                    realId);
        }
    }

    /*
     * Basic track information
     */

    track.title =
        object.value("title")
            .toString();

    track.coverUri =
        object.value("coverUri")
            .toString();

    track.durationMs =
        object.value("durationMs")
            .toInt();

    /*
     * Artists
     */

    const QJsonArray artists =
        object.value("artists")
            .toArray();

    for (
        const QJsonValue &value :
        artists
    ) {

        if (
            !value.isObject()
        ) {
            continue;
        }

        const QJsonObject artistObject =
            value.toObject();

        Artist artist;

        const qint64 artistId =
            artistObject
                .value("id")
                .toInteger();

        if (
            artistId > 0
        ) {

            artist.id =
                QString::number(
                    artistId);
        }

        artist.name =
            artistObject
                .value("name")
                .toString();

        artist.coverUri =
            artistObject
                .value("coverUri")
                .toString();

        if (
            artist.coverUri.isEmpty()
        ) {

            const QJsonObject cover =
                artistObject
                    .value("cover")
                    .toObject();

            artist.coverUri =
                cover
                    .value("uri")
                    .toString();
        }

        if (
            !artist.name.isEmpty()
        ) {

            track.artists.append(
                artist);
        }
    }

    /*
     * Albums
     */

    const QJsonArray albums =
        object.value("albums")
            .toArray();

    for (
        const QJsonValue &value :
        albums
    ) {

        if (
            !value.isObject()
        ) {
            continue;
        }

        const QJsonObject albumObject =
            value.toObject();

        Album album;

        const qint64 albumId =
            albumObject
                .value("id")
                .toInteger();

        if (
            albumId > 0
        ) {

            album.id =
                QString::number(
                    albumId);
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
            album.coverUri.isEmpty()
        ) {

            const QJsonObject cover =
                albumObject
                    .value("cover")
                    .toObject();

            album.coverUri =
                cover
                    .value("uri")
                    .toString();
        }

        if (
            !album.title.isEmpty()
        ) {

            track.albums.append(
                album);
        }
    }

    return track;
}

}


AlbumService::AlbumService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
}


void AlbumService::loadAlbum(
    const QString &id)
{
    /*
     * Authentication
     */

    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    ) {

        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }

    /*
     * Validate ID
     */

    const QString albumId =
        id.trimmed();

    if (
        albumId.isEmpty()
    ) {

        emit errorOccurred(
            "ID альбома не указан");

        return;
    }

    m_yandexClient->setToken(
        m_auth->token());

    /*
     * -------------------------------------------------
     * Album endpoint
     * -------------------------------------------------
     *
     * /albums/{id}/with-tracks
     */

    const QString path =
        QString(
            "/albums/%1/with-tracks")
            .arg(
                albumId);

    QNetworkReply *reply =
        m_yandexClient->get(
            path);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this,
         reply,
         albumId]() {

            const QByteArray data =
                reply->readAll();

            /*
             * Network error
             */

            if (
                reply->error() !=
                QNetworkReply::NoError
            ) {

                emit errorOccurred(
                    reply->errorString());

                reply->deleteLater();

                return;
            }

            /*
             * Parse JSON
             */

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

                emit errorOccurred(
                    "Некорректный ответ альбома");

                reply->deleteLater();

                return;
            }

            /*
             * API may return:
             *
             * {
             *     "result": {...}
             * }
             *
             * or the object directly.
             */

            const QJsonObject root =
                document.object();

            QJsonObject albumObject;

            if (
                root.value("result")
                    .isObject()
            ) {

                albumObject =
                    root.value("result")
                        .toObject();

            } else {

                albumObject =
                    root;
            }

            if (
                albumObject.isEmpty()
            ) {

                emit errorOccurred(
                    "Ответ альбома пуст");

                reply->deleteLater();

                return;
            }

            AlbumDetails albumDetails;

            /*
             * -------------------------------------------------
             * Album metadata
             * -------------------------------------------------
             */

            const qint64 parsedAlbumId =
                albumObject
                    .value("id")
                    .toInteger();

            if (
                parsedAlbumId > 0
            ) {

                albumDetails.album.id =
                    QString::number(
                        parsedAlbumId);

            } else {

                albumDetails.album.id =
                    albumId;
            }

            albumDetails.album.title =
                albumObject
                    .value("title")
                    .toString();

            albumDetails.album.coverUri =
                albumObject
                    .value("coverUri")
                    .toString();

            albumDetails.album.year =
                albumObject
                    .value("year")
                    .toInt();

            /*
             * Some responses use:
             *
             * cover.uri
             */

            if (
                albumDetails.album.coverUri.isEmpty()
            ) {

                const QJsonObject cover =
                    albumObject
                        .value("cover")
                        .toObject();

                albumDetails.album.coverUri =
                    cover
                        .value("uri")
                        .toString();
            }

            albumDetails.description =
                albumObject
                    .value("description")
                    .toString();

            albumDetails.trackCount =
                albumObject
                    .value("trackCount")
                    .toInt();

            /*
             * -------------------------------------------------
             * Main track structure
             * -------------------------------------------------
             *
             * volumes:
             *
             * [
             *     [
             *         track,
             *         track,
             *         ...
             *     ]
             * ]
             */

            const QJsonArray volumes =
                albumObject
                    .value("volumes")
                    .toArray();

            for (
                const QJsonValue &volumeValue :
                volumes
            ) {

                if (
                    !volumeValue.isArray()
                ) {
                    continue;
                }

                const QJsonArray volume =
                    volumeValue.toArray();

                for (
                    const QJsonValue &value :
                    volume
                ) {

                    if (
                        !value.isObject()
                    ) {
                        continue;
                    }

                    QJsonObject trackObject =
                        value.toObject();

                    /*
                     * Some responses wrap
                     * the actual track.
                     */

                    if (
                        trackObject
                            .value("track")
                            .isObject()
                    ) {

                        trackObject =
                            trackObject
                                .value("track")
                                .toObject();
                    }

                    if (
                        trackObject.isEmpty()
                    ) {
                        continue;
                    }

                    const Track track =
                        parseTrack(
                            trackObject);

                    if (
                        !track.id.isEmpty()
                    ) {

                        albumDetails.tracks.append(
                            track);
                    }
                }
            }

            /*
             * -------------------------------------------------
             * Fallback:
             * tracks directly in album object
             * -------------------------------------------------
             */

            if (
                albumDetails.tracks.isEmpty()
            ) {

                const QJsonArray tracks =
                    albumObject
                        .value("tracks")
                        .toArray();

                for (
                    const QJsonValue &value :
                    tracks
                ) {

                    if (
                        !value.isObject()
                    ) {
                        continue;
                    }

                    QJsonObject trackObject =
                        value.toObject();

                    if (
                        trackObject
                            .value("track")
                            .isObject()
                    ) {

                        trackObject =
                            trackObject
                                .value("track")
                                .toObject();
                    }

                    if (
                        trackObject.isEmpty()
                    ) {
                        continue;
                    }

                    const Track track =
                        parseTrack(
                            trackObject);

                    if (
                        !track.id.isEmpty()
                    ) {

                        albumDetails.tracks.append(
                            track);
                    }
                }
            }

            /*
             * If API did not provide
             * trackCount, use parsed count.
             */

            if (
                albumDetails.trackCount <= 0
            ) {

                albumDetails.trackCount =
                    albumDetails.tracks.size();
            }

            /*
             * Debug
             */

            qDebug()
                << "Album loaded:"
                << albumDetails.album.title
                << "| id:"
                << albumDetails.album.id
                << "| tracks:"
                << albumDetails.tracks.size();

            /*
             * Notify listeners
             */

            emit albumReceived(
                albumDetails);

            reply->deleteLater();
        });
}