#include "ArtistService.h"

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

    track.id =
        object.value("id")
            .toString();

    if (track.id.isEmpty()) {

        const qint64 realId =
            object.value("realId")
                .toInteger();

        if (realId > 0) {

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

    const QJsonArray artists =
        object.value("artists")
            .toArray();

    for (const QJsonValue &value :
         artists) {

        if (!value.isObject()) {
            continue;
        }

        const QJsonObject artistObject =
            value.toObject();

        Artist artist;

        const qint64 artistId =
            artistObject
                .value("id")
                .toInteger();

        if (artistId > 0) {

            artist.id =
                QString::number(
                    artistId);
        }

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

        if (!value.isObject()) {
            continue;
        }

        const QJsonObject albumObject =
            value.toObject();

        Album album;

        const qint64 albumId =
            albumObject
                .value("id")
                .toInteger();

        if (albumId > 0) {

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

        if (!album.title.isEmpty()) {

            track.albums.append(
                album);
        }
    }

    return track;
}

void restoreArtistName(
    ArtistDetails &artist)
{
    if (!artist.name.isEmpty() ||
        artist.tracks.isEmpty()) {

        return;
    }

    const Track &firstTrack =
        artist.tracks.first();

    if (firstTrack.artists.isEmpty()) {
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
    if (m_auth == nullptr ||
        !m_auth->isAuthenticated()) {

        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }

    const QString artistId =
        id.trimmed();

    if (artistId.isEmpty()) {

        emit errorOccurred(
            "ID исполнителя не указан");

        return;
    }

    m_yandexClient->setToken(
        m_auth->token());

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
         artistId]() {

            const QByteArray data =
                infoReply->readAll();

            if (infoReply->error() !=
                QNetworkReply::NoError) {

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

            if (artistObject.isEmpty()) {

                emit errorOccurred(
                    "Ответ исполнителя пуст");

                infoReply->deleteLater();

                return;
            }

            ArtistDetails artist;

            const qint64 parsedId =
                artistObject
                    .value("id")
                    .toInteger();

            if (parsedId > 0) {

                artist.id =
                    QString::number(
                        parsedId);

            } else {

                artist.id =
                    artistId;
            }

            artist.name =
                artistObject
                    .value("name")
                    .toString();

            artist.description =
                artistObject
                    .value("description")
                    .toString();

            const QJsonObject cover =
                artistObject
                    .value("cover")
                    .toObject();

            artist.coverUri =
                cover
                    .value("uri")
                    .toString();

            if (artist.coverUri.isEmpty()) {

                artist.coverUri =
                    artistObject
                        .value("ogImage")
                        .toString();
            }

            const QJsonArray genres =
                artistObject
                    .value("genres")
                    .toArray();

            for (const QJsonValue &value :
                 genres) {

                if (!value.isString()) {
                    continue;
                }

                artist.genres.append(
                    value.toString());
            }

            /*
             * В некоторых ответах
             * popularTracks уже находятся
             * внутри brief-info.
             */

            const QJsonArray popularTracks =
                artistObject
                    .value("popularTracks")
                    .toArray();

            for (const QJsonValue &value :
                 popularTracks) {

                if (!value.isObject()) {
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

                if (trackObject.isEmpty()) {
                    continue;
                }

                const Track track =
                    parseTrack(
                        trackObject);

                if (!track.id.isEmpty()) {

                    artist.tracks.append(
                        track);
                }
            }

            infoReply->deleteLater();

            /*
             * Если popularTracks уже
             * присутствуют, завершаем здесь.
             */

            if (!artist.tracks.isEmpty()) {

                restoreArtistName(
                    artist);

                qDebug()
                    << "Artist loaded:"
                    << artist.name
                    << "| id:"
                    << artist.id
                    << "| tracks:"
                    << artist.tracks.size();

                emit artistReceived(
                    artist);

                return;
            }

            /*
             * Иначе загружаем треки
             * отдельным запросом.
             */

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
                 artist]() mutable {

                    const QByteArray tracksData =
                        tracksReply->readAll();

                    if (
                        tracksReply->error() !=
                        QNetworkReply::NoError
                    ) {

                        emit errorOccurred(
                            tracksReply
                                ->errorString());

                        tracksReply->deleteLater();

                        return;
                    }

                    QJsonParseError
                        tracksParseError;

                    const QJsonDocument
                        tracksDocument =
                            QJsonDocument::fromJson(
                                tracksData,
                                &tracksParseError);

                    if (
                        tracksParseError.error !=
                            QJsonParseError::NoError ||
                        !tracksDocument.isObject()
                    ) {

                        emit errorOccurred(
                            "Некорректный ответ треков исполнителя");

                        tracksReply->deleteLater();

                        return;
                    }

                    const QJsonObject root =
                        tracksDocument.object();

                    QJsonArray tracksArray;

                    /*
                     * Основной вариант:
                     *
                     * result.tracks
                     */

                    if (
                        root.value("result")
                            .isObject()
                    ) {

                        const QJsonObject result =
                            root.value("result")
                                .toObject();

                        tracksArray =
                            result.value("tracks")
                                .toArray();

                    } else {

                        /*
                         * Fallback:
                         *
                         * tracks
                         */

                        tracksArray =
                            root.value("tracks")
                                .toArray();
                    }

                    for (
                        const QJsonValue &value :
                        tracksArray) {

                        if (!value.isObject()) {
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

                        if (trackObject.isEmpty()) {
                            continue;
                        }

                        const Track track =
                            parseTrack(
                                trackObject);

                        if (!track.id.isEmpty()) {

                            artist.tracks.append(
                                track);
                        }
                    }

                    restoreArtistName(
                        artist);

                    qDebug()
                        << "Artist loaded:"
                        << artist.name
                        << "| id:"
                        << artist.id
                        << "| tracks:"
                        << artist.tracks.size();

                    emit artistReceived(
                        artist);

                    tracksReply->deleteLater();
                });
        });
}