#include "PlaylistService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

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

    track.id =
        object.value("id")
            .toString();

    if (track.id.isEmpty()) {

        const qint64 realId =
            object.value("realId")
                .toInteger();

        if (realId > 0) {
            track.id =
                QString::number(realId);
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

    const QJsonArray artists =
        object.value("artists")
            .toArray();

    for (const QJsonValue &value :
         artists) {

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

        if (!artist.name.isEmpty()) {

            track.artists.append(
                artist);
        }
    }

    const QJsonArray albums =
        object.value("albums")
            .toArray();

    for (const QJsonValue &value :
         albums) {

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

        if (!album.title.isEmpty()) {

            track.albums.append(
                album);
        }
    }

    return track;
}
}

PlaylistService::PlaylistService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
}

void PlaylistService::loadPlaylist(
    const QString &uid,
    int kind)
{
    if (m_auth == nullptr ||
        !m_auth->isAuthenticated()) {

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

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]() {

            const QByteArray data =
                reply->readAll();

            if (reply->error() !=
                QNetworkReply::NoError) {

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

            if (parseError.error !=
                    QJsonParseError::NoError ||
                !document.isObject()) {

                emit errorOccurred(
                    "Некорректный ответ плейлиста");

                reply->deleteLater();

                return;
            }

            const QJsonObject root =
                document.object();

            QJsonObject playlistObject;

            if (root.value("result").isObject()) {

                playlistObject =
                    root.value("result")
                        .toObject();

            } else {

                playlistObject =
                    root;
            }

            if (playlistObject.isEmpty()) {

                emit errorOccurred(
                    "Ответ плейлиста пуст");

                reply->deleteLater();

                return;
            }

            Playlist playlist;

            playlist.uid =
                QString::number(
                    playlistObject
                        .value("uid")
                        .toInteger());

            playlist.kind =
                playlistObject
                    .value("kind")
                    .toInt();

            playlist.title =
                playlistObject
                    .value("title")
                    .toString();

            playlist.description =
                playlistObject
                    .value("description")
                    .toString();

            playlist.trackCount =
                playlistObject
                    .value("trackCount")
                    .toInt();

            const QJsonObject cover =
                playlistObject
                    .value("cover")
                    .toObject();

            playlist.coverUri =
                cover.value("uri")
                    .toString();

            const QJsonArray tracks =
                playlistObject
                    .value("tracks")
                    .toArray();

            for (const QJsonValue &value :
                 tracks) {

                const QJsonObject trackWrapper =
                    value.toObject();

                QJsonObject trackObject =
                    trackWrapper
                        .value("track")
                        .toObject();

                if (trackObject.isEmpty()) {

                    trackObject =
                        trackWrapper;
                }

                if (trackObject.isEmpty()) {
                    continue;
                }

                const Track track =
                    parseTrack(
                        trackObject);

                if (!track.id.isEmpty()) {

                    playlist.tracks.append(
                        track);
                }
            }

            emit playlistReceived(
                playlist);

            reply->deleteLater();
        });
}