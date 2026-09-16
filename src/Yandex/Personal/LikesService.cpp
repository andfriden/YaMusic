#include "LikesService.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkReply>
#include <QSet>
#include <QUrlQuery>
#include <memory>
#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

LikesService::LikesService(
    YandexAuth *auth,
    QObject *parent)
    : YandexServiceBase(auth, parent)
{
}

// Load liked tracks

void LikesService::loadLikedTracks(
    const QString &uid)
{
    if (!ensureAuthenticated())
    {
        emit errorOccurred(
            QStringLiteral("Токен Яндекс Музыки не установлен"));

        return;
    }

    if (uid.trimmed().isEmpty())
    {
        emit errorOccurred(
            QStringLiteral("UID пользователя не указан"));

        return;
    }

    if (m_loadingTracks)
        return;

    if (!m_yandexClient->hasToken())
    {
        emit errorOccurred(
            QStringLiteral("Токен Яндекс Музыки не установлен"));

        return;
    }

    m_loadingTracks = true;

    emit loadingChanged(true);

    const QString path =
        QStringLiteral("/users/%1/likes/tracks")
            .arg(uid.trimmed());

    QNetworkReply *reply =
        m_yandexClient->get(path);

    if (reply == nullptr)
    {
        m_loadingTracks = false;
        emit loadingChanged(false);
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

            if (reply->error() !=
                QNetworkReply::NoError)
            {
                const QString error =
                    reply->errorString();

                reply->deleteLater();

                m_loadingTracks = false;

                emit loadingChanged(false);
                emit errorOccurred(error);

                return;
            }

            QJsonParseError parseError;

            const QJsonDocument document =
                QJsonDocument::fromJson(
                    data,
                    &parseError);

            if (parseError.error !=
                QJsonParseError::NoError)
            {
                reply->deleteLater();

                m_loadingTracks = false;

                emit loadingChanged(false);
                emit errorOccurred(
                    QStringLiteral(
                        "Не удалось разобрать список понравившихся треков"));

                return;
            }

            if (!document.isObject())
            {
                reply->deleteLater();

                m_loadingTracks = false;

                emit loadingChanged(false);
                emit errorOccurred(
                    QStringLiteral(
                        "Некорректный ответ списка понравившихся треков"));

                return;
            }

            const QJsonObject root =
                document.object();

            const QJsonObject result =
                root.value(QStringLiteral("result"))
                    .toObject();

            const QJsonObject library =
                result.value(QStringLiteral("library"))
                    .toObject();

            const QJsonArray tracks =
                library.value(QStringLiteral("tracks"))
                    .toArray();

            QStringList trackIds;
            QSet<QString> uniqueIds;

            for (const QJsonValue &value : tracks)
            {
                QString trackId;

                if (value.isObject())
                {
                    const QJsonObject object =
                        value.toObject();

                    const QJsonValue idValue =
                        object.value(QStringLiteral("id"));

                    if (idValue.isString())
                    {
                        trackId =
                            idValue.toString();
                    }
                    else if (idValue.isDouble())
                    {
                        trackId =
                            QString::number(
                                static_cast<qint64>(
                                    idValue.toDouble()));
                    }
                }
                else if (value.isString())
                {
                    trackId =
                        value.toString();
                }
                else if (value.isDouble())
                {
                    trackId =
                        QString::number(
                            static_cast<qint64>(
                                value.toDouble()));
                }

                trackId = trackId.trimmed();

                if (trackId.isEmpty())
                    continue;

                if (uniqueIds.contains(trackId))
                    continue;

                uniqueIds.insert(trackId);
                trackIds.append(trackId);
            }

            /*
             * Replace the cached liked-track state only after
             * the server response has been parsed successfully.
             */
            m_likedTrackIds.clear();

            for (const QString &trackId : trackIds)
                m_likedTrackIds.insert(trackId);

            reply->deleteLater();

            if (trackIds.isEmpty())
            {
                m_loadingTracks = false;

                emit loadingChanged(false);
                emit tracksReceived({});

                return;
            }

            loadTracksByIds(trackIds);
        });
}

// Load liked albums

