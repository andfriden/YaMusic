#include "PersonalLanding.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSet>
#include <QUrlQuery>


namespace
{

const QString LandingBlocks =
    "personalplaylists,"
    "promotions,"
    "new-releases,"
    "new-playlists,"
    "mixes,"
    "chart,"
    "playlists,"
    "podcasts";


const QStringList PlaylistSectionTypes =
{
    "personal-playlists",
    "new-playlists",
    "playlists",
    "editorial-playlists"
};

}


// =============================================================
// Constructor
// =============================================================

PersonalLanding::PersonalLanding(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(new YandexClient(this))
{
}


// =============================================================
// Load
// =============================================================

void PersonalLanding::load()
{
    if (m_auth == nullptr)
    {
        emit errorOccurred(
            "Авторизация недоступна");

        return;
    }


    if (!m_auth->isAuthenticated())
    {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }


    m_yandexClient->setToken(
        m_auth->token());


    QUrlQuery query;

    query.addQueryItem(
        "blocks",
        LandingBlocks);


    const QString path =
        "/landing3?" +
        query.toString(
            QUrl::FullyEncoded);


    qDebug()
        << "PersonalLanding request:"
        << path;


    QNetworkReply *reply =
        m_yandexClient->get(path);


    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]()
        {
            const QByteArray data =
                reply->readAll();


            if (
                reply->error()
                != QNetworkReply::NoError
            )
            {
                emit errorOccurred(
                    reply->errorString());

                reply->deleteLater();

                return;
            }


            QJsonParseError error;


            const QJsonDocument document =
                QJsonDocument::fromJson(
                    data,
                    &error);


            if (
                error.error
                    != QJsonParseError::NoError
                ||
                !document.isObject()
            )
            {
                emit errorOccurred(
                    "Ошибка JSON landing3");

                reply->deleteLater();

                return;
            }


            const QJsonObject result =
                document.object()
                    .value("result")
                    .toObject();


            const QJsonArray blocks =
                result.value("blocks")
                    .toArray();


            QList<PersonalLandingSection> sections;

            QList<PersonalPlaylist> playlists;

            QSet<QString> playlistIds;


            for (
                const QJsonValue &value :
                blocks
            )
            {
                if (!value.isObject())
                {
                    continue;
                }


                const QJsonObject block =
                    value.toObject();


                PersonalLandingSection section =
                    parseSection(block);


                if (
                    PlaylistSectionTypes.contains(
                        section.type)
                )
                {
                    for (
                        const PersonalLandingItem &item :
                        section.items
                    )
                    {
                        PersonalPlaylist playlist =
                            parsePersonalPlaylist(item);


                        if (
                            playlist.title.isEmpty()
                            ||
                            playlist.id.isEmpty()
                        )
                        {
                            continue;
                        }


                        section.playlists.append(
                            playlist);


                        if (
                            playlistIds.contains(
                                playlist.id)
                        )
                        {
                            continue;
                        }


                        playlistIds.insert(
                            playlist.id);


                        playlists.append(
                            playlist);
                    }
                }


                sections.append(
                    section);
            }


            qDebug()
                << "Рекомендации: секций"
                << sections.size();


            qDebug()
                << "Персональных плейлистов:"
                << playlists.size();


            emit loaded(
                sections);


            emit personalPlaylistsReceived(
                playlists);


            reply->deleteLater();
        });
}


// =============================================================
// Parse item
// =============================================================

PersonalLandingItem
PersonalLanding::parseItem(
    const QJsonObject &object) const
{
    PersonalLandingItem item;


    item.id =
        object.value("id")
            .toString();


    item.type =
        object.value("type")
            .toString();


    item.data =
        object.value("data")
            .toObject();


    return item;
}


// =============================================================
// Parse section
// =============================================================

PersonalLandingSection
PersonalLanding::parseSection(
    const QJsonObject &object) const
{
    PersonalLandingSection section;


    section.id =
        object.value("id")
            .toString();


    section.title =
        object.value("title")
            .toString();


    section.type =
        object.value("type")
            .toString();


    section.typeForFrom =
        object.value("typeForFrom")
            .toString();


    section.description =
        object.value("description")
            .toString();


    QJsonArray entities =
        object.value("entities")
            .toArray();


    if (entities.isEmpty())
    {
        entities =
            object.value("items")
                .toArray();
    }


    for (
        const QJsonValue &value :
        entities
    )
    {
        if (!value.isObject())
        {
            continue;
        }


        section.items.append(
            parseItem(
                value.toObject()));
    }


    return section;
}


// =============================================================
// Parse playlist
// =============================================================

PersonalPlaylist
PersonalLanding::parsePersonalPlaylist(
    const PersonalLandingItem &item) const
{
    PersonalPlaylist playlist;


    QJsonObject object =
        item.data;


    if (
        object.contains("data")
        &&
        object.value("data").isObject()
    )
    {
        object =
            object.value("data")
                .toObject();
    }


    if (
        object.contains("playlist")
        &&
        object.value("playlist").isObject()
    )
    {
        object =
            object.value("playlist")
                .toObject();
    }


    playlist.title =
        object.value("title")
            .toString();


    playlist.description =
        object.value("description")
            .toString();


    playlist.trackCount =
        object.value("trackCount")
            .toInt();


    playlist.kind =
        object.value("kind")
            .toInt();


    const qint64 uid =
        object.value("uid")
            .toInteger();


    if (uid > 0)
    {
        playlist.uid =
            QString::number(uid);
    }


    if (
        !playlist.uid.isEmpty()
        &&
        playlist.kind > 0
    )
    {
        playlist.id =
            playlist.uid
            + ":"
            + QString::number(
                playlist.kind);
    }
    else
    {
        playlist.id =
            item.id;
    }


    playlist.coverUri =
        object.value("coverUri")
            .toString();


    if (
        playlist.coverUri.isEmpty()
    )
    {
        playlist.coverUri =
            object.value("cover")
                .toObject()
                .value("uri")
                .toString();
    }


    return playlist;
}