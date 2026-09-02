#include "PlaylistService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>


namespace
{

// =============================================================
// Track parser
// =============================================================

Track parseTrack(
    const QJsonObject &object)
{
    Track track;


    track.id =
        object
            .value("id")
            .toString();


    if (
        track.id.isEmpty()
    )
    {
        const qint64 realId =
            object
                .value("realId")
                .toInteger();


        if (
            realId > 0
        )
        {
            track.id =
                QString::number(
                    realId);
        }
    }


    track.title =
        object
            .value("title")
            .toString();


    track.coverUri =
        object
            .value("coverUri")
            .toString();


    track.durationMs =
        object
            .value("durationMs")
            .toInt();


    // =============================================================
    // Artists
    // =============================================================

    const QJsonArray artists =
        object
            .value("artists")
            .toArray();


    for (
        const QJsonValue &value :
        artists
    )
    {
        if (
            !value.isObject()
        )
        {
            continue;
        }


        const QJsonObject artistObject =
            value.toObject();


        Artist artist;


        artist.id =
            QString::number(
                artistObject
                    .value("id")
                    .toInteger());


        artist.name =
            artistObject
                .value("name")
                .toString();


        if (
            !artist.name.isEmpty()
        )
        {
            track.artists.append(
                artist);
        }
    }


    // =============================================================
    // Albums
    // =============================================================

    const QJsonArray albums =
        object
            .value("albums")
            .toArray();


    for (
        const QJsonValue &value :
        albums
    )
    {
        if (
            !value.isObject()
        )
        {
            continue;
        }


        const QJsonObject albumObject =
            value.toObject();


        Album album;


        album.id =
            QString::number(
                albumObject
                    .value("id")
                    .toInteger());


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
            !album.title.isEmpty()
        )
        {
            track.albums.append(
                album);
        }
    }


    return track;
}


// =============================================================
// Playlist parser
// =============================================================

Playlist parsePlaylist(
    const QJsonObject &object)
{
    Playlist playlist;


    playlist.uid =
        QString::number(
            object
                .value("uid")
                .toInteger());


    playlist.kind =
        object
            .value("kind")
            .toInt();


    playlist.title =
        object
            .value("title")
            .toString();


    playlist.description =
        object
            .value("description")
            .toString();


    playlist.trackCount =
        object
            .value("trackCount")
            .toInt();


    const QJsonObject cover =
        object
            .value("cover")
            .toObject();


    playlist.coverUri =
        cover
            .value("uri")
            .toString();


    const QJsonArray tracks =
        object
            .value("tracks")
            .toArray();


    for (
        const QJsonValue &value :
        tracks
    )
    {
        if (
            !value.isObject()
        )
        {
            continue;
        }


        const QJsonObject wrapper =
            value.toObject();


        QJsonObject trackObject =
            wrapper
                .value("track")
                .toObject();


        if (
            trackObject.isEmpty()
        )
        {
            trackObject =
                wrapper;
        }


        if (
            trackObject.isEmpty()
        )
        {
            continue;
        }


        const Track track =
            parseTrack(
                trackObject);


        if (
            !track.id.isEmpty()
        )
        {
            playlist.tracks.append(
                track);
        }
    }


    return playlist;
}


// =============================================================
// Playlist response parser
// =============================================================

bool parsePlaylistResponse(
    const QByteArray &data,
    Playlist &playlist)
{
    QJsonParseError parseError;


    const QJsonDocument document =
        QJsonDocument::fromJson(
            data,
            &parseError);


    if (
        parseError.error !=
            QJsonParseError::NoError ||
        !document.isObject()
    )
    {
        return false;
    }


    const QJsonObject root =
        document.object();


    QJsonObject playlistObject;


    if (
        root
            .value("result")
            .isObject()
    )
    {
        playlistObject =
            root
                .value("result")
                .toObject();
    }
    else
    {
        playlistObject =
            root;
    }


    if (
        playlistObject.isEmpty()
    )
    {
        return false;
    }


    playlist =
        parsePlaylist(
            playlistObject);


    return (
        !playlist.uid.isEmpty() &&
        playlist.kind > 0
    );
}


// =============================================================
// User playlist parser
// =============================================================

