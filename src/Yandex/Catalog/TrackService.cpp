#include "TrackService.h"

#include "../Auth/YandexAuth.h"
#include "../../Core/AudioQualityController.h"
#include "../Parsers.h"
#include "../YandexClient.h"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageAuthenticationCode>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QStringList>
#include <QUrlQuery>
#include <QXmlStreamReader>

#include <functional>

namespace {
constexpr auto DownloadInfoSalt = "XGRlBW9FXlekgbPrRHuSiA";
constexpr auto FileInfoSecret = "kzqU4XhfCaY6B6JTHODeq5";
} // namespace

// TODO(YM-2244): вынести общую проверку trackId (trim + непустой) в YandexServiceBase.

TrackService::TrackService(
    YandexAuth *auth,
    QObject *parent)
    : YandexServiceBase(auth, parent),
      m_publicNetwork(new QNetworkAccessManager(this)) {
  connect(
      m_yandexClient,
      &YandexClient::playbackReported,
      this,
      [this](bool ok) {
        emit playbackReported(ok);
      });
}

void TrackService::loadStreamInfo(
    const QString &trackId) {
  loadStreamInfo(trackId, QStringLiteral("high"));
}

void TrackService::loadStreamInfo(
    const QString &trackId,
    const QString &quality) {
  const QString trimmedTrackId = trackId.trimmed();

  // Используем классический download-info (он надёжно отдаёт URL и
  // codec/bitrate). Прямой /get-file-info возвращает 400 для многих
  // треков, поэтому не задействуем его для обычного стриминга.
  requestDownloadInfo(
      trimmedTrackId,
      {},
      [this, trimmedTrackId](
          const QString &url,
          const QString &codec,
          int bitrateKbps) {
        emit streamUrlReceived(trimmedTrackId, url);
        emit streamQualityReceived(trimmedTrackId, codec, bitrateKbps);
      });
}

void TrackService::loadDownloadUrl(
    const QString &trackId) {
  const QString trimmedTrackId =
      trackId.trimmed();

  // Для сохранения отдаём предпочтение MP3 (максимальная
  // совместимость тегов и плееров), если такой вариант есть.
  requestDownloadInfo(
      trimmedTrackId,
      QStringLiteral("mp3"),
      [this, trimmedTrackId](
          const QString &url,
          const QString &codec,
          int) {
        emit downloadUrlReceived(
            trimmedTrackId,
            url,
            codec);
      });
}

// Общая двухшаговая схема получения подписанного URL стрима:
//   1) GET /tracks/{id}/download-info
//   2) резолв подписи (host/path/ts/s) и сборка get-mp3-URL
// onResolved вызывается с финальным URL, кодеком и битрейтом.
void TrackService::requestDownloadInfo(
    const QString &trackId,
    const QString &preferredCodec,
    const std::function<void(const QString &url, const QString &codec, int bitrateKbps)>
        &onResolved) {
  if (!ensureAuthenticated()) {
    emit errorOccurred(
        "Токен Яндекс Музыки не установлен");
    return;
  }

  const QString trimmedTrackId =
      trackId.trimmed();

  if (trimmedTrackId.isEmpty()) {
    emit errorOccurred("Track ID is empty");
    return;
  }

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
      [this, reply, trimmedTrackId, preferredCodec, onResolved]() {
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
          const QJsonObject obj =
              value.toObject();

          TrackStreamInfo stream;

          stream.codec =
              obj.value("codec").toString();

          stream.downloadInfoUrl =
              obj.value("downloadInfoUrl").toString();

          stream.gain =
              obj.value("gain").toBool();

          stream.preview =
              obj.value("preview").toBool();

          stream.direct =
              obj.value("direct").toBool();

          stream.bitrateInKbps =
              obj.value("bitrateInKbps").toInt();

          if (!stream.downloadInfoUrl.isEmpty())
            streams.append(stream);
        }

        if (streams.isEmpty()) {
          emit errorOccurred(
              "No valid streaming variants available");

          reply->deleteLater();
          return;
        }

        emit streamInfoReceived(streams);

        TrackStreamInfo bestStream;

        if (!preferredCodec.isEmpty()) {
          // Сначала ищем непревьюшный вариант нужного кодека,
          // дальше — с максимальным битрейтом.
          for (const TrackStreamInfo &stream : streams) {
            if (!stream.preview &&
                stream.codec == preferredCodec) {
              if (bestStream.downloadInfoUrl.isEmpty() ||
                  stream.bitrateInKbps > bestStream.bitrateInKbps) {
                bestStream = stream;
              }
            }
          }

          // Если нужного кодека нет — берём дефолтный лучший.
          if (bestStream.downloadInfoUrl.isEmpty())
            bestStream = selectBestStream(streams);
        } else {
          bestStream = selectBestStream(streams);
        }

        if (bestStream.downloadInfoUrl.isEmpty()) {
          emit errorOccurred(
              "Unable to select streaming variant");

          reply->deleteLater();
          return;
        }

        resolveStream(
            trimmedTrackId,
            bestStream,
            [onResolved, bestStream](
                const QString &url) {
              if (onResolved)
                onResolved(url, bestStream.codec, bestStream.bitrateInKbps);
            });

        reply->deleteLater();
      });
}

