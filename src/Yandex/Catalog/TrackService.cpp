#include "TrackService.h"

#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QXmlStreamReader>

namespace
{
constexpr auto DownloadInfoSalt =
    "XGRlBW9FXlekgbPrRHuSiA";
}

TrackService::TrackService(
    YandexAuth *auth,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_yandexClient(new YandexClient(this))
{
}

void TrackService::loadStreamInfo(
    const QString &trackId)
{
    if (m_auth == nullptr ||
        !m_auth->isAuthenticated()) {

        emit errorOccurred(
            "Yandex Music token is not set");

        return;
    }

    const QString trimmedTrackId =
        trackId.trimmed();

    if (trimmedTrackId.isEmpty()) {

        emit errorOccurred(
            "Track ID is empty");

        return;
    }

    m_yandexClient->setToken(
        m_auth->token());

    const QString path =
        "/tracks/" +
        trimmedTrackId +
        "/download-info?can_use_streaming=true";

    QNetworkReply *reply =
        m_yandexClient->get(path);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, trimmedTrackId]() {

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
                    "Invalid stream information response");

                reply->deleteLater();
                return;
            }

            const QJsonObject root =
                document.object();

            const QJsonArray result =
                root.value("result").toArray();

            if (result.isEmpty()) {

                emit errorOccurred(
                    "No streaming variants available");

                reply->deleteLater();
                return;
            }

            QList<TrackStreamInfo> streams;

            for (const QJsonValue &value : result) {

                const QJsonObject object =
                    value.toObject();

                TrackStreamInfo stream;

                stream.codec =
                    object.value("codec")
                        .toString();

                stream.downloadInfoUrl =
                    object.value("downloadInfoUrl")
                        .toString();

                stream.gain =
                    object.value("gain")
                        .toBool();

                stream.preview =
                    object.value("preview")
                        .toBool();

                stream.direct =
                    object.value("direct")
                        .toBool();

                stream.bitrateInKbps =
                    object.value("bitrateInKbps")
                        .toInt();

                if (!stream.downloadInfoUrl.isEmpty()) {
                    streams.append(stream);
                }
            }

            if (streams.isEmpty()) {

                emit errorOccurred(
                    "No valid streaming variants available");

                reply->deleteLater();
                return;
            }

            emit streamInfoReceived(
                streams);

            const TrackStreamInfo bestStream =
                selectBestStream(streams);

            if (bestStream.downloadInfoUrl.isEmpty()) {

                emit errorOccurred(
                    "Unable to select streaming variant");

                reply->deleteLater();
                return;
            }

            resolveStream(
                trimmedTrackId,
                bestStream);

            reply->deleteLater();
        });
}

TrackStreamInfo TrackService::selectBestStream(
    const QList<TrackStreamInfo> &streams) const
{
    TrackStreamInfo bestStream;

    for (const TrackStreamInfo &stream : streams) {

        if (stream.preview) {
            continue;
        }

        if (stream.codec.isEmpty()) {
            continue;
        }

        if (bestStream.downloadInfoUrl.isEmpty()) {
            bestStream = stream;
            continue;
        }

        if (stream.direct &&
            !bestStream.direct) {

            bestStream = stream;
            continue;
        }

        if (stream.bitrateInKbps >
            bestStream.bitrateInKbps) {

            bestStream = stream;
        }
    }

    return bestStream;
}

void TrackService::resolveStream(
    const QString &trackId,
    const TrackStreamInfo &stream)
{
    if (stream.downloadInfoUrl.isEmpty()) {

        emit errorOccurred(
            "Download info URL is empty");

        return;
    }

    QNetworkReply *reply =
        m_yandexClient->get(
            stream.downloadInfoUrl);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, trackId]() {

            const QByteArray data =
                reply->readAll();

            if (reply->error() !=
                QNetworkReply::NoError) {

                emit errorOccurred(
                    reply->errorString());

                reply->deleteLater();
                return;
            }

            if (data.isEmpty()) {

                emit errorOccurred(
                    "Empty download-info response");

                reply->deleteLater();
                return;
            }

            QXmlStreamReader xml(data);

            QString host;
            QString path;
            QString ts;
            QString signature;

            while (!xml.atEnd()) {

                xml.readNext();

                if (!xml.isStartElement()) {
                    continue;
                }

                const QString elementName =
                    xml.name().toString();

                if (elementName == "host") {

                    host =
                        xml.readElementText();

                } else if (elementName == "path") {

                    path =
                        xml.readElementText();

                } else if (elementName == "ts") {

                    ts =
                        xml.readElementText();

                } else if (elementName == "s") {

                    signature =
                        xml.readElementText();
                }
            }

            if (xml.hasError()) {

                emit errorOccurred(
                    "Invalid download-info XML");

                reply->deleteLater();
                return;
            }

            host = host.trimmed();
            path = path.trimmed();
            ts = ts.trimmed();
            signature = signature.trimmed();

            if (host.isEmpty() ||
                path.isEmpty() ||
                ts.isEmpty() ||
                signature.isEmpty()) {

                emit errorOccurred(
                    "Incomplete download-info response");

                reply->deleteLater();
                return;
            }

            QString pathForHash = path;

            if (pathForHash.startsWith('/')) {
                pathForHash.remove(0, 1);
            }

            const QByteArray hashSource =
                QByteArray(DownloadInfoSalt) +
                pathForHash.toUtf8() +
                signature.toUtf8();

            const QByteArray hash =
                QCryptographicHash::hash(
                    hashSource,
                    QCryptographicHash::Md5);

            const QString sign =
                QString::fromLatin1(
                    hash.toHex());

            const QString streamUrl =
                QString(
                    "https://%1/get-mp3/%2/%3%4")
                    .arg(host)
                    .arg(sign)
                    .arg(ts)
                    .arg(path);

            emit streamUrlReceived(
                trackId,
                streamUrl);

            reply->deleteLater();
        });
}