PersonalPlaylist parseUserPlaylist(
    const QJsonObject &object)
{
    PersonalPlaylist playlist;


    playlist.id =
        object
            .value("id")
            .toString();


    if (
        playlist.id.isEmpty()
    )
    {
        const qint64 id =
            object
                .value("id")
                .toInteger();


        if (
            id > 0
        )
        {
            playlist.id =
                QString::number(
                    id);
        }
    }


    playlist.uid =
        QString::number(
            object
                .value("uid")
                .toInteger());


    playlist.kind =
        object
            .value("kind")
            .toInt();


    playlist.title =
        object
            .value("title")
            .toString();


    playlist.description =
        object
            .value("description")
            .toString();


    playlist.previewDescription =
        object
            .value("description")
            .toString();


    playlist.trackCount =
        object
            .value("trackCount")
            .toInt();


    playlist.generatedPlaylistType =
        object
            .value("generatedPlaylistType")
            .toString();


    playlist.coverUri =
        object
            .value("cover")
            .toObject()
            .value("uri")
            .toString();


    return playlist;
}

}


// =============================================================
// Constructor
// =============================================================

PlaylistService::PlaylistService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
}


// =============================================================
// Single playlist
// =============================================================

void PlaylistService::loadPlaylist(
    const QString &uid,
    int kind)
{
    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    )
    {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }


    const QString trimmedUid =
        uid.trimmed();


    if (
        trimmedUid.isEmpty()
    )
    {
        emit errorOccurred(
            "UID плейлиста не указан");

        return;
    }


    if (
        kind <= 0
    )
    {
        emit errorOccurred(
            "Идентификатор плейлиста некорректен");

        return;
    }


    m_yandexClient
        ->setToken(
            m_auth->token());


    const QString path =
        QString(
            "/users/%1/playlists/%2")
        .arg(
            trimmedUid)
        .arg(
            kind);


    QNetworkReply *reply =
        m_yandexClient
            ->get(
                path);


    if (
        reply == nullptr
    )
    {
        emit errorOccurred(
            "Не удалось загрузить плейлист");

        return;
    }


    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]()
        {
            const QByteArray data =
                reply->readAll();


            if (
                reply->error() !=
                QNetworkReply::NoError
            )
            {
                emit errorOccurred(
                    reply->errorString());

                reply->deleteLater();

                return;
            }


            Playlist playlist;


            if (
                !parsePlaylistResponse(
                    data,
                    playlist)
            )
            {
                emit errorOccurred(
                    "Некорректный ответ плейлиста");

                reply->deleteLater();

                return;
            }


            emit playlistReceived(
                playlist);


            reply->deleteLater();
        });
}


// =============================================================
// Multiple playlists
// =============================================================

void PlaylistService::loadPlaylists(
    const QList<QPair<QString, int>> &playlists)
{
    m_playlistBatchQueue.clear();

    m_playlistBatchResults.clear();

    m_playlistBatchActive = 0;

    m_playlistBatchCompleted = 0;

    m_playlistBatchError = false;


    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    )
    {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }


    for (
        const QPair<QString, int> &playlist :
        playlists
    )
    {
        const QString uid =
            playlist.first.trimmed();


        const int kind =
            playlist.second;


        if (
            uid.isEmpty() ||
            kind <= 0
        )
        {
            continue;
        }


        m_playlistBatchQueue.append(
            qMakePair(
                uid,
                kind));
    }


    if (
        m_playlistBatchQueue.isEmpty()
    )
    {
        emit playlistsReceived(
            {});

        return;
    }


    m_yandexClient
        ->setToken(
            m_auth->token());


    qDebug()
        << "PlaylistService:"
        << "starting batch:"
        << m_playlistBatchQueue.size();


    startNextPlaylistBatchRequests();
}


// =============================================================
// Start batch requests
// =============================================================

