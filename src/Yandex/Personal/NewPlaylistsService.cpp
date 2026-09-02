#include "NewPlaylistsService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"
#include "PlaylistService.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QPair>


// =============================================================
// Constructor
// =============================================================

NewPlaylistsService::NewPlaylistsService(
    YandexAuth *auth,
    PlaylistService *playlistService,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
    , m_playlistService(playlistService)
{
    if (
        m_playlistService != nullptr
    )
    {
        connect(
            m_playlistService,
            &PlaylistService::playlistsReceived,
            this,
            [this](
                const QList<Playlist> &playlists)
            {
                qDebug()
                    << "NewPlaylistsService:"
                    << "resolved playlists:"
                    << playlists.size();


                emit playlistsReceived(
                    playlists);
            });
    }
}


// =============================================================
// Load
// =============================================================

void NewPlaylistsService::load()
{
    qDebug()
        << "NewPlaylistsService::load()";


    if (
        m_auth == nullptr
    )
    {
        emit errorOccurred(
            "Авторизация недоступна");

        return;
    }


    if (
        !m_auth->isAuthenticated()
    )
    {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }


    if (
        m_playlistService == nullptr
    )
    {
        emit errorOccurred(
            "PlaylistService недоступен");

        return;
    }


    m_yandexClient
        ->setToken(
            m_auth->token());


    const QString path =
        "/landing3/new-playlists";


    qDebug()
        << "NewPlaylistsService:"
        << "request:"
        << path;


    QNetworkReply *reply =
        m_yandexClient
            ->get(
                path);


    if (
        reply == nullptr
    )
    {
        emit errorOccurred(
            "Не удалось выполнить запрос новых плейлистов");

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


            const int statusCode =
                reply
                    ->attribute(
                        QNetworkRequest::HttpStatusCodeAttribute)
                    .toInt();


            qDebug()
                << "NewPlaylistsService:"
                << "HTTP:"
                << statusCode;


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
                    QJsonParseError::NoError ||
                !document.isObject()
            )
            {
                emit errorOccurred(
                    "Некорректный ответ /landing3/new-playlists");

                reply->deleteLater();

                return;
            }


            const QJsonObject root =
                document.object();


            const QJsonObject result =
                root
                    .value("result")
                    .toObject();


            if (
                result.isEmpty()
            )
            {
                emit errorOccurred(
                    "Ответ /landing3/new-playlists не содержит result");

                reply->deleteLater();

                return;
            }


            const QJsonArray newPlaylists =
                result
                    .value("newPlaylists")
                    .toArray();


            QList<QPair<QString, int>>
                references;


            references.reserve(
                newPlaylists.size());


            for (
                const QJsonValue &value :
                newPlaylists
            )
            {
                if (
                    !value.isObject()
                )
                {
                    continue;
                }


                const QJsonObject object =
                    value.toObject();


                const qint64 uid =
                    object
                        .value("uid")
                        .toInteger();


                const int kind =
                    object
                        .value("kind")
                        .toInt();


                if (
                    uid <= 0 ||
                    kind <= 0
                )
                {
                    continue;
                }


                references.append(
                    qMakePair(
                        QString::number(
                            uid),
                        kind));
            }


            qDebug()
                << "NewPlaylistsService:"
                << "references:"
                << references.size();


            reply->deleteLater();


            if (
                references.isEmpty()
            )
            {
                emit playlistsReceived(
                    {});

                return;
            }


            m_playlistService
                ->loadPlaylists(
                    references);
        });
}