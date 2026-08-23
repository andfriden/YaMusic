#include "RecentListeningService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrlQuery>

RecentListeningService::RecentListeningService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
    connect(
        m_yandexClient,
        &YandexClient::tracksReceived,
        this,
        [this](
            const QList<Track> &tracks) {

            QList<Track> orderedTracks;

            QHash<QString, Track>
                tracksById;

            for (
                const Track &track :
                tracks
            ) {

                if (
                    track.id.isEmpty()
                ) {
                    continue;
                }

                tracksById.insert(
                    track.id,
                    track);
            }

            for (
                const RecentListeningTrack
                    &reference :
                m_pendingReferences
            ) {

                const auto iterator =
                    tracksById.constFind(
                        reference.trackId);

                if (
                    iterator ==
                    tracksById.constEnd()
                ) {
                    continue;
                }

                orderedTracks.append(
                    iterator.value());
            }

            m_pendingReferences.clear();

            m_loading =
                false;

            emit loadingChanged(
                false);

            qDebug()
                << "Recent listening tracks:"
                << orderedTracks.size();

            for (
                const Track &track :
                orderedTracks
            ) {

                QString artistName;

                if (
                    !track.artists.isEmpty()
                ) {

                    artistName =
                        track.artists
                            .first()
                            .name;
                }

                qDebug()
                    << "Recent Track:"
                    << track.id
                    << "|"
                    << track.title
                    << "|"
                    << artistName;
            }

            emit tracksReceived(
                orderedTracks);
        });

    connect(
        m_yandexClient,
        &YandexClient::requestError,
        this,
        [this](
            const QString &message) {

            m_pendingReferences.clear();

            m_loading =
                false;

            emit loadingChanged(
                false);

            emit errorOccurred(
                message);
        });
}

void RecentListeningService::setUserId(
    const QString &userId)
{
    m_userId =
        userId.trimmed();
}

void RecentListeningService::load(
    int trackCount,
    int contextCount)
{
    if (m_loading) {
        return;
    }

    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    ) {

        emit errorOccurred(
            "Yandex Music token is not set");

        return;
    }

    if (m_userId.isEmpty()) {

        emit errorOccurred(
            "Yandex Music user id is not set");

        return;
    }

    if (trackCount <= 0) {
        trackCount = 50;
    }

    if (contextCount <= 0) {
        contextCount = 10;
    }

    m_yandexClient
        ->setToken(
            m_auth->token());

    QUrlQuery query;

    query.addQueryItem(
        "trackCount",
        QString::number(
            trackCount));

    query.addQueryItem(
        "contextCount",
        QString::number(
            contextCount));

    query.addQueryItem(
        "types",
        "playlist,album,artist");

    const QString path =
        QString(
            "/users/%1/contexts?%2")
            .arg(
                m_userId)
            .arg(
                query.toString(
                    QUrl::FullyEncoded));

    m_pendingReferences.clear();

    m_loading =
        true;

    emit loadingChanged(
        true);

    QNetworkReply *reply =
        m_yandexClient
            ->get(
                path);

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

                const QString message =
                    reply->errorString();

                reply->deleteLater();

                m_loading =
                    false;

                emit loadingChanged(
                    false);

                emit errorOccurred(
                    message);

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

                reply->deleteLater();

                m_loading =
                    false;

                emit loadingChanged(
                    false);

                emit errorOccurred(
                    "Некорректный ответ истории "
                    "Яндекс Музыки");

                return;
            }

            const QList<RecentListeningTrack>
                references =
                parseResponse(
                    data);

            reply->deleteLater();

            qDebug()
                << "Recent listening references:"
                << references.size();

            if (
                references.isEmpty()
            ) {

                m_loading =
                    false;

                emit loadingChanged(
                    false);

                emit tracksReceived({});

                return;
            }

            m_pendingReferences =
                references;

            QStringList trackIds;

            for (
                const RecentListeningTrack
                    &reference :
                references
            ) {

                if (
                    reference.trackId.isEmpty()
                ) {
                    continue;
                }

                if (
                    !trackIds.contains(
                        reference.trackId)
                ) {

                    trackIds.append(
                        reference.trackId);
                }
            }

            if (trackIds.isEmpty()) {

                m_pendingReferences.clear();

                m_loading =
                    false;

                emit loadingChanged(
                    false);

                emit tracksReceived({});

                return;
            }

            qDebug()
                << "Resolving recent tracks:"
                << trackIds.size();

            m_yandexClient
                ->getTracks(
                    trackIds);
        });
}

QList<RecentListeningTrack>
RecentListeningService::parseResponse(
    const QByteArray &data) const
{
    QList<RecentListeningTrack>
        result;

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
        return result;
    }

    const QJsonObject root =
        document.object();

    const QJsonObject resultObject =
        root
            .value("result")
            .toObject();

    if (
        resultObject.isEmpty()
    ) {
        return result;
    }

    const QJsonArray contexts =
        resultObject
            .value("contexts")
            .toArray();

    for (
        const QJsonValue &contextValue :
        contexts
    ) {

        if (
            !contextValue.isObject()
        ) {
            continue;
        }

        const QJsonObject context =
            contextValue.toObject();

        const QJsonArray tracks =
            context
                .value("tracks")
                .toArray();

        for (
            const QJsonValue &trackValue :
            tracks
        ) {

            if (
                !trackValue.isObject()
            ) {
                continue;
            }

            const RecentListeningTrack track =
                parseListenedTrack(
                    trackValue.toObject());

            if (
                track.trackId.isEmpty()
            ) {
                continue;
            }

            result.append(
                track);
        }
    }

    QList<RecentListeningTrack>
        uniqueTracks;

    for (
        const RecentListeningTrack &track :
        result
    ) {

        bool exists = false;

        for (
            const RecentListeningTrack &existing :
            uniqueTracks
        ) {

            if (
                existing.trackId ==
                track.trackId
            ) {

                exists = true;

                break;
            }
        }

        if (!exists) {

            uniqueTracks.append(
                track);
        }
    }

    return uniqueTracks;
}

RecentListeningTrack
RecentListeningService::parseListenedTrack(
    const QJsonObject &object) const
{
    RecentListeningTrack result;

    const QJsonValue trackIdValue =
        object.value("trackId");

    if (
        trackIdValue.isObject()
    ) {

        const QJsonObject trackIdObject =
            trackIdValue.toObject();

        const QJsonValue idValue =
            trackIdObject.value("id");

        if (
            idValue.isString()
        ) {

            result.trackId =
                idValue.toString();

        } else if (
            idValue.isDouble()
        ) {

            const qint64 id =
                idValue.toInteger();

            if (id > 0) {

                result.trackId =
                    QString::number(
                        id);
            }
        }

    } else if (
        trackIdValue.isString()
    ) {

        result.trackId =
            trackIdValue.toString();

    } else if (
        trackIdValue.isDouble()
    ) {

        const qint64 id =
            trackIdValue.toInteger();

        if (id > 0) {

            result.trackId =
                QString::number(
                    id);
        }
    }

    QString timestampString =
        object
            .value("timeStamp")
            .toString();

    if (
        timestampString.isEmpty()
    ) {

        timestampString =
            object
                .value("timestamp")
                .toString();
    }

    if (
        !timestampString.isEmpty()
    ) {

        result.timestamp =
            QDateTime::fromString(
                timestampString,
                Qt::ISODate);

        if (
            !result.timestamp.isValid()
        ) {

            result.timestamp =
                QDateTime::fromString(
                    timestampString,
                    Qt::ISODateWithMs);
        }
    }

    return result;
}