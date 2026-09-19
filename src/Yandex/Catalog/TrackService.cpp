#include "TrackService.h"
#include "../Auth/YandexAuth.h"
#include "../Parsers.h"
#include "../YandexClient.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QXmlStreamReader>

namespace {
constexpr auto DownloadInfoSalt = "XGRlBW9FXlekgbPrRHuSiA";
} // namespace

// TODO(YM-2244): вынести общую проверку trackId (trim + непустой) в YandexServiceBase.

TrackService::TrackService(YandexAuth *auth, QObject *parent) : YandexServiceBase(auth, parent) {
  connect(m_yandexClient, &YandexClient::playbackReported, this,
          [this](bool ok) { emit playbackReported(ok); });
}

void TrackService::loadStreamInfo(const QString &trackId) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }
  const QString trimmedTrackId = trackId.trimmed();
  if (trimmedTrackId.isEmpty()) {
    emit errorOccurred("Track ID is empty");
    return;
  }
  const QString path = "/tracks/" + trimmedTrackId + "/download-info?can_use_streaming=true";
  QNetworkReply *reply = m_yandexClient->get(path);

  connect(reply, &QNetworkReply::finished, this, [this, reply, trimmedTrackId]() {
    const QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
      emit errorOccurred("Invalid stream information response");
      reply->deleteLater();
      return;
    }
    const QJsonObject root = document.object();
    const QJsonArray result = root.value("result").toArray();
    if (result.isEmpty()) {
      emit errorOccurred("No streaming variants available");
      reply->deleteLater();
      return;
    }
    QList<TrackStreamInfo> streams;

    for (const QJsonValue &value : result) {
      const QJsonObject obj = value.toObject();
      TrackStreamInfo stream;
      stream.codec = obj.value("codec").toString();
      stream.downloadInfoUrl = obj.value("downloadInfoUrl").toString();
      stream.gain = obj.value("gain").toBool();
      stream.preview = obj.value("preview").toBool();
      stream.direct = obj.value("direct").toBool();
      stream.bitrateInKbps = obj.value("bitrateInKbps").toInt();

      if (!stream.downloadInfoUrl.isEmpty()) {
        streams.append(stream);
      }
    }
    if (streams.isEmpty()) {
      emit errorOccurred("No valid streaming variants available");
      reply->deleteLater();
      return;
    }
    emit streamInfoReceived(streams);
    const TrackStreamInfo bestStream = selectBestStream(streams);
    if (bestStream.downloadInfoUrl.isEmpty()) {
      emit errorOccurred("Unable to select streaming variant");
      reply->deleteLater();
      return;
    }
    resolveStream(trimmedTrackId, bestStream);
    reply->deleteLater();
  });
}

TrackStreamInfo TrackService::selectBestStream(const QList<TrackStreamInfo> &streams) const {
  TrackStreamInfo bestStream;

  for (const TrackStreamInfo &stream : streams) {
    if (stream.preview) continue;
    if (stream.codec.isEmpty()) continue;
    if (bestStream.downloadInfoUrl.isEmpty()) {
      bestStream = stream;
      continue;
    }
    if (stream.direct && !bestStream.direct) {
      bestStream = stream;
      continue;
    }
    if (stream.bitrateInKbps > bestStream.bitrateInKbps) {
      bestStream = stream;
    }
  }
  return bestStream;
}

void TrackService::resolveStream(const QString &trackId, const TrackStreamInfo &stream) {
  if (stream.downloadInfoUrl.isEmpty()) {
    emit errorOccurred("Download info URL is empty");
    return;
  }
  QNetworkReply *reply = m_yandexClient->get(stream.downloadInfoUrl);

  connect(reply, &QNetworkReply::finished, this, [this, reply, trackId]() {
    const QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }
    if (data.isEmpty()) {
      emit errorOccurred("Empty download-info response");
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
      if (!xml.isStartElement()) continue;
      const QString elementName = xml.name().toString();

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
      emit errorOccurred("Invalid download-info XML");
      reply->deleteLater();
      return;
    }
    host = host.trimmed();
    path = path.trimmed();
    ts = ts.trimmed();
    signature = signature.trimmed();
    if (host.isEmpty() || path.isEmpty() || ts.isEmpty() || signature.isEmpty()) {
      emit errorOccurred("Incomplete download-info response");
      reply->deleteLater();
      return;
    }
    QString pathForHash = path;
    if (pathForHash.startsWith('/')) {
      pathForHash.remove(0, 1);
    }
    const QByteArray hashSource =
        QByteArray(DownloadInfoSalt) + pathForHash.toUtf8() + signature.toUtf8();
    const QByteArray hash = QCryptographicHash::hash(hashSource, QCryptographicHash::Md5);
    const QString sign = QString::fromLatin1(hash.toHex());
    const QString streamUrl =
        QStringLiteral("https://%1/get-mp3/%2/%3%4").arg(host).arg(sign).arg(ts).arg(path);
    emit streamUrlReceived(trackId, streamUrl);
    reply->deleteLater();
  });
}