TrackStreamInfo TrackService::selectBestStream(
    const QList<TrackStreamInfo> &streams) const {
  TrackStreamInfo bestStream;

  for (const TrackStreamInfo &stream : streams) {
    if (stream.preview)
      continue;

    if (stream.codec.isEmpty())
      continue;

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
    const TrackStreamInfo &stream,
    const std::function<void(const QString &url)> &onResolved) {
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
      [this, reply, trackId, onResolved]() {
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

          if (!xml.isStartElement())
            continue;

          const QString elementName =
              xml.name().toString();

          if (elementName == "host") {
            host = xml.readElementText();
          } else if (elementName == "path") {
            path = xml.readElementText();
          } else if (elementName == "ts") {
            ts = xml.readElementText();
          } else if (elementName == "s") {
            signature = xml.readElementText();
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

        if (pathForHash.startsWith('/'))
          pathForHash.remove(0, 1);

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
            QStringLiteral(
                "https://%1/get-mp3/%2/%3%4")
                .arg(host)
                .arg(sign)
                .arg(ts)
                .arg(path);

        emit streamUrlReceived(
            trackId,
            streamUrl);

        if (onResolved)
          onResolved(streamUrl);

        reply->deleteLater();
      });
}

void TrackService::loadSupplementary(
    const QString &trackId) {
  if (!ensureAuthenticated()) {
    emit errorOccurred(
        "Токен Яндекс Музыки не установлен");
    return;
  }

  const QString trimmedTrackId =
      trackId.trimmed();

  if (trimmedTrackId.isEmpty()) {
    emit errorOccurred("Track ID is empty");
    return;
  }

  const QString path =
      "/tracks/" +
      trimmedTrackId +
      "/supplement";

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
          reply->deleteLater();
          return;
        }

        const QJsonObject result =
            unwrapResult(document);

        const QJsonValue lyricsVal =
            result.value("lyrics");

        QJsonArray lyricsArray;

        if (lyricsVal.isArray()) {
          lyricsArray = lyricsVal.toArray();
        } else if (lyricsVal.isObject()) {
          const QJsonObject lyricsObj =
              lyricsVal.toObject();

          const QJsonValue inner =
              lyricsObj.value("lyrics");

          if (inner.isArray()) {
            lyricsArray = inner.toArray();
          } else if (inner.isObject()) {
            lyricsArray.append(
                inner.toObject());
          } else {
            lyricsArray.append(
                lyricsObj);
          }
        } else if (lyricsVal.isString()) {
          TrackSupplementary supplementary;

          supplementary.trackId =
              trimmedTrackId;

          supplementary.fullText =
              lyricsVal.toString().trimmed();

          reply->deleteLater();

          emit supplementReceived(
              supplementary);

          return;
        }

        TrackSupplementary supplementary;

        supplementary.trackId =
            trimmedTrackId;

        for (const QJsonValue &lyricsValue :
             lyricsArray) {
          if (!lyricsValue.isObject())
            continue;

          const QJsonObject lyrics =
              lyricsValue.toObject();

          const QString fullLyrics =
              lyrics.value("fullLyrics")
                  .toString()
                  .trimmed();

          if (!fullLyrics.isEmpty())
            supplementary.fullText =
                fullLyrics;

          const QJsonArray lines =
              lyrics.value("lines")
                  .toArray();

          for (const QJsonValue &lineValue :
               lines) {
            if (!lineValue.isObject())
              continue;

            const QJsonObject line =
                lineValue.toObject();

            LyricLine lyricLine;

            lyricLine.timestampMs =
                line.value("timestamp")
                    .toVariant()
                    .toLongLong();

            lyricLine.text =
                line.value("line")
                    .toString()
                    .trimmed();

            if (lyricLine.text.isEmpty()) {
              lyricLine.text =
                  line.value("text")
                      .toString()
                      .trimmed();
            }

            if (!lyricLine.text.isEmpty())
              supplementary.lines.append(
                  lyricLine);
          }
        }

        reply->deleteLater();

        if (supplementary.fullText.isEmpty() &&
            supplementary.lines.isEmpty()) {
          loadTrackLyrics(trimmedTrackId);
          return;
        }

        emit supplementReceived(
            supplementary);
      });
}

