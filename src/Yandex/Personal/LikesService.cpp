#include "LikesService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>


LikesService::LikesService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
}


// =============================================================
// Load liked tracks
// =============================================================

void LikesService::loadLikedTracks(
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


    const QString userUid =
        uid.trimmed();


    if (
        userUid.isEmpty()
    )
    {
        emit errorOccurred(
            "UID пользователя не указан");

        return;
    }


    if (
        m_loading
    )
    {
        return;
    }


    m_loading =
        true;

    emit loadingChanged(
        true);


    m_yandexClient
        ->setToken(
            m_auth->token());


    const QString path =
        QString(
            "/users/%1/likes/tracks")
        .arg(
            userUid);


    QNetworkReply *reply =
        m_yandexClient
            ->get(path);


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
                m_loading =
                    false;

                emit loadingChanged(
                    false);


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
                    QJsonParseError::NoError ||
                !document.isObject()
            )
            {
                m_loading =
                    false;

                emit loadingChanged(
                    false);


                emit errorOccurred(
                    "Некорректный ответ списка лайков");


                reply->deleteLater();

                return;
            }


            const QJsonObject root =
                document.object();


            QJsonObject result =
                root.value("result")
                    .toObject();


            if (
                result.isEmpty()
            )
            {
                result =
                    root;
            }


            const QJsonObject library =
                result.value("library")
                    .toObject();


            const QJsonArray tracks =
                library.value("tracks")
                    .toArray();


            QStringList trackIds;


            for (
                const QJsonValue &value :
                tracks
            )
            {
                if (
                    value.isObject()
                )
                {
                    const QJsonObject object =
                        value.toObject();


                    QString id =
                        object
                            .value("id")
                            .toString();


                    if (
                        id.isEmpty()
                    )
                    {
                        const qint64 integerId =
                            object
                                .value("id")
                                .toInteger();


                        if (
                            integerId > 0
                        )
                        {
                            id =
                                QString::number(
                                    integerId);
                        }
                    }


                    if (
                        !id.isEmpty() &&
                        !trackIds.contains(id)
                    )
                    {
                        trackIds.append(
                            id);
                    }
                }
                else if (
                    value.isString()
                )
                {
                    const QString id =
                        value
                            .toString()
                            .trimmed();


                    if (
                        !id.isEmpty() &&
                        !trackIds.contains(id)
                    )
                    {
                        trackIds.append(
                            id);
                    }
                }
                else if (
                    value.isDouble()
                )
                {
                    const qint64 id =
                        static_cast<qint64>(
                            value.toDouble());


                    if (
                        id > 0
                    )
                    {
                        const QString idString =
                            QString::number(
                                id);


                        if (
                            !trackIds.contains(
                                idString)
                        )
                        {
                            trackIds.append(
                                idString);
                        }
                    }
                }
            }


            reply->deleteLater();


            if (
                trackIds.isEmpty()
            )
            {
                m_loading =
                    false;

                emit loadingChanged(
                    false);


                emit tracksReceived(
                    {});


                return;
            }


            loadTracksByIds(
                trackIds);
        });
}


// =============================================================
// Load full tracks
// =============================================================

void LikesService::loadTracksByIds(
    const QStringList &trackIds)
{
    if (
        m_yandexClient == nullptr
    )
    {
        m_loading =
            false;

        emit loadingChanged(
            false);


        emit errorOccurred(
            "YandexClient недоступен");


        return;
    }


    connect(
        m_yandexClient,
        &YandexClient::tracksReceived,
        this,
        [this](
            const QList<Track> &tracks)
        {
            m_loading =
                false;

            emit loadingChanged(
                false);


            emit tracksReceived(
                tracks);
        },
        Qt::SingleShotConnection);


    connect(
        m_yandexClient,
        &YandexClient::requestError,
        this,
        [this](
            const QString &message)
        {
            m_loading =
                false;

            emit loadingChanged(
                false);


            emit errorOccurred(
                message);
        },
        Qt::SingleShotConnection);


    m_yandexClient
        ->getTracks(
            trackIds);
}