void TrackService::loadSupplementary(const QString &trackId) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }
  const QString trimmedTrackId = trackId.trimmed();
  if (trimmedTrackId.isEmpty()) {
    emit errorOccurred("Track ID is empty");
    return;
  }
  const QString path = "/tracks/" + trimmedTrackId + "/supplement";
  QNetworkReply *reply = m_yandexClient->get(path);

  connect(reply, &QNetworkReply::finished, this, [this, reply, trimmedTrackId]() {
    const QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
      reply->deleteLater();
      return;
    }
    const QJsonObject result = unwrapResult(document);

    // Supplement поддерживает несколько вариантов текста:
    // fullLyrics и/или синхронизированные lines.
    const QJsonValue lyricsVal = result.value("lyrics");
    QJsonArray lyricsArray;

    if (lyricsVal.isArray()) {
      lyricsArray = lyricsVal.toArray();
    } else if (lyricsVal.isObject()) {
      // Иногда lyrics — объект с вложенным массивом
      const QJsonObject lyricsObj = lyricsVal.toObject();
      const QJsonValue inner = lyricsObj.value("lyrics");

      if (inner.isArray()) {
        lyricsArray = inner.toArray();
      } else if (inner.isObject()) {
        lyricsArray.append(inner.toObject());
      } else {
        lyricsArray.append(lyricsObj);
      }
    } else if (lyricsVal.isString()) {
      // Иногда приходит строкой
      TrackSupplementary supplementary;
      supplementary.trackId = trimmedTrackId;
      supplementary.fullText = lyricsVal.toString().trimmed();
      reply->deleteLater();
      emit supplementReceived(supplementary);
      return;
    }
    TrackSupplementary supplementary;
    supplementary.trackId = trimmedTrackId;

    for (const QJsonValue &lyricsValue : lyricsArray) {
      if (!lyricsValue.isObject()) continue;
      const QJsonObject lyrics = lyricsValue.toObject();
      const QString fullLyrics = lyrics.value("fullLyrics").toString().trimmed();

      if (!fullLyrics.isEmpty()) {
        supplementary.fullText = fullLyrics;
      }
      const QJsonArray lines = lyrics.value("lines").toArray();

      for (const QJsonValue &lineValue : lines) {
        if (!lineValue.isObject()) continue;
        const QJsonObject line = lineValue.toObject();
        LyricLine lyricLine;
        lyricLine.timestampMs = line.value("timestamp").toVariant().toLongLong();

        // Ключ строки в API — "line", но поддерживаем
        // и "text" на случай изменения формата.
        lyricLine.text = line.value("line").toString().trimmed();

        if (lyricLine.text.isEmpty()) {
          lyricLine.text = line.value("text").toString().trimmed();
        }
        if (!lyricLine.text.isEmpty()) {
          supplementary.lines.append(lyricLine);
        }
      }
    }
    reply->deleteLater();

    // Если через supplement текст не найден,
    // пробуем /tracks/{id}/lyrics (LRC).
    if (supplementary.fullText.isEmpty() && supplementary.lines.isEmpty()) {
      loadTrackLyrics(trimmedTrackId);
      return;
    }
    emit supplementReceived(supplementary);
  });
}

void TrackService::loadTrackLyrics(const QString &trackId) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }
  const QString trimmedTrackId = trackId.trimmed();
  if (trimmedTrackId.isEmpty()) {
    emit errorOccurred("Track ID is empty");
    return;
  }
  const QString path = "/tracks/" + trimmedTrackId + "/lyrics";
  QNetworkReply *reply = m_yandexClient->get(path);

  if (reply == nullptr) {
    emit errorOccurred("Не удалось получить текст трека");
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply, trimmedTrackId]() {
    const QByteArray data = reply->readAll();
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      return;
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
      emit errorOccurred("Некорректный ответ текста трека");
      return;
    }
    const QJsonObject result = unwrapResult(document);
    const QString downloadUrl = result.value("downloadUrl").toString().trimmed();
    if (downloadUrl.isEmpty()) {
      emit errorOccurred("Синхронизированный текст недоступен");
      return;
    }

    // Скачиваем сам LRC-файл
    QNetworkReply *lrcReply = m_yandexClient->get(downloadUrl);

    if (lrcReply == nullptr) {
      emit errorOccurred("Не удалось скачать текст трека");
      return;
    }

    connect(lrcReply, &QNetworkReply::finished, this, [this, lrcReply, trimmedTrackId]() {
      const QByteArray lrcData = lrcReply->readAll();
      lrcReply->deleteLater();
      if (lrcReply->error() != QNetworkReply::NoError) {
        emit errorOccurred(lrcReply->errorString());
        return;
      }
      TrackSupplementary supplementary;
      supplementary.trackId = trimmedTrackId;
      parseLrc(QString::fromUtf8(lrcData), supplementary);

      if (supplementary.fullText.isEmpty() && supplementary.lines.isEmpty()) {
        emit errorOccurred("Текст для трека не найден");
        return;
      }
      emit supplementReceived(supplementary);
    });
  });
}

