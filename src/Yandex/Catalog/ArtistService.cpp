#include "ArtistService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrlQuery>

#include <memory>

namespace
{

Track parseTrack(
    const QJsonObject &object)
{
    Track track;

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


Album parseAlbum(
    const QJsonObject &object)
{
    QJsonObject albumObject =
        object;

    if (
        object.value("album")
            .isObject()
    ) {

        albumObject =
            object.value("album")
                .toObject();
    }

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

    return album;
}


Artist parseArtist(
    const QJsonObject &object)
{
    QJsonObject artistObject =
        object;

    if (
        object.value("artist")
            .isObject()
    ) {

        artistObject =
            object.value("artist")
                .toObject();
    }

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
        artist.coverUri.isEmpty()
    ) {

        artist.coverUri =
            artistObject
                .value("ogImage")
                .toString();
    }

    return artist;
}


QJsonObject resultObject(
    const QJsonDocument &document)
{
    if (
        !document.isObject()
    ) {
        return {};
    }

    const QJsonObject root =
        document.object();

    if (
        root.value("result")
            .isObject()
    ) {

        return root.value("result")
            .toObject();
    }

    return root;
}


QJsonArray firstArray(
    const QJsonObject &object,
    const QStringList &keys)
{
    for (
        const QString &key :
        keys
    ) {

        const QJsonValue value =
            object.value(
                key);

        if (
            value.isArray()
        ) {

            return value.toArray();
        }
    }

    return {};
}


void restoreArtistName(
    ArtistDetails &artist)
{
    if (
        !artist.name.isEmpty() ||
        artist.tracks.isEmpty()
    ) {
        return;
    }

    const Track &firstTrack =
        artist.tracks.first();

    if (
        firstTrack.artists.isEmpty()
    ) {
        return;
    }

    artist.name =
        firstTrack.artists
            .first()
            .name;
}

}


ArtistService::ArtistService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
}