void LikesService::loadLikedAlbums(
    const QString &uid)
{
    if (!ensureAuthenticated())
    {
        emit errorOccurred("Токен Яндекс Музыки не установлен");
        return;
    }

    const QString userId = uid.trimmed();

    if (userId.isEmpty())
    {
        emit errorOccurred("UID пользователя не указан");
        return;
    }

    m_loadingAlbums = true;

    emit loadingChanged(true);

    const QString path =
        QStringLiteral("/users/%1/likes/albums")
            .arg(userId);

    QNetworkReply *reply =
        m_yandexClient->get(path);

    if (reply == nullptr)
    {
        m_loadingAlbums = false;
        emit loadingChanged(false);
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

            m_loadingAlbums = false;

            emit loadingChanged(false);

            if (reply->error() != QNetworkReply::NoError)
            {
                const QString error = reply->errorString();
                reply->deleteLater();
                emit errorOccurred(error);
                return;
            }

            QJsonParseError parseError;

            const QJsonDocument document =
                QJsonDocument::fromJson(data, &parseError);

            if (parseError.error != QJsonParseError::NoError ||
                !document.isObject())
            {
                reply->deleteLater();
                emit errorOccurred("Некорректный ответ альбомов");
                return;
            }

            QList<Album> albums;

            const QJsonObject root = document.object();
            const QJsonObject result = root.value("result").toObject();
            const QJsonObject library = result.value("library").toObject();
            const QJsonArray albumData = library.value("albums").toArray();

            m_likedAlbumIds.clear();

            albums.reserve(albumData.size());

            for (const QJsonValue &value : albumData)
            {
                if (!value.isObject())
                    continue;

                const QJsonObject obj = value.toObject();

                Album album;

                const QJsonValue idValue = obj.value("id");

                if (idValue.isString())
                    album.id = idValue.toString();
                else if (idValue.isDouble())
                    album.id = QString::number(static_cast<qint64>(idValue.toDouble()));

                if (!album.id.isEmpty())
                    m_likedAlbumIds.insert(album.id);

                album.title = obj.value("title").toString();

                const QJsonValue coverValue = obj.value("coverUri");
                album.coverUri = coverValue.toString();

                if (album.coverUri.isEmpty())
                {
                    const QJsonObject cover = obj.value("cover").toObject();
                    album.coverUri = cover.value("uri").toString();
                }

                album.year = obj.value("year").toInt();

                if (!album.id.isEmpty() || !album.title.isEmpty())
                    albums.append(album);
            }

            reply->deleteLater();

            emit albumsReceived(albums);
        });
}

// Load liked artists

void LikesService::loadLikedArtists(
    const QString &uid)
{
    if (!ensureAuthenticated())
    {
        emit errorOccurred("Токен Яндекс Музыки не установлен");
        return;
    }

    const QString userId = uid.trimmed();

    if (userId.isEmpty())
    {
        emit errorOccurred("UID пользователя не указан");
        return;
    }

    m_loadingArtists = true;

    emit loadingChanged(true);

    const QString path =
        QStringLiteral("/users/%1/likes/artists")
            .arg(userId);

    QNetworkReply *reply =
        m_yandexClient->get(path);

    if (reply == nullptr)
    {
        m_loadingArtists = false;
        emit loadingChanged(false);
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

            m_loadingArtists = false;

            emit loadingChanged(false);

            if (reply->error() != QNetworkReply::NoError)
            {
                const QString error = reply->errorString();
                reply->deleteLater();
                emit errorOccurred(error);
                return;
            }

            QJsonParseError parseError;

            const QJsonDocument document =
                QJsonDocument::fromJson(data, &parseError);

            if (parseError.error != QJsonParseError::NoError ||
                !document.isObject())
            {
                reply->deleteLater();
                emit errorOccurred("Некорректный ответ исполнителей");
                return;
            }

            QList<Artist> artists;

            const QJsonObject root = document.object();
            const QJsonObject result = root.value("result").toObject();
            const QJsonObject library = result.value("library").toObject();
            const QJsonArray artistData = library.value("artists").toArray();

            m_likedArtistIds.clear();

            artists.reserve(artistData.size());

            for (const QJsonValue &value : artistData)
            {
                if (!value.isObject())
                    continue;

                const QJsonObject obj = value.toObject();

                Artist artist;

                const QJsonValue idValue = obj.value("id");

                if (idValue.isString())
                    artist.id = idValue.toString();
                else if (idValue.isDouble())
                    artist.id = QString::number(static_cast<qint64>(idValue.toDouble()));

                if (!artist.id.isEmpty())
                    m_likedArtistIds.insert(artist.id);

                artist.name = obj.value("name").toString();

                const QJsonValue coverValue = obj.value("coverUri");
                artist.coverUri = coverValue.toString();

                if (artist.coverUri.isEmpty())
                {
                    const QJsonObject cover = obj.value("cover").toObject();
                    artist.coverUri = cover.value("uri").toString();
                }

                if (!artist.id.isEmpty() || !artist.name.isEmpty())
                    artists.append(artist);
            }

            reply->deleteLater();

            emit artistsReceived(artists);
        });
}

