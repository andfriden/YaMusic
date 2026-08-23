#include "PersonalLanding.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
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
}

PersonalLanding::PersonalLanding(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
}

void PersonalLanding::load()
{
    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    ) {

        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");

        return;
    }

    m_yandexClient
        ->setToken(
            m_auth->token());

    QUrlQuery query;

    query.addQueryItem(
        "blocks",
        LandingBlocks);

    const QString path =
        "/landing3?" +
        query.toString(
            QUrl::FullyEncoded);

    QNetworkReply *reply =
        m_yandexClient
            ->get(path);

    if (reply == nullptr) {

        emit errorOccurred(
            "Не удалось создать запрос рекомендаций");

        return;
    }

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]() {

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
                    QJsonParseError::NoError ||
                !document.isObject()
            ) {

                emit errorOccurred(
                    "Некорректный ответ от Яндекс Музыки");

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
            ) {

                emit errorOccurred(
                    "Ответ рекомендаций пуст");

                reply->deleteLater();

                return;
            }

            const QJsonArray blocks =
                result
                    .value("blocks")
                    .toArray();

            if (
                blocks.isEmpty()
            ) {

                emit errorOccurred(
                    "В ответе нет блоков рекомендаций");

                reply->deleteLater();

                return;
            }

            QList<PersonalLandingSection>
                sections;

            QList<PersonalPlaylist>
                personalPlaylists;

            for (
                const QJsonValue &value :
                blocks
            ) {

                if (
                    !value.isObject()
                ) {
                    continue;
                }

                const QJsonObject block =
                    value.toObject();

                const PersonalLandingSection section =
                    parseSection(
                        block);

                if (
                    !section.id.isEmpty() ||
                    !section.title.isEmpty() ||
                    !section.items.isEmpty()
                ) {

                    sections.append(
                        section);
                }

                if (
                    section.type ==
                    "personal-playlists"
                ) {

                    for (
                        const PersonalLandingItem &item :
                        section.items
                    ) {

                        const PersonalPlaylist playlist =
                            parsePersonalPlaylist(
                                item);

                        if (
                            !playlist.id.isEmpty() &&
                            !playlist.title.isEmpty()
                        ) {

                            personalPlaylists.append(
                                playlist);
                        }
                    }
                }
            }

            if (
                sections.isEmpty()
            ) {

                emit errorOccurred(
                    "Не удалось разобрать блоки рекомендаций");

                reply->deleteLater();

                return;
            }

            emit loaded(
                sections);

            if (
                !personalPlaylists.isEmpty()
            ) {

                emit personalPlaylistsReceived(
                    personalPlaylists);
            }

            reply->deleteLater();
        });
}

PersonalLandingItem
PersonalLanding::parseItem(
    const QJsonObject &object) const
{
    PersonalLandingItem item;

    item.id =
        object
            .value("id")
            .toString();

    item.type =
        object
            .value("type")
            .toString();

    item.data =
        object
            .value("data")
            .toObject();

    return item;
}

PersonalLandingSection
PersonalLanding::parseSection(
    const QJsonObject &object) const
{
    PersonalLandingSection section;

    section.id =
        object
            .value("id")
            .toString();

    section.title =
        object
            .value("title")
            .toString();

    section.type =
        object
            .value("type")
            .toString();

    section.typeForFrom =
        object
            .value("typeForFrom")
            .toString();

    section.description =
        object
            .value("description")
            .toString();

    const QJsonArray entities =
        object
            .value("entities")
            .toArray();

    for (
        const QJsonValue &value :
        entities
    ) {

        if (
            !value.isObject()
        ) {
            continue;
        }

        const QJsonObject entity =
            value.toObject();

        const PersonalLandingItem item =
            parseItem(
                entity);

        if (
            item.id.isEmpty() &&
            item.data.isEmpty()
        ) {

            continue;
        }

        section.items.append(
            item);
    }

    return section;
}

PersonalPlaylist
PersonalLanding::parsePersonalPlaylist(
    const PersonalLandingItem &item) const
{
    PersonalPlaylist playlist;

    const QJsonObject playlistObject =
        item.data
            .value("data")
            .toObject();

    if (
        playlistObject.isEmpty()
    ) {
        return playlist;
    }

    playlist.kind =
        playlistObject
            .value("kind")
            .toInt();

    const qint64 uid =
        playlistObject
            .value("uid")
            .toInteger();

    if (
        uid > 0
    ) {

        playlist.uid =
            QString::number(
                uid);
    }

    if (
        playlist.uid.isEmpty()
    ) {

        const qint64 ownerUid =
            playlistObject
                .value("owner")
                .toObject()
                .value("uid")
                .toInteger();

        if (
            ownerUid > 0
        ) {

            playlist.uid =
                QString::number(
                    ownerUid);
        }
    }

    if (
        !playlist.uid.isEmpty() &&
        playlist.kind > 0
    ) {

        playlist.id =
            playlist.uid +
            ":" +
            QString::number(
                playlist.kind);

    } else {

        playlist.id =
            item.id;
    }

    playlist.title =
        playlistObject
            .value("title")
            .toString();

    playlist.description =
        playlistObject
            .value("description")
            .toString();

    playlist.previewDescription =
        item.data
            .value("previewDescription")
            .toString();

    playlist.trackCount =
        playlistObject
            .value("trackCount")
            .toInt();

    playlist.generatedPlaylistType =
        playlistObject
            .value("generatedPlaylistType")
            .toString();

    const QJsonObject cover =
        playlistObject
            .value("cover")
            .toObject();

    playlist.coverUri =
        cover
            .value("uri")
            .toString();

    return playlist;
}