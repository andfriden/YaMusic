#include "AlbumService.h"
#include "../Auth/YandexAuth.h"
#include "../Parsers.h"
#include "../YandexClient.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

AlbumService::AlbumService(
    YandexAuth *auth,
    QObject *parent)
    : YandexServiceBase(auth, parent)
{
}

void AlbumService::loadAlbum(
    const QString &id)
{
    if (
        !ensureAuthenticated()
    ) {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }

    const QString albumId =
        id.trimmed();

    if (
        albumId.isEmpty()
    ) {
        emit errorOccurred(
            "ID альбома не указан");

        return;
    }

    /*
     * L1-кэш: отдаём сразу, если данные ещё свежие.
     */
    AlbumDetails cached;

    if (m_cache.get(albumId, cached)) {
        emit albumReceived(cached);
        return;
    }

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

            const QJsonObject albumObject =
                unwrapResult(document);

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
                parseCoverUri(
                    albumObject);

            albumDetails.album.year =
                albumObject
                    .value("year")
                    .toInt();

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

                const QList<Track> volumeTracks =
                    parseTrackArray(
                        volumeValue.toArray());

                albumDetails.tracks.append(
                    volumeTracks);
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

                const QList<Track> tracks =
                    parseTrackArray(
                        albumObject
                            .value("tracks")
                            .toArray());

                albumDetails.tracks =
                    tracks;
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

            /*
             * Notify listeners
             */

            m_cache.put(albumId, albumDetails);

            emit albumReceived(
                albumDetails);

            reply->deleteLater();
        });
}