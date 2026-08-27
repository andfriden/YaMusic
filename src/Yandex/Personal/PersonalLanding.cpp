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
}


// =============================================================
// Constructor
// =============================================================

PersonalLanding::PersonalLanding(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
    qDebug()
        << "PersonalLanding CREATED";
}


// =============================================================
// Load
// =============================================================

void PersonalLanding::load()
{
    qDebug()
        << "========================================";

    qDebug()
        << "PersonalLanding::load";

    if (
        m_auth == nullptr
    ) {
        qDebug()
            << "ERROR: auth is null";

        emit errorOccurred(
            "Авторизация недоступна");

        return;
    }


    if (
        !m_auth->isAuthenticated()
    ) {
        qDebug()
            << "ERROR: not authenticated";

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


    qDebug()
        << "Request:"
        << path;


    QNetworkReply *reply =
        m_yandexClient
            ->get(path);


    if (
        reply == nullptr
    ) {
        qDebug()
            << "ERROR: reply is null";

        emit errorOccurred(
            "Не удалось создать запрос рекомендаций");

        return;
    }


    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]() {

            qDebug()
                << "========================================";

            qDebug()
                << "PersonalLanding RESPONSE";


            const QByteArray data =
                reply->readAll();


            qDebug()
                << "HTTP error:"
                << reply->error();

            qDebug()
                << "HTTP error string:"
                << reply->errorString();


            if (
                reply->error() !=
                QNetworkReply::NoError
            ) {

                emit errorOccurred(
                    reply->errorString());

                reply->deleteLater();

                return;
            }


            qDebug()
                << "Response bytes:"
                << data.size();


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

                qDebug()
                    << "JSON parse error:"
                    << parseError.errorString();


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

                qDebug()
                    << "ERROR: result is empty";


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

                qDebug()
                    << "ERROR: blocks is empty";


                emit errorOccurred(
                    "В ответе нет блоков рекомендаций");

                reply->deleteLater();

                return;
            }


            QList<PersonalLandingSection>
                sections;


            QList<PersonalPlaylist>
                personalPlaylists;


            QSet<QString>
                playlistIds;


            /*
             * =====================================================
             * Parse every landing block
             * =====================================================
             */

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


                /*
                 * -------------------------------------------------
                 * Log raw block metadata
                 * -------------------------------------------------
                 */

                const QString blockId =
                    block
                        .value("id")
                        .toString();


                const QString blockTitle =
                    block
                        .value("title")
                        .toString();


                const QString blockType =
                    block
                        .value("type")
                        .toString();


                const QJsonArray entities =
                    block
                        .value("entities")
                        .toArray();


                qDebug()
                    << "----------------------------------------";


                qDebug()
                    << "LANDING BLOCK";

                qDebug()
                    << "id:"
                    << blockId;

                qDebug()
                    << "title:"
                    << blockTitle;

                qDebug()
                    << "type:"
                    << blockType;

                qDebug()
                    << "entities:"
                    << entities.size();


                /*
                 * -------------------------------------------------
                 * Parse section
                 * -------------------------------------------------
                 */

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


                /*
                 * -------------------------------------------------
                 * Personal playlists
                 * -------------------------------------------------
                 */

                if (
                    section.type ==
                    "personal-playlists"
                ) {

                    qDebug()
                        << "Processing personal-playlists"
                        << "| items:"
                        << section.items.size();


                    for (
                        int i = 0;
                        i < section.items.size();
                        ++i
                    ) {

                        const PersonalLandingItem &item =
                            section.items.at(i);


                        const PersonalPlaylist playlist =
                            parsePersonalPlaylist(
                                item);


                        qDebug()
                            << "PERSONAL PLAYLIST"
                            << i
                            << "| item id:"
                            << item.id
                            << "| parsed id:"
                            << playlist.id
                            << "| title:"
                            << playlist.title
                            << "| uid:"
                            << playlist.uid
                            << "| kind:"
                            << playlist.kind;


                        if (
                            playlist.id.isEmpty() ||
                            playlist.title.isEmpty()
                        ) {

                            qDebug()
                                << "Skipped:"
                                << "invalid personal playlist";

                            continue;
                        }


                        if (
                            playlistIds.contains(
                                playlist.id)
                        ) {

                            qDebug()
                                << "Skipped duplicate:"
                                << playlist.id;

                            continue;
                        }


                        playlistIds.insert(
                            playlist.id);


                        personalPlaylists.append(
                            playlist);
                    }
                }


                /*
                 * -------------------------------------------------
                 * New playlists
                 *
                 * IMPORTANT:
                 * We log the complete item JSON because its
                 * structure may differ from personal-playlists.
                 * -------------------------------------------------
                 */

                if (
                    section.type ==
                    "new-playlists"
                ) {

                    qDebug()
                        << "Processing new-playlists"
                        << "| items:"
                        << section.items.size();


                    for (
                        int i = 0;
                        i < section.items.size();
                        ++i
                    ) {

                        const PersonalLandingItem &item =
                            section.items.at(i);


                        qDebug()
                            << "========================================";

                        qDebug()
                            << "NEW PLAYLIST ITEM"
                            << i;


                        qDebug()
                            << "item.id:"
                            << item.id;


                        qDebug()
                            << "item.type:"
                            << item.type;


                        qDebug()
                            << "item.data:"
                            << QJsonDocument(
                                   item.data
                               ).toJson(
                                   QJsonDocument::Compact);


                        qDebug()
                            << "========================================";


                        const PersonalPlaylist playlist =
                            parsePersonalPlaylist(
                                item);


                        qDebug()
                            << "NEW PLAYLIST PARSED"
                            << i
                            << "| id:"
                            << playlist.id
                            << "| title:"
                            << playlist.title
                            << "| uid:"
                            << playlist.uid
                            << "| kind:"
                            << playlist.kind
                            << "| cover:"
                            << playlist.coverUri;


                        /*
                         * Do not silently discard the object.
                         * We need to know what structure Yandex
                         * actually returned.
                         */

                        if (
                            playlist.id.isEmpty() ||
                            playlist.title.isEmpty()
                        ) {

                            qDebug()
                                << "NEW PLAYLIST SKIPPED"
                                << "| parser could not extract playlist";

                            continue;
                        }


                        if (
                            playlistIds.contains(
                                playlist.id)
                        ) {

                            qDebug()
                                << "NEW PLAYLIST DUPLICATE:"
                                << playlist.id;

                            continue;
                        }


                        playlistIds.insert(
                            playlist.id);


                        personalPlaylists.append(
                            playlist);
                    }
                }
            }


            /*
             * =====================================================
             * Final diagnostics
             * =====================================================
             */

            qDebug()
                << "========================================";

            qDebug()
                << "PersonalLanding sections:"
                << sections.size();


            for (
                const PersonalLandingSection &section :
                sections
            ) {

                qDebug()
                    << "Section:"
                    << section.title
                    << "| type:"
                    << section.type
                    << "| items:"
                    << section.items.size();
            }


            qDebug()
                << "Playlist cards:"
                << personalPlaylists.size();


            for (
                int i = 0;
                i < personalPlaylists.size();
                ++i
            ) {

                const PersonalPlaylist &playlist =
                    personalPlaylists.at(i);


                qDebug()
                    << "Playlist"
                    << i
                    << "| title:"
                    << playlist.title
                    << "| id:"
                    << playlist.id
                    << "| uid:"
                    << playlist.uid
                    << "| kind:"
                    << playlist.kind
                    << "| tracks:"
                    << playlist.trackCount
                    << "| cover:"
                    << playlist.coverUri;
            }


            qDebug()
                << "========================================";


            /*
             * =====================================================
             * Validate sections
             * =====================================================
             */

            if (
                sections.isEmpty()
            ) {

                emit errorOccurred(
                    "Не удалось разобрать блоки рекомендаций");

                reply->deleteLater();

                return;
            }


            /*
             * =====================================================
             * Emit sections
             * =====================================================
             */

            emit loaded(
                sections);


            /*
             * =====================================================
             * Emit playlists
             * =====================================================
             */

            if (
                !personalPlaylists.isEmpty()
            ) {

                emit personalPlaylistsReceived(
                    personalPlaylists);
            }


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


