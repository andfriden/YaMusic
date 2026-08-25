#include "ArtistService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QUrlQuery>

#include <memory>

namespace
{

QString jsonId(
    const QJsonObject &object)
{
    const QString stringId =
        object
            .value("id")
            .toString();

    if (!stringId.isEmpty()) {
        return stringId;
    }

    const qint64 numericId =
        object
            .value("id")
            .toInteger();

    if (numericId > 0) {
        return QString::number(
            numericId);
    }

    const qint64 realId =
        object
            .value("realId")
            .toInteger();

    if (realId > 0) {
        return QString::number(
            realId);
    }

    return {};
}


QString coverUriFromObject(
    const QJsonObject &object)
{
    QString coverUri =
        object
            .value("coverUri")
            .toString();

    if (!coverUri.isEmpty()) {
        return coverUri;
    }

    const QJsonObject cover =
        object
            .value("cover")
            .toObject();

    coverUri =
        cover
            .value("uri")
            .toString();

    if (!coverUri.isEmpty()) {
        return coverUri;
    }

    coverUri =
        object
            .value("ogImage")
            .toString();

    return coverUri;
}


Track parseTrack(
    const QJsonObject &object)
{
    Track track;

    QJsonObject trackObject =
        object;

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

    track.id =
        jsonId(
            trackObject);

    track.title =
        trackObject
            .value("title")
            .toString();

    track.coverUri =
        coverUriFromObject(
            trackObject);

    track.durationMs =
        trackObject
            .value("durationMs")
            .toInt();

    /*
     * Artists
     */

    const QJsonArray artists =
        trackObject
            .value("artists")
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

        QJsonObject artistObject =
            value.toObject();

        if (
            artistObject
                .value("artist")
                .isObject()
        ) {
            artistObject =
                artistObject
                    .value("artist")
                    .toObject();
        }

        Artist artist;

        artist.id =
            jsonId(
                artistObject);

        artist.name =
            artistObject
                .value("name")
                .toString();

        artist.coverUri =
            coverUriFromObject(
                artistObject);

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
        trackObject
            .value("albums")
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

        QJsonObject albumObject =
            value.toObject();

        if (
            albumObject
                .value("album")
                .isObject()
        ) {
            albumObject =
                albumObject
                    .value("album")
                    .toObject();
        }

        Album album;

        album.id =
            jsonId(
                albumObject);

        album.title =
            albumObject
                .value("title")
                .toString();

        album.coverUri =
            coverUriFromObject(
                albumObject);

        album.year =
            albumObject
                .value("year")
                .toInt();

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
        albumObject
            .value("album")
            .isObject()
    ) {
        albumObject =
            albumObject
                .value("album")
                .toObject();
    }

    Album album;

    album.id =
        jsonId(
            albumObject);

    album.title =
        albumObject
            .value("title")
            .toString();

    album.coverUri =
        coverUriFromObject(
            albumObject);

    album.year =
        albumObject
            .value("year")
            .toInt();

    return album;
}


Artist parseArtist(
    const QJsonObject &object)
{
    QJsonObject artistObject =
        object;

    if (
        artistObject
            .value("artist")
            .isObject()
    ) {
        artistObject =
            artistObject
                .value("artist")
                .toObject();
    }

    Artist artist;

    artist.id =
        jsonId(
            artistObject);

    artist.name =
        artistObject
            .value("name")
            .toString();

    artist.coverUri =
        coverUriFromObject(
            artistObject);

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

    const QJsonValue result =
        root.value("result");

    if (
        result.isObject()
    ) {
        return result.toObject();
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
            object.value(key);

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
        !artist.name.isEmpty()
    ) {
        return;
    }

    for (
        const Track &track :
        artist.tracks
    ) {
        for (
            const Artist &trackArtist :
            track.artists
        ) {
            if (
                (
                    !artist.id.isEmpty() &&
                    trackArtist.id == artist.id
                ) ||
                (
                    artist.id.isEmpty()
                )
            ) {
                if (
                    !trackArtist.name.isEmpty()
                ) {
                    artist.name =
                        trackArtist.name;

                    return;
                }
            }
        }
    }
}


void restoreArtistArtwork(
    ArtistDetails &artist)
{
    /*
     * Если /brief-info уже дал artwork,
     * ничего менять не нужно.
     */
    if (
        !artist.coverUri.isEmpty()
    ) {
        return;
    }

    /*
     * Самый надёжный fallback:
     *
     * /artists/{id}/brief-info
     *        ↓
     * popularTracks
     *        ↓
     * Track.artists[]
     *        ↓
     * Artist.coverUri
     *
     * В этих данных Yandex уже отдаёт
     * корректный URI artwork исполнителя.
     */
    for (
        const Track &track :
        artist.tracks
    ) {
        for (
            const Artist &trackArtist :
            track.artists
        ) {
            if (
                trackArtist.coverUri.isEmpty()
            ) {
                continue;
            }

            if (
                !artist.id.isEmpty() &&
                !trackArtist.id.isEmpty() &&
                trackArtist.id != artist.id
            ) {
                continue;
            }

            artist.coverUri =
                trackArtist.coverUri;

            qDebug()
                << "Artist artwork restored from track artist:"
                << artist.name
                << "| cover:"
                << artist.coverUri;

            return;
        }
    }

    /*
     * Если ID почему-то не совпал,
     * используем первый доступный artwork.
     */
    for (
        const Track &track :
        artist.tracks
    ) {
        for (
            const Artist &trackArtist :
            track.artists
        ) {
            if (
                !trackArtist.coverUri.isEmpty()
            ) {
                artist.coverUri =
                    trackArtist.coverUri;

                qDebug()
                    << "Artist artwork restored from fallback:"
                    << artist.name
                    << "| cover:"
                    << artist.coverUri;

                return;
            }
        }
    }
}