void ArtistService::loadArtist(
    const QString &id)
{
    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    ) {

        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }

    const QString artistId =
        id.trimmed();

    if (
        artistId.isEmpty()
    ) {

        emit errorOccurred(
            "ID исполнителя не указан");

        return;
    }

    m_yandexClient->setToken(
        m_auth->token());

    /*
     * -------------------------------------------------
     * Helper: load additional artist data
     * -------------------------------------------------
     */

    auto loadAdditionalData =
        [this,
         artistId](
            const ArtistDetails &sourceArtist) {

            auto artistData =
                std::make_shared<
                    ArtistDetails>(
                    sourceArtist);

            auto completed =
                std::make_shared<int>(
                    0);

            auto finalize =
                [this,
                 artistData,
                 completed]() {

                    ++(*completed);

                    if (
                        *completed < 3
                    ) {
                        return;
                    }

                    restoreArtistName(
                        *artistData);

                    qDebug()
                        << "Artist loaded:"
                        << artistData->name
                        << "| id:"
                        << artistData->id
                        << "| tracks:"
                        << artistData->tracks.size()
                        << "| albums:"
                        << artistData->popularAlbums.size()
                        << "| new release:"
                        << artistData->newRelease.title
                        << "| similar:"
                        << artistData->similarArtists.size();

                    emit artistReceived(
                        *artistData);
                };

            /*
             * -------------------------------------------------
             * Popular albums
             * -------------------------------------------------
             */

            QUrlQuery popularQuery;

            popularQuery.addQueryItem(
                "page",
                "0");

            popularQuery.addQueryItem(
                "pageSize",
                "5");

            popularQuery.addQueryItem(
                "sortBy",
                "rating");

            const QString popularAlbumsPath =
                QString(
                    "/artists/%1/direct-albums?")
                    .arg(
                        artistId) +
                popularQuery.toString(
                    QUrl::FullyEncoded);

            QNetworkReply *
                popularAlbumsReply =
                    m_yandexClient->get(
                        popularAlbumsPath);

            connect(
                popularAlbumsReply,
                &QNetworkReply::finished,
                this,
                [artistData,
                 popularAlbumsReply,
                 finalize]() {

                    const QByteArray data =
                        popularAlbumsReply
                            ->readAll();

                    if (
                        popularAlbumsReply->error() ==
                        QNetworkReply::NoError
                    ) {

                        QJsonParseError
                            parseError;

                        const QJsonDocument
                            document =
                                QJsonDocument::fromJson(
                                    data,
                                    &parseError);

                        if (
                            parseError.error ==
                                QJsonParseError::NoError &&
                            document.isObject()
                        ) {

                            const QJsonObject
                                result =
                                resultObject(
                                    document);

                            const QJsonArray
                                albums =
                                firstArray(
                                    result,
                                    {
                                        "albums",
                                        "items"
                                    });

                            for (
                                const QJsonValue &value :
                                albums
                            ) {

                                if (
                                    !value.isObject()
                                ) {
                                    continue;
                                }

                                const Album album =
                                    parseAlbum(
                                        value.toObject());

                                if (
                                    !album.id.isEmpty()
                                ) {

                                    artistData
                                        ->popularAlbums
                                        .append(
                                            album);
                                }
                            }
                        }
                    }

                    popularAlbumsReply
                        ->deleteLater();

                    finalize();
                });

            /*
             * -------------------------------------------------
             * New release
             * -------------------------------------------------
             */

            QUrlQuery newestQuery;

            newestQuery.addQueryItem(
                "page",
                "0");

            newestQuery.addQueryItem(
                "pageSize",
                "1");

            newestQuery.addQueryItem(
                "sortBy",
                "year");

            const QString newestAlbumPath =
                QString(
                    "/artists/%1/direct-albums?")
                    .arg(
                        artistId) +
                newestQuery.toString(
                    QUrl::FullyEncoded);

            QNetworkReply *
                newestAlbumReply =
                    m_yandexClient->get(
                        newestAlbumPath);

            connect(
                newestAlbumReply,
                &QNetworkReply::finished,
                this,
                [artistData,
                 newestAlbumReply,
                 finalize]() {

                    const QByteArray data =
                        newestAlbumReply
                            ->readAll();

                    if (
                        newestAlbumReply->error() ==
                        QNetworkReply::NoError
                    ) {

                        QJsonParseError
                            parseError;

                        const QJsonDocument
                            document =
                                QJsonDocument::fromJson(
                                    data,
                                    &parseError);

                        if (
                            parseError.error ==
                                QJsonParseError::NoError &&
                            document.isObject()
                        ) {

                            const QJsonObject
                                result =
                                resultObject(
                                    document);

                            const QJsonArray
                                albums =
                                firstArray(
                                    result,
                                    {
                                        "albums",
                                        "items"
                                    });

                            if (
                                !albums.isEmpty()
                            ) {

                                artistData
                                    ->newRelease =
                                    parseAlbum(
                                        albums.first()
                                            .toObject());
                            }
                        }
                    }

                    newestAlbumReply
                        ->deleteLater();

                    finalize();
                });

            /*
             * -------------------------------------------------
             * Similar artists
             * -------------------------------------------------
             *
             * Supported shapes:
             *
             * result.similarArtists
             * result.similar_artists
             * result.artists
             * result.similar
             * result.items
             *
             * An item may optionally be wrapped
             * in { "artist": {...} }.
             */

            const QString similarPath =
                QString(
                    "/artists/%1/similar")
                    .arg(
                        artistId);

            QNetworkReply *similarReply =
                m_yandexClient->get(
                    similarPath);

            connect(
                similarReply,
                &QNetworkReply::finished,
                this,
                [artistData,
                 similarReply,
                 finalize]() {

                    const QByteArray data =
                        similarReply
                            ->readAll();

                    if (
                        similarReply->error() ==
                        QNetworkReply::NoError
                    ) {

                        QJsonParseError
                            parseError;

                        const QJsonDocument
                            document =
                                QJsonDocument::fromJson(
                                    data,
                                    &parseError);

                        if (
                            parseError.error ==
                                QJsonParseError::NoError &&
                            document.isObject()
                        ) {

                            const QJsonObject
                                result =
                                resultObject(
                                    document);

                            QJsonArray artists =
                                firstArray(
                                    result,
                                    {
                                        "similarArtists",
                                        "similar_artists",
                                        "artists",
                                        "similar",
                                        "items"
                                    });

                            /*
                             * Some API responses can put
                             * similarArtists under "result"
                             * as an object wrapper.
                             */

                            if (
                                artists.isEmpty() &&
                                result
                                    .value(
                                        "similarArtists")
                                    .isObject()
                            ) {

                                const QJsonObject
                                    similarObject =
                                    result
                                        .value(
                                            "similarArtists")
                                        .toObject();

                                artists =
                                    firstArray(
                                        similarObject,
                                        {
                                            "artists",
                                            "items"
                                        });
                            }

                            for (
                                const QJsonValue &value :
                                artists
                            ) {

                                if (
                                    !value.isObject()
                                ) {
                                    continue;
                                }

                                const Artist
                                    similarArtist =
                                    parseArtist(
                                        value.toObject());

                                if (
                                    !similarArtist.id.isEmpty() &&
                                    !similarArtist.name.isEmpty()
                                ) {

                                    bool duplicate =
                                        false;

                                    for (
                                        const Artist &
                                            existing :
                                        artistData
                                            ->similarArtists
                                    ) {

                                        if (
                                            existing.id ==
                                            similarArtist.id
                                        ) {

                                            duplicate =
                                                true;

                                            break;
                                        }
                                    }

                                    if (
                                        !duplicate
                                    ) {

                                        artistData
                                            ->similarArtists
                                            .append(
                                                similarArtist);
                                    }
                                }
                            }
                        }
                    }

                    qDebug()
                        << "Similar artists loaded:"
                        << artistData
                               ->similarArtists
                               .size();

                    similarReply
                        ->deleteLater();

                    finalize();
                });
        };

    /*
     * -------------------------------------------------
     * Artist brief-info
     * -------------------------------------------------
     */

    const QString infoPath =
        QString(
            "/artists/%1/brief-info")
            .arg(
                artistId);

    QNetworkReply *infoReply =
        m_yandexClient->get(
            infoPath);

    connect(
        infoReply,
        &QNetworkReply::finished,
        this,
        [this,
         infoReply,
         artistId,
         loadAdditionalData]() {

            const QByteArray data =
                infoReply
                    ->readAll();

            if (
                infoReply->error() !=
                QNetworkReply::NoError
            ) {

                emit errorOccurred(
                    infoReply->errorString());

                infoReply->deleteLater();

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

                emit errorOccurred(
                    "Некорректный ответ исполнителя");

                infoReply->deleteLater();

                return;
            }

            const QJsonObject root =
                document.object();

            QJsonObject artistObject;

            if (
                root.value("result")
                    .isObject()
            ) {

                artistObject =
                    root.value("result")
                        .toObject();

            } else {

                artistObject =
                    root;
            }

            if (
                artistObject.isEmpty()
            ) {

                emit errorOccurred(
                    "Ответ исполнителя пуст");

                infoReply->deleteLater();

                return;
            }

            ArtistDetails artist;

            /*
             * ID
             */

            const qint64 parsedId =
                artistObject
                    .value("id")
                    .toInteger();

            if (
                parsedId > 0
            ) {

                artist.id =
                    QString::number(
                        parsedId);

            } else {

                artist.id =
                    artistId;
            }

            /*
             * Basic information
             */

            artist.name =
                artistObject
                    .value("name")
                    .toString();

            artist.description =
                artistObject
                    .value("description")
                    .toString();

            /*
             * Cover
             */

            const QJsonObject cover =
                artistObject
                    .value("cover")
                    .toObject();

            artist.coverUri =
                cover
                    .value("uri")
                    .toString();

            if (
                artist.coverUri.isEmpty()
            ) {

                artist.coverUri =
                    artistObject
                        .value("coverUri")
                        .toString();
            }

            if (
                artist.coverUri.isEmpty()
            ) {

                artist.coverUri =
                    artistObject
                        .value("ogImage")
                        .toString();
            }

            /*
             * Genres
             */

            const QJsonArray genres =
                artistObject
                    .value("genres")
                    .toArray();

            for (
                const QJsonValue &value :
                genres
            ) {

                if (
                    value.isString()
                ) {

                    artist.genres.append(
                        value.toString());
                }
            }

            /*
             * Popular tracks
             */

            const QJsonArray popularTracks =
                artistObject
                    .value("popularTracks")
                    .toArray();

            for (
                const QJsonValue &value :
                popularTracks
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

                    artist.tracks.append(
                        track);
                }
            }

            /*
             * We now have basic artist data.
             */

            infoReply
                ->deleteLater();

            /*
             * If popular tracks are not
             * included in brief-info,
             * load them separately first.
             */

            if (
                artist.tracks.isEmpty()
            ) {

                const QString tracksPath =
                    QString(
                        "/artists/%1/tracks")
                        .arg(
                            artistId);

                QNetworkReply *tracksReply =
                    m_yandexClient->get(
                        tracksPath);

                connect(
                    tracksReply,
                    &QNetworkReply::finished,
                    this,
                    [this,
                     tracksReply,
                     artist,
                     loadAdditionalData]() mutable {

                        const QByteArray data =
                            tracksReply
                                ->readAll();

                        if (
                            tracksReply->error() !=
                            QNetworkReply::NoError
                        ) {

                            emit errorOccurred(
                                tracksReply
                                    ->errorString());

                            tracksReply
                                ->deleteLater();

                            return;
                        }

                        QJsonParseError
                            parseError;

                        const QJsonDocument
                            document =
                                QJsonDocument::fromJson(
                                    data,
                                    &parseError);

                        if (
                            parseError.error !=
                                QJsonParseError::NoError ||
                            !document.isObject()
                        ) {

                            emit errorOccurred(
                                "Некорректный ответ треков исполнителя");

                            tracksReply
                                ->deleteLater();

                            return;
                        }

                        const QJsonObject
                            result =
                            resultObject(
                                document);

                        QJsonArray tracks =
                            firstArray(
                                result,
                                {
                                    "tracks",
                                    "popularTracks"
                                });

                        if (
                            tracks.isEmpty()
                        ) {

                            const QJsonObject
                                root =
                                document.object();

                            if (
                                root
                                    .value("tracks")
                                    .isArray()
                            ) {

                                tracks =
                                    root
                                        .value("tracks")
                                        .toArray();
                            }
                        }

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

                                artist.tracks.append(
                                    track);
                            }
                        }

                        restoreArtistName(
                            artist);

                        qDebug()
                            << "Artist tracks loaded:"
                            << artist.name
                            << "| id:"
                            << artist.id
                            << "| tracks:"
                            << artist.tracks.size();

                        tracksReply
                            ->deleteLater();

                        loadAdditionalData(
                            artist);
                    });

                return;
            }

            restoreArtistName(
                artist);

            loadAdditionalData(
                artist);
        });
}