void PlaylistService::startNextPlaylistBatchRequests()
{
    while (
        m_playlistBatchActive <
            MaxConcurrentPlaylistRequests &&
        !m_playlistBatchQueue.isEmpty()
    )
    {
        const QPair<QString, int> reference =
            m_playlistBatchQueue.takeFirst();


        const QString uid =
            reference.first;


        const int kind =
            reference.second;


        const QString path =
            QString(
                "/users/%1/playlists/%2")
        .arg(
            uid)
        .arg(
            kind);


        ++m_playlistBatchActive;


        QNetworkReply *reply =
            m_yandexClient
                ->get(
                    path);


        if (
            reply == nullptr
        )
        {
            --m_playlistBatchActive;

            ++m_playlistBatchCompleted;

            m_playlistBatchError =
                true;

            continue;
        }


        connect(
            reply,
            &QNetworkReply::finished,
            this,
            [this, reply]()
            {
                const QByteArray data =
                    reply->readAll();


                --m_playlistBatchActive;

                ++m_playlistBatchCompleted;


                if (
                    reply->error() !=
                    QNetworkReply::NoError
                )
                {
                    m_playlistBatchError =
                        true;
                }
                else
                {
                    Playlist playlist;


                    if (
                        parsePlaylistResponse(
                            data,
                            playlist)
                    )
                    {
                        m_playlistBatchResults.append(
                            playlist);
                    }
                    else
                    {
                        m_playlistBatchError =
                            true;
                    }
                }


                reply->deleteLater();


                startNextPlaylistBatchRequests();


                if (
                    m_playlistBatchActive == 0 &&
                    m_playlistBatchQueue.isEmpty()
                )
                {
                    finishPlaylistBatch();
                }
            });
    }


    if (
        m_playlistBatchActive == 0 &&
        m_playlistBatchQueue.isEmpty() &&
        m_playlistBatchCompleted > 0
    )
    {
        finishPlaylistBatch();
    }
}


// =============================================================
// Finish batch
// =============================================================

void PlaylistService::finishPlaylistBatch()
{
    qDebug()
        << "PlaylistService:"
        << "batch completed:"
        << m_playlistBatchCompleted
        << "| loaded:"
        << m_playlistBatchResults.size();


    emit playlistsReceived(
        m_playlistBatchResults);


    if (
        m_playlistBatchError
    )
    {
        qDebug()
            << "PlaylistService:"
            << "some playlists failed";
    }


    m_playlistBatchQueue.clear();

    m_playlistBatchResults.clear();

    m_playlistBatchActive = 0;

    m_playlistBatchCompleted = 0;

    m_playlistBatchError = false;
}


// =============================================================
// User playlists
// =============================================================

void PlaylistService::loadUserPlaylists(
    const QString &uid)
{
    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    )
    {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }


    const QString trimmedUid =
        uid.trimmed();


    if (
        trimmedUid.isEmpty()
    )
    {
        emit errorOccurred(
            "UID пользователя не указан");

        return;
    }


    m_yandexClient
        ->setToken(
            m_auth->token());


    const QString path =
        QString(
            "/users/%1/playlists/list")
        .arg(
            trimmedUid);


    QNetworkReply *reply =
        m_yandexClient
            ->get(
                path);


    if (
        reply == nullptr
    )
    {
        emit errorOccurred(
            "Не удалось загрузить список плейлистов");

        return;
    }


    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]()
        {
            const QByteArray data =
                reply->readAll();


            if (
                reply->error() !=
                QNetworkReply::NoError
            )
            {
                emit errorOccurred(
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
                    QJsonParseError::NoError
            )
            {
                emit errorOccurred(
                    "Некорректный ответ списка плейлистов");

                reply->deleteLater();

                return;
            }


            QJsonArray playlistsArray;


            if (
                document.isArray()
            )
            {
                playlistsArray =
                    document.array();
            }
            else if (
                document.isObject()
            )
            {
                const QJsonObject root =
                    document.object();


                if (
                    root
                        .value("result")
                        .isArray()
                )
                {
                    playlistsArray =
                        root
                            .value("result")
                            .toArray();
                }
                else
                {
                    emit errorOccurred(
                        "Список плейлистов отсутствует");

                    reply->deleteLater();

                    return;
                }
            }


            QList<PersonalPlaylist>
                playlists;


            for (
                const QJsonValue &value :
                playlistsArray
            )
            {
                if (
                    !value.isObject()
                )
                {
                    continue;
                }


                const PersonalPlaylist playlist =
                    parseUserPlaylist(
                        value.toObject());


                if (
                    playlist.uid.isEmpty() ||
                    playlist.kind <= 0
                )
                {
                    continue;
                }


                playlists.append(
                    playlist);
            }


            emit userPlaylistsReceived(
                playlists);


            reply->deleteLater();
        });
}