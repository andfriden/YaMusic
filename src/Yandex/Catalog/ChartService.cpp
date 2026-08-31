#include "ChartService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QNetworkReply>


namespace
{

Track parseTrack(
    const QJsonObject &object)
{
    Track track;

    track.id =
        object
            .value("id")
            .toString();

    if (track.id.isEmpty())
    {
        const qint64 realId =
            object
                .value("realId")
                .toInteger();

        if (realId > 0)
        {
            track.id =
                QString::number(realId);
        }
    }

    track.title =
        object
            .value("title")
            .toString();

    track.coverUri =
        object
            .value("coverUri")
            .toString();

    track.durationMs =
        object
            .value("durationMs")
            .toInt();

    const QJsonArray artists =
        object
            .value("artists")
            .toArray();

    for (
        const QJsonValue &value :
        artists
    )
    {
        if (!value.isObject())
        {
            continue;
        }

        const QJsonObject artistObject =
            value.toObject();

        Artist artist;

        const qint64 artistId =
            artistObject
                .value("id")
                .toInteger();

        if (artistId > 0)
        {
            artist.id =
                QString::number(
                    artistId);
        }

        artist.name =
            artistObject
                .value("name")
                .toString();

        const QJsonObject cover =
            artistObject
                .value("cover")
                .toObject();

        artist.coverUri =
            cover
                .value("uri")
                .toString();

        if (artist.coverUri.isEmpty())
        {
            artist.coverUri =
                artistObject
                    .value("coverUri")
                    .toString();
        }

        track.artists.append(
            artist);
    }

    return track;
}


Track parseTrackItem(
    const QJsonObject &item)
{
    const QJsonObject trackObject =
        item
            .value("track")
            .toObject();

    if (!trackObject.isEmpty())
    {
        return parseTrack(
            trackObject);
    }

    return parseTrack(
        item);
}


QList<Track> parseChartTracks(
    const QJsonArray &tracksArray)
{
    QList<Track> tracks;

    tracks.reserve(
        tracksArray.size());

    for (
        const QJsonValue &value :
        tracksArray
    )
    {
        if (!value.isObject())
        {
            continue;
        }

        const Track track =
            parseTrackItem(
                value.toObject());

        if (track.id.isEmpty())
        {
            continue;
        }

        tracks.append(
            track);
    }

    return tracks;
}

}


ChartService::ChartService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(
          new YandexClient(this))
{
}


void ChartService::loadChart(
    const QString &chartType)
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

    const QString type =
        chartType
            .trimmed()
            .toLower();

    if (
        type != "russia" &&
        type != "world"
    )
    {
        emit errorOccurred(
            "Некорректный тип чарта");

        return;
    }

    m_yandexClient->setToken(
        m_auth->token());

    const QString path =
        "/landing3/chart/" +
        type;

    QNetworkReply *reply =
        m_yandexClient->get(
            path);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, type]()
        {
            const QByteArray data =
                reply->readAll();

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
                    "Некорректный JSON чарта");

                reply->deleteLater();

                return;
            }

            const QJsonObject root =
                document.object();

            const QJsonObject result =
                root
                    .value("result")
                    .toObject();

            if (result.isEmpty())
            {
                emit errorOccurred(
                    "Пустой результат чарта");

                reply->deleteLater();

                return;
            }

            const QJsonObject chart =
                result
                    .value("chart")
                    .toObject();

            if (chart.isEmpty())
            {
                emit errorOccurred(
                    "В ответе отсутствует чарт");

                reply->deleteLater();

                return;
            }

            const QJsonArray tracksArray =
                chart
                    .value("tracks")
                    .toArray();

            const QList<Track> tracks =
                parseChartTracks(
                    tracksArray);

            emit chartReceived(
                tracks,
                type);

            reply->deleteLater();
        });
}