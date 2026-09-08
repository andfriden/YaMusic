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
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(new YandexClient(this))
{
}


// =============================================================
// Load liked tracks
// =============================================================

void LikesService::loadLikedTracks(
    const QString &uid)
{
    if (m_auth == nullptr)
    {
        emit errorOccurred(
            QStringLiteral("Сервис авторизации недоступен"));

        return;
    }

    if (uid.trimmed().isEmpty())
    {
        emit errorOccurred(
            QStringLiteral("UID пользователя не указан"));

        return;
    }

    if (m_loading)
        return;

    m_yandexClient->setToken(
        m_auth->token());

    if (!m_yandexClient->hasToken())
    {
        emit errorOccurred(
            QStringLiteral("Токен Яндекс Музыки не установлен"));

        return;
    }

    m_loading = true;

    emit loadingChanged(true);

    const QString path =
        QStringLiteral("/users/%1/likes/tracks")
            .arg(uid.trimmed());

    QNetworkReply *reply =
        m_yandexClient->get(path);

    if (reply == nullptr)
    {
        m_loading = false;
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

                m_loading = false;

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

                m_loading = false;

                emit loadingChanged(false);
                emit errorOccurred(
                    QStringLiteral(
                        "Не удалось разобрать список понравившихся треков"));

                return;
            }

            if (!document.isObject())
            {
                reply->deleteLater();

                m_loading = false;

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
                m_loading = false;

                emit loadingChanged(false);
                emit tracksReceived({});

                return;
            }

            loadTracksByIds(trackIds);
        });
}


// =============================================================
// Load full track data by IDs
// =============================================================

void LikesService::loadTracksByIds(
    const QStringList &trackIds)
{
    if (m_yandexClient == nullptr)
    {
        m_loading = false;

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

                m_loading = false;

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

                m_loading = false;

                emit loadingChanged(false);
                emit errorOccurred(message);
            },
            Qt::SingleShotConnection);

    Q_UNUSED(reply);

    m_yandexClient->getTracks(trackIds);
}


// =============================================================
// Add like
// =============================================================

void LikesService::addLike(
    const QString &uid,
    const QString &trackId)
{
    changeLike(
        uid,
        trackId,
        true);
}


// =============================================================
// Remove like
// =============================================================

void LikesService::removeLike(
    const QString &uid,
    const QString &trackId)
{
    changeLike(
        uid,
        trackId,
        false);
}


// =============================================================
// Like state
// =============================================================

bool LikesService::isLiked(
    const QString &trackId) const
{
    const QString id =
        trackId.trimmed();

    if (id.isEmpty())
        return false;

    return m_likedTrackIds.contains(id);
}


// =============================================================
// Change like
// =============================================================

void LikesService::changeLike(
    const QString &uid,
    const QString &trackId,
    bool liked)
{
    if (m_auth == nullptr)
    {
        emit errorOccurred(
            QStringLiteral("Сервис авторизации недоступен"));

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

    m_yandexClient->setToken(
        m_auth->token());

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