void TrackService::loadTrackLyrics(
    const QString &trackId) {
  if (!ensureAuthenticated()) {
    emit errorOccurred(
        "Токен Яндекс Музыки не установлен");
    return;
  }

  const QString trimmedTrackId =
      trackId.trimmed();

  if (trimmedTrackId.isEmpty()) {
    emit errorOccurred("Track ID is empty");
    return;
  }

  const QString path =
      "/tracks/" +
      trimmedTrackId +
      "/lyrics";

  QNetworkReply *reply =
      m_yandexClient->get(path);

  if (reply == nullptr) {
    emit errorOccurred(
        "Не удалось получить текст трека");
    return;
  }

  connect(
      reply,
      &QNetworkReply::finished,
      this,
      [this, reply, trimmedTrackId]() {
        const QByteArray data =
            reply->readAll();

        reply->deleteLater();

        if (reply->error() !=
            QNetworkReply::NoError) {
          emit errorOccurred(
              reply->errorString());
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
              "Некорректный ответ текста трека");
          return;
        }

        const QJsonObject result =
            unwrapResult(document);

        const QString downloadUrl =
            result.value("downloadUrl")
                .toString()
                .trimmed();

        if (downloadUrl.isEmpty()) {
          emit errorOccurred(
              "Синхронизированный текст недоступен");
          return;
        }

        QNetworkReply *lrcReply =
            m_yandexClient->get(downloadUrl);

        if (lrcReply == nullptr) {
          emit errorOccurred(
              "Не удалось скачать текст трека");
          return;
        }

        connect(
            lrcReply,
            &QNetworkReply::finished,
            this,
            [this, lrcReply, trimmedTrackId]() {
              const QByteArray lrcData =
                  lrcReply->readAll();

              lrcReply->deleteLater();

              if (lrcReply->error() !=
                  QNetworkReply::NoError) {
                emit errorOccurred(
                    lrcReply->errorString());
                return;
              }

              TrackSupplementary supplementary;

              supplementary.trackId =
                  trimmedTrackId;

              parseLrc(
                  QString::fromUtf8(lrcData),
                  supplementary);

              if (supplementary.fullText.isEmpty() &&
                  supplementary.lines.isEmpty()) {
                emit errorOccurred(
                    "Текст для трека не найден");
                return;
              }

              emit supplementReceived(
                  supplementary);
            });
      });
}