// Load full track data by IDs

void LikesService::loadTracksByIds(
    const QStringList &trackIds)
{
    if (m_yandexClient == nullptr)
    {
        m_loadingTracks = false;

        emit loadingChanged(false);
        emit errorOccurred(
            QStringLiteral("YandexClient недоступен"));

        return;
    }

    QNetworkReply *reply =
        nullptr;

    /*
     * getTracks() emits either tracksReceived() or requestError().
     * Use one-shot connections because every load creates a new
     * request.
     */

    auto tracksConnection =
        std::make_shared<QMetaObject::Connection>();

    *tracksConnection =
        connect(
            m_yandexClient,
            &YandexClient::tracksReceived,
            this,
            [this, tracksConnection](
                const QList<Track> &tracks)
            {
                disconnect(*tracksConnection);

                QList<Track> likedTracks =
                    tracks;

                for (Track &track : likedTracks)
                    track.liked = true;

                m_loadingTracks = false;

                emit loadingChanged(false);
                emit tracksReceived(likedTracks);
            },
            Qt::SingleShotConnection);

    auto errorConnection =
        std::make_shared<QMetaObject::Connection>();

    *errorConnection =
        connect(
            m_yandexClient,
            &YandexClient::requestError,
            this,
            [this, errorConnection](
                const QString &message)
            {
                disconnect(*errorConnection);

                m_loadingTracks = false;

                emit loadingChanged(false);
                emit errorOccurred(message);
            },
            Qt::SingleShotConnection);

    Q_UNUSED(reply);

    m_yandexClient->getTracks(trackIds);
}

// Add like

void LikesService::addLike(
    const QString &uid,
    const QString &trackId)
{
    changeLike(
        uid,
        trackId,
        true);
}

// Remove like

void LikesService::removeLike(
    const QString &uid,
    const QString &trackId)
{
    changeLike(
        uid,
        trackId,
        false);
}

// Like state

bool LikesService::isLiked(
    const QString &trackId) const
{
    const QString id =
        trackId.trimmed();

    if (id.isEmpty())
        return false;

    return m_likedTrackIds.contains(id);
}

// Change like

void LikesService::changeLike(
    const QString &uid,
    const QString &trackId,
    bool liked)
{
    if (!ensureAuthenticated())
    {
        emit errorOccurred(
            QStringLiteral("Токен Яндекс Музыки не установлен"));

        return;
    }

    const QString userId =
        uid.trimmed();

    const QString id =
        trackId.trimmed();

    if (userId.isEmpty())
    {
        emit errorOccurred(
            QStringLiteral("UID пользователя не указан"));

        return;
    }

    if (id.isEmpty())
    {
        emit errorOccurred(
            QStringLiteral("ID трека не указан"));

        return;
    }

    if (m_yandexClient == nullptr)
    {
        emit errorOccurred(
            QStringLiteral("YandexClient недоступен"));

        return;
    }

    if (!m_yandexClient->hasToken())
    {
        emit errorOccurred(
            QStringLiteral("Токен Яндекс Музыки не установлен"));

        return;
    }

    const QString path =
        liked
            ? QStringLiteral(
                  "/users/%1/likes/tracks/add-multiple")
                  .arg(userId)
            : QStringLiteral(
                  "/users/%1/likes/tracks/remove")
                  .arg(userId);

    QUrlQuery body;

    body.addQueryItem(
        QStringLiteral("track-ids"),
        id);

    QNetworkReply *reply =
        m_yandexClient->postForm(
            path,
            body);

    if (reply == nullptr)
        return;

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, id, liked]()
        {
            const QByteArray data =
                reply->readAll();

            Q_UNUSED(data);

            if (reply->error() !=
                QNetworkReply::NoError)
            {
                const QString error =
                    reply->errorString();

                reply->deleteLater();

                emit errorOccurred(error);

                return;
            }

            if (liked)
                m_likedTrackIds.insert(id);
            else
                m_likedTrackIds.remove(id);

            reply->deleteLater();

            emit likeChanged(
                id,
                liked);
        });
}

