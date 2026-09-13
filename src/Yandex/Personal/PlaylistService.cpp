#include "PlaylistService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>


namespace
{

// =============================================================
// Track parser
// =============================================================

Track parseTrack(const QJsonObject &object)
{
    Track track;

    track.id =
        object.value("id").toString();

    if (track.id.isEmpty()) {

        const qint64 realId =
            object.value("realId").toInteger();

        if (realId > 0) {
            track.id =
                QString::number(realId);
        }
    }

    track.title =
        object.value("title").toString();

    track.coverUri =
        object.value("coverUri").toString();

    track.durationMs =
        object.value("durationMs").toInt();


    const QJsonArray artists =
        object.value("artists").toArray();

    for (const QJsonValue &value : artists) {

        if (!value.isObject()) {
            continue;
        }

        const QJsonObject artistObject =
            value.toObject();

        Artist artist;

        artist.id =
            QString::number(
                artistObject.value("id").toInteger());

        artist.name =
            artistObject.value("name").toString();

        if (!artist.name.isEmpty()) {
            track.artists.append(artist);
        }
    }


    const QJsonArray albums =
        object.value("albums").toArray();

    for (const QJsonValue &value : albums) {

        if (!value.isObject()) {
            continue;
        }

        const QJsonObject albumObject =
            value.toObject();

        Album album;

        album.id =
            QString::number(
                albumObject.value("id").toInteger());

        album.title =
            albumObject.value("title").toString();

        album.coverUri =
            albumObject.value("coverUri").toString();

        album.year =
            albumObject.value("year").toInt();

        if (!album.title.isEmpty()) {
            track.albums.append(album);
        }
    }

    return track;
}


// =============================================================
// Playlist parser
// =============================================================

Playlist parsePlaylist(const QJsonObject &object)
{
    Playlist playlist;

    playlist.uid =
        QString::number(
            object.value("uid").toInteger());

    playlist.kind =
        object.value("kind").toInt();

    playlist.uuid =
        object.value("playlistUuid").toString();

    playlist.title =
        object.value("title").toString();

    playlist.description =
        object.value("description").toString();

    playlist.trackCount =
        object.value("trackCount").toInt();


    const QJsonObject cover =
        object.value("cover").toObject();

    playlist.coverUri =
        cover.value("uri").toString();


    const QJsonArray tracks =
        object.value("tracks").toArray();

    for (const QJsonValue &value : tracks) {

        if (!value.isObject()) {
            continue;
        }

        const QJsonObject wrapper =
            value.toObject();

        QJsonObject trackObject =
            wrapper.value("track").toObject();

        if (trackObject.isEmpty()) {
            trackObject =
                wrapper;
        }

        if (trackObject.isEmpty()) {
            continue;
        }

        const Track track =
            parseTrack(trackObject);

        if (!track.id.isEmpty()) {
            playlist.tracks.append(track);
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
    ) {
        return false;
    }


    const QJsonObject root =
        document.object();

    QJsonObject playlistObject;


    if (root.value("result").isObject()) {

        playlistObject =
            root.value("result").toObject();

    } else {

        playlistObject =
            root;
    }


    if (playlistObject.isEmpty()) {
        return false;
    }


    playlist =
        parsePlaylist(playlistObject);


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
        object.value("id").toString();

    if (playlist.id.isEmpty()) {

        const qint64 id =
            object.value("id").toInteger();

        if (id > 0) {
            playlist.id =
                QString::number(id);
        }
    }


    playlist.uid =
        QString::number(
            object.value("uid").toInteger());

    playlist.kind =
        object.value("kind").toInt();

    playlist.title =
        object.value("title").toString();

    playlist.description =
        object.value("description").toString();

    playlist.previewDescription =
        object.value("description").toString();

    playlist.trackCount =
        object.value("trackCount").toInt();

    playlist.generatedPlaylistType =
        object.value(
            "generatedPlaylistType")
            .toString();

    playlist.coverUri =
        object
            .value("cover")
            .toObject()
            .value("uri")
            .toString();


    return playlist;
}


// =============================================================
// Similar playlist parser
// =============================================================

Playlist parseSimilarPlaylist(
    const QJsonObject &object)
{
    Playlist playlist;

    playlist.uid =
        QString::number(
            object.value("uid").toInteger());

    playlist.kind =
        object.value("kind").toInt();

    playlist.uuid =
        object.value("playlistUuid").toString();

    playlist.title =
        object.value("title").toString();

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
// PlaylistService
// =============================================================

PlaylistService::PlaylistService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(new YandexClient(this))
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
    ) {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");
        return;
    }


    const QString trimmedUid =
        uid.trimmed();

    if (trimmedUid.isEmpty()) {
        emit errorOccurred(
            "UID плейлиста не указан");
        return;
    }


    if (kind <= 0) {
        emit errorOccurred(
            "Идентификатор плейлиста некорректен");
        return;
    }


    m_yandexClient->setToken(
        m_auth->token());


    const QString path =
        QString(
            "/users/%1/playlists/%2")
            .arg(trimmedUid)
            .arg(kind);


    QNetworkReply *reply =
        m_yandexClient->get(path);


    if (reply == nullptr) {
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
            ) {
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
            ) {
                emit errorOccurred(
                    "Некорректный ответ плейлиста");

                reply->deleteLater();
                return;
            }


            if (!playlist.uuid.isEmpty()) {
                loadSimilarPlaylists(
                    playlist.uuid);
            }


            emit playlistReceived(
                playlist);


            reply->deleteLater();
        });
}