// Парсит LRC: [mm:ss.xx] строка, попутно сохраняя
// полный текст без таймингов в fullText.
void TrackService::parseLrc(
    const QString &lrcText,
    TrackSupplementary &out) const {
  const QStringList rawLines =
      lrcText.split('\n');

  QStringList plainLines;

  // [mm:ss.xx] или [mm:ss.xxx]
  static const QRegularExpression timeRe(
      "^\\[(\\d{1,2}):(\\d{1,2})(?:\\.(\\d{1,3}))?\\]\\s*(.*)$");

  for (const QString &rawLine : rawLines) {
    const QString line =
        rawLine.trimmed();

    if (line.isEmpty())
      continue;

    const QRegularExpressionMatch match =
        timeRe.match(line);

    if (!match.hasMatch()) {
      if (!line.startsWith('[')) {
        if (!plainLines.isEmpty())
          plainLines.append(line);
      }

      continue;
    }

    const int minutes =
        match.captured(1).toInt();

    const int seconds =
        match.captured(2).toInt();

    int millis = 0;

    const QString frac =
        match.captured(3);

    if (!frac.isEmpty()) {
      QString padded = frac;

      while (padded.size() < 3)
        padded.append('0');

      millis =
          padded.left(3).toInt();
    }

    const QString text =
        match.captured(4).trimmed();

    if (text.isEmpty())
      continue;

    LyricLine lyricLine;

    lyricLine.timestampMs =
        static_cast<qint64>(minutes) * 60000 +
        static_cast<qint64>(seconds) * 1000 +
        millis;

    lyricLine.text = text;

    out.lines.append(lyricLine);
    plainLines.append(text);
  }

  if (!plainLines.isEmpty()) {
    out.fullText =
        plainLines.join('\n').trimmed();
  }
}

void TrackService::reportPlayback(
    const QString &trackId,
    const QString &albumId,
    const QString &uid,
    int trackLengthSeconds,
    int playedSeconds,
    int endPositionSeconds) {
  if (!ensureAuthenticated())
    return;

  if (trackId.trimmed().isEmpty() ||
      uid.trimmed().isEmpty()) {
    return;
  }

  m_yandexClient->reportPlayback(
      trackId.trimmed(),
      albumId.trimmed(),
      uid.trimmed(),
      trackLengthSeconds,
      playedSeconds,
      endPositionSeconds);
}

void TrackService::loadSimilarTracks(
    const QString &trackId) {
  if (!ensureAuthenticated()) {
    emit errorOccurred(
        "Токен Яндекс Музыки не установлен");
    return;
  }

  const QString trimmedTrackId =
      trackId.trimmed();

  if (trimmedTrackId.isEmpty()) {
    emit errorOccurred("Track ID is empty");
    return;
  }

  const QString path =
      "/tracks/" +
      trimmedTrackId +
      "/similar";

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
              "Invalid similar tracks response");

          reply->deleteLater();
          return;
        }

        const QJsonObject root =
            document.object();

        const QJsonValue resultVal =
            root.value("result");

        QJsonArray trackArray;

        if (resultVal.isArray()) {
          trackArray =
              resultVal.toArray();
        } else if (resultVal.isObject()) {
          const QJsonObject resultObj =
              resultVal.toObject();

          if (resultObj.contains(
                  "similarTracks")) {
            trackArray =
                resultObj.value(
                    "similarTracks")
                    .toArray();
          } else {
            trackArray =
                resultObj.value("tracks")
                    .toArray();
          }
        }

        const QList<Track> tracks =
            parseTrackArray(trackArray);

        reply->deleteLater();

        emit similarTracksReceived(
            tracks);
      });
}
QString TrackService::lyricsSign(qint64 ts, const QString &trackId, const QString &format) {
  // Подпись для /tracks/{id}/lyrics — как в клиентах Яндекса.
  const QByteArray message =
      QByteArray::number(ts) + trackId.toUtf8() + format.toUtf8() +
      QByteArray(FileInfoSecret);
  QMessageAuthenticationCode hmac(QCryptographicHash::Sha256, QByteArray(FileInfoSecret));
  hmac.addData(message);
  return hmac.result().toBase64(QByteArray::OmitTrailingEquals);
}

