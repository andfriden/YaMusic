#include "YandexPersonal.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
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
    const QString trimmedQueue =
        queueTrackId.trimmed();

    if (trimmedQueue.isEmpty()) {

        emit errorOccurred(
            "Идентификатор последнего трека не задан");

        return;
    }

    loadMyWaveInternal(
        trimmedQueue);
}

void YandexPersonal::sendMyWaveFeedback(
    const QString &event,
    const QString &trackId,
    const QString &batchId,
    qint64 totalPlayedSeconds)
{
    if (m_auth == nullptr ||
        !m_auth->isAuthenticated()) {

        emit feedbackError(
            "Токен Яндекс Музыки не установлен");

        return;
    }

    const QString trimmedEvent =
        event.trimmed();

    const QString trimmedTrackId =
        trackId.trimmed();

    const QString trimmedBatchId =
        batchId.trimmed();

    if (trimmedEvent.isEmpty()) {
        return;
    }

    if (trimmedBatchId.isEmpty()) {

        emit feedbackError(
            "Для feedback не указан batchId");

        return;
    }

    if (trimmedTrackId.isEmpty()) {

        emit feedbackError(
            "Для feedback не указан trackId");

        return;
    }

    if (trimmedEvent != "trackStarted" &&
        trimmedEvent != "trackFinished" &&
        trimmedEvent != "skip") {

        emit feedbackError(
            "Неподдерживаемый тип feedback");

        return;
    }

    m_yandexClient->setToken(
        m_auth->token());

    QJsonObject body;

    body.insert(
        "type",
        trimmedEvent);

    body.insert(
        "timestamp",
        QDateTime::currentDateTimeUtc()
            .toString(Qt::ISODate));

    body.insert(
        "trackId",
        trimmedTrackId);

    if (trimmedEvent == "trackFinished" ||
        trimmedEvent == "skip") {

        body.insert(
            "totalPlayedSeconds",
            static_cast<qint64>(
                qMax(
                    qint64(0),
                    totalPlayedSeconds)));
    }

    QUrlQuery query;

    query.addQueryItem(
        "batch-id",
        trimmedBatchId);

    const QString path =
        "/rotor/station/user:onyourwave/feedback?" +
        query.toString(
            QUrl::FullyEncoded);

    QNetworkReply *reply =
        m_yandexClient->post(
            path,
            body);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this,
         reply,
         trimmedEvent]() {

            const QByteArray response =
                reply->readAll();

            if (reply->error() !=
                QNetworkReply::NoError) {

                QString message =
                    reply->errorString();

                if (!response.isEmpty()) {

                    message +=
                        " | " +
                        QString::fromUtf8(
                            response);
                }

                emit feedbackError(
                    message);

                reply->deleteLater();

                return;
            }

            emit feedbackSent(
                trimmedEvent);

            reply->deleteLater();
        });
}

void YandexPersonal::loadMyWaveInternal(
    const QString &queueTrackId)
{
    if (m_loading) {
        return;
    }

    if (m_auth == nullptr ||
        !m_auth->isAuthenticated()) {

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

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]() {

            m_loading = false;

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
                    "Ответ «Моя волна» пуст");

                reply->deleteLater();

                return;
            }

            const QString batchId =
                result.value("batchId")
                    .toString();

            const QJsonArray sequence =
                result.value("sequence")
                    .toArray();

            if (sequence.isEmpty()) {

                emit errorOccurred(
                    "Яндекс Музыка не вернула треки");

                reply->deleteLater();

                return;
            }

            QList<Track> tracks;

            for (const QJsonValue &value :
                 sequence) {

                const QJsonObject sequenceItem =
                    value.toObject();

                if (sequenceItem.isEmpty()) {
                    continue;
                }

                const QJsonObject trackObject =
                    sequenceItem
                        .value("track")
                        .toObject();

                if (trackObject.isEmpty()) {
                    continue;
                }

                const Track track =
                    parseTrack(
                        trackObject);

                if (!track.id.isEmpty()) {
                    tracks.append(track);
                }
            }

            if (tracks.isEmpty()) {

                emit errorOccurred(
                    "В ответе нет корректных треков");

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

    track.id =
        object.value("id")
            .toString();

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

    for (const QJsonValue &value :
         artists) {

        const QJsonObject artistObject =
            value.toObject();

        Artist artist;

        artist.id =
            QString::number(
                artistObject
                    .value("id")
                    .toInteger());

        artist.name =
            artistObject
                .value("name")
                .toString();

        if (!artist.name.isEmpty()) {
            track.artists.append(
                artist);
        }
    }

    const QJsonArray albums =
        object.value("albums")
            .toArray();

    for (const QJsonValue &value :
         albums) {

        const QJsonObject albumObject =
            value.toObject();

        Album album;

        album.id =
            QString::number(
                albumObject
                    .value("id")
                    .toInteger());

        album.title =
            albumObject
                .value("title")
                .toString();

        album.coverUri =
            albumObject
                .value("coverUri")
                .toString();

        album.year =
            albumObject
                .value("year")
                .toInt();

        if (!album.title.isEmpty()) {
            track.albums.append(
                album);
        }
    }

    return track;
}