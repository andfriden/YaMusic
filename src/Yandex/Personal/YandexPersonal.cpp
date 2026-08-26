#include "YandexPersonal.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QUrlQuery>

YandexPersonal::YandexPersonal(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
}

void YandexPersonal::loadMyWave()
{
    loadMyWaveInternal({});
}

void YandexPersonal::loadMoreMyWave(
    const QString &queueTrackId)
{
    const QString trackId =
        queueTrackId.trimmed();

    if (trackId.isEmpty()) {
        emit errorOccurred(
            "Идентификатор последнего трека "
            "для продолжения Wave не задан");
        return;
    }

    loadMyWaveInternal(trackId);
}

void YandexPersonal::sendMyWaveFeedback(
    const QString &event,
    const QString &trackId,
    const QString &batchId,
    qint64 totalPlayedSeconds)
{
    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    ) {
        emit feedbackError(
            "Токен Яндекс Музыки не установлен");
        return;
    }

    const QString type =
        event.trimmed();

    const QString id =
        trackId.trimmed();

    const QString batch =
        batchId.trimmed();

    if (type.isEmpty()) {
        return;
    }

    if (id.isEmpty()) {
        emit feedbackError(
            "Для feedback не указан trackId");
        return;
    }

    if (batch.isEmpty()) {
        emit feedbackError(
            "Для feedback не указан batchId");
        return;
    }

    if (
        type != "trackStarted" &&
        type != "trackFinished" &&
        type != "skip"
    ) {
        emit feedbackError(
            "Неподдерживаемый тип feedback");
        return;
    }

    m_yandexClient->setToken(
        m_auth->token());

    QJsonObject body;

    body.insert(
        "type",
        type);

    body.insert(
        "timestamp",
        QDateTime::currentDateTimeUtc()
            .toString(Qt::ISODate));

    body.insert(
        "trackId",
        id);

    if (
        type == "trackFinished" ||
        type == "skip"
    ) {
        body.insert(
            "totalPlayedSeconds",
            qMax(
                qint64(0),
                totalPlayedSeconds));
    }

    QUrlQuery query;

    query.addQueryItem(
        "batch-id",
        batch);

    const QString path =
        "/rotor/station/user:onyourwave/feedback?" +
        query.toString(
            QUrl::FullyEncoded);

    QNetworkReply *reply =
        m_yandexClient->post(
            path,
            body);

    if (reply == nullptr) {
        emit feedbackError(
            "Не удалось создать запрос feedback");
        return;
    }

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, type]() {

            const QByteArray response =
                reply->readAll();

            if (
                reply->error() !=
                QNetworkReply::NoError
            ) {
                QString message =
                    reply->errorString();

                if (!response.isEmpty()) {
                    message +=
                        " | " +
                        QString::fromUtf8(
                            response);
                }

                emit feedbackError(message);

                reply->deleteLater();

                return;
            }

            emit feedbackSent(type);

            reply->deleteLater();
        });
}

void YandexPersonal::loadMyWaveInternal(
    const QString &queueTrackId)
{
    if (m_loading) {
        return;
    }

    if (
        m_auth == nullptr ||
        !m_auth->isAuthenticated()
    ) {
        emit errorOccurred(
            "Токен Яндекс Музыки не установлен");
        return;
    }

    m_loading = true;

    m_yandexClient->setToken(
        m_auth->token());

    QUrlQuery query;

    query.addQueryItem(
        "settings2",
        "true");

    if (!queueTrackId.isEmpty()) {
        query.addQueryItem(
            "queue",
            queueTrackId);
    }

    const QString path =
        "/rotor/station/user:onyourwave/tracks?" +
        query.toString(
            QUrl::FullyEncoded);

    QNetworkReply *reply =
        m_yandexClient->get(path);

    if (reply == nullptr) {
        m_loading = false;

        emit errorOccurred(
            "Не удалось создать запрос My Wave");

        return;
    }

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, queueTrackId]() {

            m_loading = false;

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
                    "Некорректный ответ My Wave");

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
                    "Ответ My Wave пуст");

                reply->deleteLater();

                return;
            }

            const QString batchId =
                result.value("batchId")
                    .toString()
                    .trimmed();

            const QJsonArray sequence =
                result.value("sequence")
                    .toArray();

            if (sequence.isEmpty()) {
                emit errorOccurred(
                    queueTrackId.isEmpty()
                        ? "Яндекс Музыка не вернула "
                          "первую партию My Wave"
                        : "Яндекс Музыка не вернула "
                          "следующую партию My Wave");

                reply->deleteLater();

                return;
            }

            QList<Track> tracks;

            for (
                const QJsonValue &value :
                sequence
            ) {
                if (!value.isObject()) {
                    continue;
                }

                const QJsonObject item =
                    value.toObject();

                const QJsonObject trackObject =
                    item.value("track")
                        .toObject();

                if (trackObject.isEmpty()) {
                    continue;
                }

                const QString type =
                    item.value("type")
                        .toString();

                if (
                    !type.isEmpty() &&
                    type != "track"
                ) {
                    continue;
                }

                const Track track =
                    parseTrack(trackObject);

                if (!track.id.isEmpty()) {
                    tracks.append(track);
                }
            }

            if (tracks.isEmpty()) {
                emit errorOccurred(
                    "В ответе My Wave нет "
                    "корректных треков");

                reply->deleteLater();

                return;
            }

            emit myWaveReceived(
                tracks,
                batchId);

            reply->deleteLater();
        });
}

Track YandexPersonal::parseTrack(
    const QJsonObject &object) const
{
    Track track;

    const QJsonValue idValue =
        object.value("id");

    if (idValue.isString()) {
        track.id =
            idValue.toString();
    }
    else if (idValue.isDouble()) {
        track.id =
            QString::number(
                idValue.toInteger());
    }

    track.title =
        object.value("title")
            .toString();

    track.coverUri =
        object.value("coverUri")
            .toString();

    track.durationMs =
        object.value("durationMs")
            .toInt();

    const QJsonArray artists =
        object.value("artists")
            .toArray();

    for (
        const QJsonValue &value :
        artists
    ) {
        if (!value.isObject()) {
            continue;
        }

        const QJsonObject object =
            value.toObject();

        Artist artist;

        const QJsonValue id =
            object.value("id");

        if (id.isString()) {
            artist.id =
                id.toString();
        }
        else if (id.isDouble()) {
            artist.id =
                QString::number(
                    id.toInteger());
        }

        artist.name =
            object.value("name")
                .toString();

        if (
            !artist.id.isEmpty() ||
            !artist.name.isEmpty()
        ) {
            track.artists.append(artist);
        }
    }

    const QJsonArray albums =
        object.value("albums")
            .toArray();

    for (
        const QJsonValue &value :
        albums
    ) {
        if (!value.isObject()) {
            continue;
        }

        const QJsonObject object =
            value.toObject();

        Album album;

        const QJsonValue id =
            object.value("id");

        if (id.isString()) {
            album.id =
                id.toString();
        }
        else if (id.isDouble()) {
            album.id =
                QString::number(
                    id.toInteger());
        }

        album.title =
            object.value("title")
                .toString();

        album.coverUri =
            object.value("coverUri")
                .toString();

        album.year =
            object.value("year")
                .toInt();

        if (
            !album.id.isEmpty() ||
            !album.title.isEmpty()
        ) {
            track.albums.append(album);
        }
    }

    return track;
}