// =============================================================
// Parse section
// =============================================================

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


// =============================================================
// Parse personal playlist
// =============================================================

PersonalPlaylist
PersonalLanding::parsePersonalPlaylist(
    const PersonalLandingItem &item) const
{
    PersonalPlaylist playlist;


    qDebug()
        << "parsePersonalPlaylist"
        << "| item id:"
        << item.id
        << "| item type:"
        << item.type;


    /*
     * ==========================================================
     * Current known structure:
     *
     * item.data["data"]
     *
     * Some landing blocks may use a different nesting.
     * We inspect all obvious candidates here.
     * ==========================================================
     */

    QJsonObject playlistObject =
        item.data
            .value("data")
            .toObject();


    if (
        playlistObject.isEmpty()
    ) {

        /*
         * Fallback:
         *
         * Sometimes the playlist data itself can be directly
         * inside item.data.
         */

        playlistObject =
            item.data;
    }


    if (
        playlistObject.isEmpty()
    ) {

        qDebug()
            << "parsePersonalPlaylist:"
            << "playlist object is empty";

        return playlist;
    }


    qDebug()
        << "playlist object:"
        << QJsonDocument(
               playlistObject
           ).toJson(
               QJsonDocument::Compact);


    /*
     * ==========================================================
     * kind
     * ==========================================================
     */

    playlist.kind =
        playlistObject
            .value("kind")
            .toInt();


    /*
     * ==========================================================
     * uid
     * ==========================================================
     */

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


    /*
     * owner.uid fallback
     * ==========================================================
     */

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


    /*
     * ==========================================================
     * id
     * ==========================================================
     */

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


    /*
     * ==========================================================
     * title
     * ==========================================================
     */

    playlist.title =
        playlistObject
            .value("title")
            .toString();


    /*
     * ==========================================================
     * description
     * ==========================================================
     */

    playlist.description =
        playlistObject
            .value("description")
            .toString();


    playlist.previewDescription =
        item.data
            .value("previewDescription")
            .toString();


    /*
     * ==========================================================
     * track count
     * ==========================================================
     */

    playlist.trackCount =
        playlistObject
            .value("trackCount")
            .toInt();


    /*
     * ==========================================================
     * generated playlist type
     * ==========================================================
     */

    playlist.generatedPlaylistType =
        playlistObject
            .value("generatedPlaylistType")
            .toString();


    /*
     * ==========================================================
     * Cover
     * ==========================================================
     */

    const QJsonObject cover =
        playlistObject
            .value("cover")
            .toObject();


    playlist.coverUri =
        cover
            .value("uri")
            .toString();


    /*
     * ==========================================================
     * Final parser diagnostics
     * ==========================================================
     */

    qDebug()
        << "parsePersonalPlaylist RESULT"
        << "| id:"
        << playlist.id
        << "| title:"
        << playlist.title
        << "| uid:"
        << playlist.uid
        << "| kind:"
        << playlist.kind
        << "| tracks:"
        << playlist.trackCount
        << "| cover:"
        << playlist.coverUri;


    return playlist;
}