// Парсит LRC: [mm:ss.xx] строка, попутно сохраняя
// полный текст без таймингов в fullText.
void TrackService::parseLrc(const QString &lrcText, TrackSupplementary &out) const {
  const QStringList rawLines = lrcText.split('\n');
  QStringList plainLines;

  // [mm:ss.xx] или [mm:ss.xxx]
  static const QRegularExpression timeRe("^\\[(\\d{1,2}):(\\d{1,2})(?:\\.(\\d{1,3}))?\\]\\s*(.*)$");

  for (const QString &rawLine : rawLines) {
    const QString line = rawLine.trimmed();
    if (line.isEmpty()) continue;
    const QRegularExpressionMatch match = timeRe.match(line);

    if (!match.hasMatch()) {
      // Строка без тайминга — просто текст
      if (!line.startsWith('[')) {
        if (!plainLines.isEmpty()) plainLines.append(line);
      }
      continue;
    }
    const int minutes = match.captured(1).toInt();
    const int seconds = match.captured(2).toInt();
    int millis = 0;
    const QString frac = match.captured(3);

    if (!frac.isEmpty()) {
      // xxx → мс, xx → десятки мс (дополняем до 3 цифр)
      QString padded = frac;
      while (padded.size() < 3) {
        padded.append('0');
      }
      millis = padded.left(3).toInt();
    }
    const QString text = match.captured(4).trimmed();
    if (text.isEmpty()) continue;
    LyricLine lyricLine;
    lyricLine.timestampMs =
        static_cast<qint64>(minutes) * 60000 + static_cast<qint64>(seconds) * 1000 + millis;
    lyricLine.text = text;
    out.lines.append(lyricLine);
    plainLines.append(text);
  }

  if (!plainLines.isEmpty()) {
    out.fullText = plainLines.join('\n').trimmed();
  }
}

void TrackService::reportPlayback(const QString &trackId, const QString &albumId,
                                  const QString &uid, int trackLengthSeconds,
                                  int playedSeconds, int endPositionSeconds) {
  if (!ensureAuthenticated()) {
    return;
  }
  if (trackId.trimmed().isEmpty() || uid.trimmed().isEmpty()) {
    return;
  }
  m_yandexClient->reportPlayback(trackId.trimmed(), albumId.trimmed(), uid.trimmed(),
                                 trackLengthSeconds, playedSeconds, endPositionSeconds);
}

void TrackService::loadSimilarTracks(const QString &trackId) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }
  const QString trimmedTrackId = trackId.trimmed();
  if (trimmedTrackId.isEmpty()) {
    emit errorOccurred("Track ID is empty");
    return;
  }
  const QString path = "/tracks/" + trimmedTrackId + "/similar";
  QNetworkReply *reply = m_yandexClient->get(path);

  connect(reply, &QNetworkReply::finished, this, [this, reply]() {
    const QByteArray data = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
      emit errorOccurred(reply->errorString());
      reply->deleteLater();
      return;
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
      emit errorOccurred("Invalid similar tracks response");
      reply->deleteLater();
      return;
    }
    const QJsonObject root = document.object();
    const QJsonValue resultVal = root.value("result");
    QJsonArray trackArray;

    if (resultVal.isArray()) {
      trackArray = resultVal.toArray();
    } else if (resultVal.isObject()) {
      const QJsonObject resultObj = resultVal.toObject();
      if (resultObj.contains("similarTracks")) {
        trackArray = resultObj.value("similarTracks").toArray();
      } else {
        trackArray = resultObj.value("tracks").toArray();
      }
    }
    const QList<Track> tracks = parseTrackArray(trackArray);
    reply->deleteLater();
    emit similarTracksReceived(tracks);
  });
}