void TrackService::requestYandexLrc(
    const QString &trackId,
    const std::function<void(const QString &lrcText)> &onOk,
    const std::function<void()> &onFail) {
  if (!ensureAuthenticated()) {
    if (onFail) onFail();
    return;
  }
  const QString id = trackId.trimmed();
  if (id.isEmpty()) {
    if (onFail) onFail();
    return;
  }

  const QString format = QStringLiteral("LRC");
  const qint64 ts = QDateTime::currentSecsSinceEpoch();
  const QString sign = lyricsSign(ts, id, format);

  QUrlQuery query;
  query.addQueryItem("format", format);
  query.addQueryItem("timeStamp", QString::number(ts));
  query.addQueryItem("sign", sign);

  const QString path = QStringLiteral("/tracks/%1/lyrics?%2")
                           .arg(id, query.toString(QUrl::FullyEncoded));
  QNetworkReply *reply = m_yandexClient->get(path);
  if (reply == nullptr) {
    if (onFail) onFail();
    return;
  }

  connect(reply, &QNetworkReply::finished, this,
          [this, reply, id, onOk, onFail]() {
            const QByteArray data = reply->readAll();
            const bool ok = (reply->error() == QNetworkReply::NoError);
            reply->deleteLater();
            if (!ok || data.isEmpty()) {
              if (onFail) onFail();
              return;
            }
            QJsonParseError parseError;
            const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
            if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
              if (onFail) onFail();
              return;
            }
            const QJsonObject result = unwrapResult(doc);
            const QString lrc = result.value("lyrics").toString().trimmed();
            const QString full = result.value("fullLyrics").toString().trimmed();
            const QString text = !lrc.isEmpty() ? lrc : full;
            if (text.isEmpty()) {
              if (onFail) onFail();
              return;
            }
            if (onOk) onOk(text);
          });
}

void TrackService::requestLrcLib(
    const QString &title, const QString &artist, int durationSec,
    const std::function<void(const QString &lrcText)> &onOk,
    const std::function<void()> &onFail) {
  QUrl url(QStringLiteral("https://lrclib.net/api/get"));
  QUrlQuery query;
  if (!artist.isEmpty()) query.addQueryItem("artist_name", artist);
  if (!title.isEmpty()) query.addQueryItem("track_name", title);
  if (durationSec > 0) query.addQueryItem("duration", QString::number(durationSec));
  url.setQuery(query);

  QNetworkRequest request(url);
  request.setRawHeader("Lrclib-Client", "YaMusic/1.0 (Qt)");
  request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
  QNetworkReply *reply = m_publicNetwork->get(request);

  connect(reply, &QNetworkReply::finished, this,
          [this, reply, onOk, onFail]() {
            const QByteArray data = reply->readAll();
            const bool ok = (reply->error() == QNetworkReply::NoError);
            reply->deleteLater();
            if (!ok || data.isEmpty()) {
              if (onFail) onFail();
              return;
            }
            QJsonParseError parseError;
            const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
            if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
              if (onFail) onFail();
              return;
            }
            const QJsonObject obj = doc.object();
            const QString synced = obj.value("syncedLyrics").toString().trimmed();
            const QString plain = obj.value("plainLyrics").toString().trimmed();
            const QString text = !synced.isEmpty() ? synced : plain;
            if (text.isEmpty()) {
              if (onFail) onFail();
              return;
            }
            if (onOk) onOk(text);
          });
}

void TrackService::emitSupplement(const TrackSupplementary &supplement) {
  emit supplementReceived(supplement);
}

void TrackService::loadSyncLyrics(const QString &trackId, const QString &title,
                                  const QString &artist) {
  const QString id = trackId.trimmed();
  if (id.isEmpty()) {
    emit errorOccurred("Track ID is empty");
    return;
  }

  auto makeSupplement = [this, id](const QString &lrcText) {
    TrackSupplementary supplement;
    supplement.trackId = id;
    parseLrc(lrcText, supplement);
    if (supplement.fullText.isEmpty()) {
      supplement.fullText = lrcText.trimmed();
    }
    emitSupplement(supplement);
  };

  requestYandexLrc(
      id,
      makeSupplement,
      [this, id, title, artist, makeSupplement]() {
        // Яндекса нет — пробуем публичную базу LRCLIB.
        requestLrcLib(title, artist, 0, makeSupplement, [this, id]() {
          emit errorOccurred("Текст для трека не найден");
        });
      });
}
