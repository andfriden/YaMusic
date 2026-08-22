#include "PersonalLanding.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QStringList>
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
    "play_contexts,"
    "podcasts";
}

PersonalLanding::PersonalLanding(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(new YandexClient(this))
{
}

void PersonalLanding::load()
{
    if (m_auth == nullptr ||
        !m_auth->isAuthenticated()) {

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
                    "Некорректный ответ от Яндекс Музыки");

                reply->deleteLater();

                return;
            }

            const QJsonObject root =
                document.object();

            const QJsonObject result =
                root.value("result")
                    .toObject();

            if (result.isEmpty()) {

                emit errorOccurred(
                    "Ответ рекомендаций пуст");

                reply->deleteLater();

                return;
            }

            const QJsonArray blocks =
                result.value("blocks")
                    .toArray();

            if (blocks.isEmpty()) {

                emit errorOccurred(
                    "В ответе нет блоков рекомендаций");

                reply->deleteLater();

                return;
            }

            QList<PersonalLandingSection>
                sections;

            QList<PersonalPlaylist>
                personalPlaylists;

            QList<RecentListeningItem>
                recentListening;

            for (const QJsonValue &value :
                 blocks) {

                if (!value.isObject()) {
                    continue;
                }

                const QJsonObject block =
                    value.toObject();

                const PersonalLandingSection section =
                    parseSection(block);

                if (!section.id.isEmpty() ||
                    !section.title.isEmpty() ||
                    !section.items.isEmpty()) {

                    sections.append(
                        section);
                }

                if (section.type ==
                    "personal-playlists") {

                    for (
                        const PersonalLandingItem &item :
                        section.items) {

                        const PersonalPlaylist playlist =
                            parsePersonalPlaylist(
                                item);

                        if (!playlist.id.isEmpty() &&
                            !playlist.title.isEmpty()) {

                            personalPlaylists.append(
                                playlist);
                        }
                    }
                }

                if (section.type ==
                    "play-contexts") {

                    for (
                        const PersonalLandingItem &item :
                        section.items) {

                        const RecentListeningItem recentItem =
                            parseRecentListeningItem(
                                item);

                        if (!recentItem.id.isEmpty()) {

                            recentListening.append(
                                recentItem);
                        }
                    }
                }
            }

            if (sections.isEmpty()) {

                emit errorOccurred(
                    "Не удалось разобрать блоки рекомендаций");

                reply->deleteLater();

                return;
            }

            emit loaded(
                sections);

            if (!personalPlaylists.isEmpty()) {

                emit personalPlaylistsReceived(
                    personalPlaylists);
            }

            if (!recentListening.isEmpty()) {

                emit recentListeningReceived(
                    recentListening);
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

    const QJsonArray entities =
        object.value("entities")
            .toArray();

    for (const QJsonValue &value :
         entities) {

        if (!value.isObject()) {
            continue;
        }

        const QJsonObject entity =
            value.toObject();

        const PersonalLandingItem item =
            parseItem(entity);

        if (item.id.isEmpty() &&
            item.data.isEmpty()) {

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

    /*
     * Реальная структура personal-playlist:
     *
     * item.data
     * ├── data
     * │   ├── uid
     * │   ├── kind
     * │   ├── title
     * │   ├── description
     * │   ├── trackCount
     * │   ├── generatedPlaylistType
     * │   └── cover.uri
     * ├── previewDescription
     * ├── ready
     * └── type
     */

    const QJsonObject playlistObject =
        item.data
            .value("data")
            .toObject();

    if (playlistObject.isEmpty()) {
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

    if (uid > 0) {

        playlist.uid =
            QString::number(
                uid);
    }

    if (playlist.uid.isEmpty()) {

        const qint64 ownerUid =
            playlistObject
                .value("owner")
                .toObject()
                .value("uid")
                .toInteger();

        if (ownerUid > 0) {

            playlist.uid =
                QString::number(
                    ownerUid);
        }
    }

    if (!playlist.uid.isEmpty() &&
        playlist.kind > 0) {

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
        cover.value("uri")
            .toString();

    return playlist;
}

RecentListeningItem
PersonalLanding::parseRecentListeningItem(
    const PersonalLandingItem &item) const
{
    RecentListeningItem result;

    result.id =
        item.id;

    result.type =
        item.type;

    const QJsonObject data =
        item.data;

    result.context =
        data.value("context")
            .toString();

    result.contextItem =
        data.value("contextItem")
            .toString();

    const QJsonObject payload =
        data.value("payload")
            .toObject();

    /*
     * Playlist
     */

    if (result.context == "playlist") {

        result.title =
            payload.value("title")
                .toString();

        result.subtitle =
            payload.value("description")
                .toString();

        const qint64 uid =
            payload.value("uid")
                .toInteger();

        if (uid > 0) {

            result.uid =
                QString::number(
                    uid);
        }

        result.kind =
            payload.value("kind")
                .toInt();

        const QJsonObject cover =
            payload.value("cover")
                .toObject();

        result.coverUri =
            cover.value("uri")
                .toString();

        /*
         * Fallback для playlist:
         *
         * uid:kind
         */

        if (
            (result.uid.isEmpty() ||
             result.kind <= 0) &&
            result.contextItem.contains(":")
        ) {

            const QStringList parts =
                result.contextItem.split(":");

            if (parts.size() == 2) {

                if (result.uid.isEmpty()) {

                    result.uid =
                        parts.at(0);
                }

                if (result.kind <= 0) {

                    result.kind =
                        parts.at(1)
                            .toInt();
                }
            }
        }
    }

    /*
     * Album
     *
     * Реальная структура:
     *
     * payload
     * ├── id
     * ├── title
     * ├── coverUri
     * ├── trackCount
     * ├── genre
     * └── artists[]
     */

    else if (result.context == "album") {

        result.title =
            payload.value("title")
                .toString();

        result.uid =
            result.contextItem;

        result.coverUri =
            payload.value("coverUri")
                .toString();

        result.subtitle =
            payload.value("genre")
                .toString();

        /*
         * Первый исполнитель альбома.
         */

        const QJsonArray artists =
            payload.value("artists")
                .toArray();

        if (!artists.isEmpty()) {

            const QJsonObject artist =
                artists.first()
                    .toObject();

            const QString artistName =
                artist.value("name")
                    .toString();

            if (!artistName.isEmpty()) {

                if (result.subtitle.isEmpty()) {

                    result.subtitle =
                        artistName;

                } else {

                    result.subtitle =
                        artistName +
                        " • " +
                        result.subtitle;
                }
            }
        }

        if (result.subtitle.isEmpty()) {

            result.subtitle =
                "Альбом";
        }
    }

    /*
     * Artist
     *
     * Реальная структура:
     *
     * payload
     * ├── id
     * ├── name
     * ├── cover
     * └── genres[]
     */

    else if (result.context == "artist") {

        const QJsonObject artist =
            payload.isEmpty()
                ? data
                : payload;

        result.title =
            artist.value("name")
                .toString();

        if (result.title.isEmpty()) {

            result.title =
                artist.value("title")
                    .toString();
        }

        const qint64 artistId =
            artist.value("id")
                .toInteger();

        if (artistId > 0) {

            result.uid =
                QString::number(
                    artistId);
        }

        if (result.uid.isEmpty()) {

            result.uid =
                result.contextItem;
        }

        const QJsonObject cover =
            artist.value("cover")
                .toObject();

        result.coverUri =
            cover.value("uri")
                .toString();

        const QJsonArray genres =
            artist.value("genres")
                .toArray();

        if (!genres.isEmpty()) {

            QStringList genreNames;

            for (const QJsonValue &value :
                 genres) {

                if (!value.isString()) {
                    continue;
                }

                genreNames.append(
                    value.toString());
            }

            result.subtitle =
                genreNames.join(", ");
        }

        if (result.subtitle.isEmpty()) {

            result.subtitle =
                "Исполнитель";
        }
    }

    /*
     * Общие fallback-правила.
     */

    if (result.title.isEmpty()) {

        result.title =
            data.value("name")
                .toString();
    }

    if (result.title.isEmpty()) {

        result.title =
            data.value("title")
                .toString();
    }

    /*
     * Для album не используем
     * contextItem как название,
     * потому что это ID.
     */

    if (result.title.isEmpty() &&
        result.context != "album" &&
        !result.contextItem.isEmpty()) {

        result.title =
            result.contextItem;
    }

    /*
     * Последний fallback для artwork.
     */

    if (result.coverUri.isEmpty()) {

        const QJsonObject cover =
            data.value("cover")
                .toObject();

        result.coverUri =
            cover.value("uri")
                .toString();
    }

    /*
     * Общий fallback ID.
     */

    if (result.uid.isEmpty() &&
        !result.contextItem.isEmpty()) {

        result.uid =
            result.contextItem;
    }

    return result;
}