// Album like

void LikesService::addAlbumLike(
    const QString &uid,
    const QString &albumId)
{
    const QString userId = uid.trimmed();
    const QString id = albumId.trimmed();

    if (userId.isEmpty() || id.isEmpty())
        return;

    if (m_yandexClient == nullptr || !m_yandexClient->hasToken())
        return;

    const QString path =
        QStringLiteral("/users/%1/likes/albums/add-multiple")
            .arg(userId);

    QUrlQuery body;
    body.addQueryItem(QStringLiteral("album-ids"), id);

    QNetworkReply *reply = m_yandexClient->postForm(path, body);

    if (reply == nullptr)
        return;

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, id]()
        {
            Q_UNUSED(reply->readAll());

            if (reply->error() != QNetworkReply::NoError)
            {
                reply->deleteLater();
                return;
            }

            m_likedAlbumIds.insert(id);
            reply->deleteLater();
            emit albumLikeChanged(id, true);
        });
}

void LikesService::removeAlbumLike(
    const QString &uid,
    const QString &albumId)
{
    const QString userId = uid.trimmed();
    const QString id = albumId.trimmed();

    if (userId.isEmpty() || id.isEmpty())
        return;

    if (m_yandexClient == nullptr || !m_yandexClient->hasToken())
        return;

    const QString path =
        QStringLiteral("/users/%1/likes/albums/remove")
            .arg(userId);

    QUrlQuery body;
    body.addQueryItem(QStringLiteral("album-ids"), id);

    QNetworkReply *reply = m_yandexClient->postForm(path, body);

    if (reply == nullptr)
        return;

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, id]()
        {
            Q_UNUSED(reply->readAll());

            if (reply->error() != QNetworkReply::NoError)
            {
                reply->deleteLater();
                return;
            }

            m_likedAlbumIds.remove(id);
            reply->deleteLater();
            emit albumLikeChanged(id, false);
        });
}

// Artist like

void LikesService::addArtistLike(
    const QString &uid,
    const QString &artistId)
{
    const QString userId = uid.trimmed();
    const QString id = artistId.trimmed();

    if (userId.isEmpty() || id.isEmpty())
        return;

    if (m_yandexClient == nullptr || !m_yandexClient->hasToken())
        return;

    const QString path =
        QStringLiteral("/users/%1/likes/artists/add-multiple")
            .arg(userId);

    QUrlQuery body;
    body.addQueryItem(QStringLiteral("artist-ids"), id);

    QNetworkReply *reply = m_yandexClient->postForm(path, body);

    if (reply == nullptr)
        return;

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, id]()
        {
            Q_UNUSED(reply->readAll());

            if (reply->error() != QNetworkReply::NoError)
            {
                reply->deleteLater();
                return;
            }

            m_likedArtistIds.insert(id);
            reply->deleteLater();
            emit artistLikeChanged(id, true);
        });
}

void LikesService::removeArtistLike(
    const QString &uid,
    const QString &artistId)
{
    const QString userId = uid.trimmed();
    const QString id = artistId.trimmed();

    if (userId.isEmpty() || id.isEmpty())
        return;

    if (m_yandexClient == nullptr || !m_yandexClient->hasToken())
        return;

    const QString path =
        QStringLiteral("/users/%1/likes/artists/remove")
            .arg(userId);

    QUrlQuery body;
    body.addQueryItem(QStringLiteral("artist-ids"), id);

    QNetworkReply *reply = m_yandexClient->postForm(path, body);

    if (reply == nullptr)
        return;

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, id]()
        {
            Q_UNUSED(reply->readAll());

            if (reply->error() != QNetworkReply::NoError)
            {
                reply->deleteLater();
                return;
            }

            m_likedArtistIds.remove(id);
            reply->deleteLater();
            emit artistLikeChanged(id, false);
        });
}

// Like state queries

bool LikesService::isAlbumLiked(
    const QString &albumId) const
{
    const QString id = albumId.trimmed();
    return !id.isEmpty() && m_likedAlbumIds.contains(id);
}

bool LikesService::isArtistLiked(
    const QString &artistId) const
{
    const QString id = artistId.trimmed();
    return !id.isEmpty() && m_likedArtistIds.contains(id);
}