void restoreSimilarArtistArtwork(
    ArtistDetails &artist)
{
    /*
     * Похожие исполнители уже приходят
     * с корректным coverUri через parseArtist().
     *
     * Здесь только удаляем потенциальные
     * дубликаты и оставляем данные как есть.
     */
    QList<Artist> unique;

    for (
        const Artist &similar :
        artist.similarArtists
    ) {
        if (
            similar.id.isEmpty()
        ) {
            continue;
        }

        bool duplicate =
            false;

        for (
            const Artist &existing :
            unique
        ) {
            if (
                existing.id == similar.id
            ) {
                duplicate = true;
                break;
            }
        }

        if (
            !duplicate
        ) {
            unique.append(
                similar);
        }
    }

    artist.similarArtists =
        unique;
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
     * Финализация дополнительных запросов
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

                    restoreArtistArtwork(
                        *artistData);

                    restoreSimilarArtistArtwork(
                        *artistData);

                    qDebug()
                        << "Artist loaded:"
                        << artistData->name
                        << "| id:"
                        << artistData->id
                        << "| cover:"
                        << artistData->coverUri
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
             * Популярные альбомы
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
                        QJsonParseError parseError;

                        const QJsonDocument document =
                            QJsonDocument::fromJson(
                                data,
                                &parseError);

                        if (
                            parseError.error ==
                                QJsonParseError::NoError &&
                            document.isObject()
                        ) {
                            const QJsonObject result =
                                resultObject(
                                    document);

                            const QJsonArray albums =
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
             * Последний релиз
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
                        QJsonParseError parseError;

                        const QJsonDocument document =
                            QJsonDocument::fromJson(
                                data,
                                &parseError);

                        if (
                            parseError.error ==
                                QJsonParseError::NoError &&
                            document.isObject()
                        ) {
                            const QJsonObject result =
                                resultObject(
                                    document);

                            const QJsonArray albums =
                                firstArray(
                                    result,
                                    {
                                        "albums",
                                        "items"
                                    });

                            if (
                                !albums.isEmpty() &&
                                albums.first().isObject()
                            ) {
                                artistData
                                    ->newRelease =
                                    parseAlbum(
                                        albums
                                            .first()
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
             * Похожие исполнители
             * -------------------------------------------------
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
                        QJsonParseError parseError;

                        const QJsonDocument document =
                            QJsonDocument::fromJson(
                                data,
                                &parseError);

                        if (
                            parseError.error ==
                                QJsonParseError::NoError &&
                            document.isObject()
                        ) {
                            const QJsonObject result =
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
                             * Возможная форма:
                             *
                             * result: {
                             *     similarArtists: {
                             *         artists: [...]
                             *     }
                             * }
                             */

                            if (
                                artists.isEmpty() &&
                                result
                                    .value("similarArtists")
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

                                const Artist similarArtist =
                                    parseArtist(
                                        value.toObject());

                                if (
                                    similarArtist.id.isEmpty() ||
                                    similarArtist.name.isEmpty()
                                ) {
                                    continue;
                                }

                                bool duplicate =
                                    false;

                                for (
                                    const Artist &existing :
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
     * /artists/{id}/brief-info
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
                root
                    .value("result")
                    .isObject()
            ) {
                artistObject =
                    root
                        .value("result")
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

            artist.id =
                jsonId(
                    artistObject);

            if (
                artist.id.isEmpty()
            ) {
                artist.id =
                    artistId;
            }

            /*
             * Основная информация
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
             * Artwork
             *
             * Здесь сначала пытаемся получить
             * обычный cover.uri.
             */

            artist.coverUri =
                coverUriFromObject(
                    artistObject);

            /*
             * Жанры
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
                    const QString genre =
                        value.toString()
                            .trimmed();

                    if (
                        !genre.isEmpty()
                    ) {
                        artist.genres.append(
                            genre);
                    }
                }
            }

            /*
             * Популярные треки из brief-info
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

                const Track track =
                    parseTrack(
                        value.toObject());

                if (
                    !track.id.isEmpty()
                ) {
                    artist.tracks.append(
                        track);
                }
            }

            infoReply
                ->deleteLater();

            /*
             * Если tracks уже пришли,
             * artwork можно восстановить
             * прямо сейчас.
             */

            restoreArtistName(
                artist);

            restoreArtistArtwork(
                artist);

            /*
             * Если brief-info не содержит
             * popularTracks — догружаем их.
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
                                "Некорректный ответ треков исполнителя");

                            tracksReply
                                ->deleteLater();

                            return;
                        }

                        const QJsonObject result =
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
                            const QJsonObject root =
                                document.object();

                            const QJsonValue rootTracks =
                                root.value("tracks");

                            if (
                                rootTracks.isArray()
                            ) {
                                tracks =
                                    rootTracks
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

                            const Track track =
                                parseTrack(
                                    value.toObject());

                            if (
                                !track.id.isEmpty()
                            ) {
                                artist.tracks.append(
                                    track);
                            }
                        }

                        restoreArtistName(
                            artist);

                        restoreArtistArtwork(
                            artist);

                        qDebug()
                            << "Artist tracks loaded:"
                            << artist.name
                            << "| id:"
                            << artist.id
                            << "| cover:"
                            << artist.coverUri
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

            restoreArtistArtwork(
                artist);

            qDebug()
                << "Artist brief-info:"
                << artist.name
                << "| id:"
                << artist.id
                << "| cover:"
                << artist.coverUri
                << "| tracks:"
                << artist.tracks.size();

            loadAdditionalData(
                artist);
        });
}