// =============================================================
// Playlist batch
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
    ) {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");
        return;
    }


    for (
        const QPair<QString, int> &playlist :
        playlists
    ) {
        const QString uid =
            playlist.first.trimmed();

        const int kind =
            playlist.second;


        if (
            uid.isEmpty() ||
            kind <= 0
        ) {
            continue;
        }


        m_playlistBatchQueue.append(
            qMakePair(uid, kind));
    }


    if (m_playlistBatchQueue.isEmpty()) {

        emit playlistsReceived({});

        return;
    }


    m_yandexClient->setToken(
        m_auth->token());


    startNextPlaylistBatchRequests();
}


void PlaylistService::startNextPlaylistBatchRequests()
{
    while (
        m_playlistBatchActive <
            MaxConcurrentPlaylistRequests &&
        !m_playlistBatchQueue.isEmpty()
    ) {

        const QPair<QString, int> playlist =
            m_playlistBatchQueue.takeFirst();

        const QString uid =
            playlist.first;

        const int kind =
            playlist.second;


        const QString path =
            QString(
                "/users/%1/playlists/%2")
                .arg(uid)
                .arg(kind);


        QNetworkReply *reply =
            m_yandexClient->get(path);


        ++m_playlistBatchActive;


        if (reply == nullptr) {

            --m_playlistBatchActive;
            ++m_playlistBatchCompleted;

            m_playlistBatchError = true;

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
                ) {

                    m_playlistBatchError = true;

                } else {

                    Playlist playlist;


                    if (
                        parsePlaylistResponse(
                            data,
                            playlist)
                    ) {

                        m_playlistBatchResults.append(
                            playlist);

                    } else {

                        m_playlistBatchError = true;
                    }
                }


                reply->deleteLater();


                startNextPlaylistBatchRequests();


                if (
                    m_playlistBatchActive == 0 &&
                    m_playlistBatchQueue.isEmpty()
                ) {
                    finishPlaylistBatch();
                }
            });
    }


    if (
        m_playlistBatchActive == 0 &&
        m_playlistBatchQueue.isEmpty() &&
        m_playlistBatchCompleted > 0
    ) {
        finishPlaylistBatch();
    }
}


void PlaylistService::finishPlaylistBatch()
{
    const QList<Playlist> results =
        m_playlistBatchResults;

    m_playlistBatchQueue.clear();
    m_playlistBatchResults.clear();
    m_playlistBatchActive = 0;
    m_playlistBatchCompleted = 0;
    m_playlistBatchError = false;


    emit playlistsReceived(
        results);
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
    ) {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");
        return;
    }


    const QString trimmedUid =
        uid.trimmed();


    if (trimmedUid.isEmpty()) {
        emit errorOccurred(
            "UID пользователя не указан");
        return;
    }


    m_yandexClient->setToken(
        m_auth->token());


    const QString path =
        QString(
            "/users/%1/playlists/list")
            .arg(trimmedUid);


    QNetworkReply *reply =
        m_yandexClient->get(path);


    if (reply == nullptr) {
        emit errorOccurred(
            "Не удалось загрузить плейлисты пользователя");
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
            ) {
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
            ) {
                emit errorOccurred(
                    "Некорректный ответ плейлистов пользователя");

                reply->deleteLater();
                return;
            }


            QJsonArray playlistArray;


            if (document.isArray()) {

                playlistArray =
                    document.array();

            } else if (document.isObject()) {

                const QJsonObject root =
                    document.object();

                if (
                    root.value("result").isArray()
                ) {
                    playlistArray =
                        root.value("result")
                            .toArray();
                }
            }


            if (
                playlistArray.isEmpty() &&
                !document.isArray()
            ) {
                emit errorOccurred(
                    "Некорректная структура плейлистов пользователя");

                reply->deleteLater();
                return;
            }


            QList<PersonalPlaylist> playlists;


            for (
                const QJsonValue &value :
                playlistArray
            ) {

                if (!value.isObject()) {
                    continue;
                }


                const PersonalPlaylist playlist =
                    parseUserPlaylist(
                        value.toObject());


                if (
                    playlist.uid.isEmpty() ||
                    playlist.kind <= 0
                ) {
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


// =============================================================
// Similar playlists
// =============================================================

void PlaylistService::loadSimilarPlaylists(
    const QString &uuid)
{
    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    ) {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");
        return;
    }


    const QString trimmedUuid =
        uuid.trimmed();

    if (trimmedUuid.isEmpty()) {
        return;
    }


    m_yandexClient->setToken(
        m_auth->token());


    const QString path =
        QString(
            "/playlist/%1/similar-entities")
            .arg(trimmedUuid);


    QNetworkReply *reply =
        m_yandexClient->get(path);


    if (reply == nullptr) {
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
            ) {
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
                reply->deleteLater();
                return;
            }


            const QJsonObject root =
                document.object();

            const QJsonObject result =
                root.value("result").toObject();

            const QJsonArray items =
                result.value("items").toArray();


            QList<Playlist> playlists;


            for (
                const QJsonValue &value :
                items
            ) {
                if (!value.isObject()) {
                    continue;
                }


                const QJsonObject item =
                    value.toObject();


                if (
                    item.value("type").toString() !=
                    "playlist_item"
                ) {
                    continue;
                }


                const QJsonObject playlistObject =
                    item
                        .value("data")
                        .toObject()
                        .value("playlist")
                        .toObject();


                if (playlistObject.isEmpty()) {
                    continue;
                }


                const Playlist playlist =
                    parseSimilarPlaylist(
                        playlistObject);


                if (
                    playlist.uid.isEmpty() ||
                    playlist.uuid.isEmpty() ||
                    playlist.kind <= 0 ||
                    playlist.title.isEmpty()
                ) {
                    continue;
                }


                playlists.append(
                    playlist);
            }


            emit similarPlaylistsReceived(
                playlists);


            reply->deleteLater();
        });
}