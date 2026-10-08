#pragma once

#include "../../Models/Track.h"
#include "../YandexServiceBase.h"
#include <QList>
#include <QRegularExpression>
#include <QString>
#include <functional>

class YandexClient;

struct TrackStreamInfo {
  QString codec;
  QString downloadInfoUrl;
  bool gain = false;
  bool preview = false;
  bool direct = false;
  int bitrateInKbps = 0;
};

class TrackService : public YandexServiceBase {
  Q_OBJECT

public:
  explicit TrackService(YandexAuth *auth, QObject *parent = nullptr);

  void loadStreamInfo(const QString &trackId);

  // Как loadStreamInfo, но с явным выбором качества:
  // "low" / "normal" / "high" / "lossless".
  void loadStreamInfo(const QString &trackId, const QString &quality);

  // Получает прямой URL для скачивания трека и его кодек.
  // В отличие от loadStreamInfo (который выбирает лучший
  // стрим для воспроизведения), здесь результат отдаётся
  // сигналом downloadUrlReceived(trackId, url, codec) —
  // например, для сохранения файла с тегами и артворком.
  void loadDownloadUrl(const QString &trackId);

  // Загружает дополнительную информацию о треке
  // (текст песни) через /tracks/{id}/supplement.
  void loadSupplementary(const QString &trackId);

  // Загружает синхронизированный текст через
  // /tracks/{id}/lyrics (LRC по downloadUrl).
  void loadTrackLyrics(const QString &trackId);

  void loadSimilarTracks(const QString &trackId);

  // Отправляет факт прослушивания трека на сервер
  // (POST /play-audio), чтобы трек попал в «Недавно
  // прослушаны» и рекомендации.
  void reportPlayback(const QString &trackId, const QString &albumId, const QString &uid,
                      int trackLengthSeconds, int playedSeconds, int endPositionSeconds);

signals:
  void streamInfoReceived(const QList<TrackStreamInfo> &streams);

  void streamUrlReceived(const QString &trackId, const QString &url);

  // Актуальный кодек и битрейт полученного стрима (для отображения
  // качества при воспроизведении; пустые поля — если не определено).
  void streamQualityReceived(
      const QString &trackId,
      const QString &codec,
      int bitrateInKbps);

  // Отдаёт сигнатурный URL прямого скачивания и кодек
  // выбранного стрима (mp3/aac/flac). Используется для
  // сохранения файла с метаданными и вложенным артворком.
  void downloadUrlReceived(
      const QString &trackId,
      const QString &url,
      const QString &codec);

  void supplementReceived(const TrackSupplementary &supplement);

  void similarTracksReceived(const QList<Track> &tracks);

  void errorOccurred(const QString &message);

  // true — сервер принял факт прослушивания.
  void playbackReported(bool ok);

private:
  TrackStreamInfo selectBestStream(const QList<TrackStreamInfo> &streams) const;

  void resolveStream(
      const QString &trackId,
      const TrackStreamInfo &stream,
      const std::function<void(const QString &url)> &onResolved = {});

  // Новый прямой путь получения URL трека: GET /get-file-info со
  // HMAC-SHA256-подписью. Отдаёт сразу готовый к стримингу URL
  // (без второго шага к CDN). onResolved(url, codec, bitrateKbps)
  // вызывается при успехе, onFallback — если все попытки неудачны.
  void loadStreamFileInfo(
      const QString &trackId,
      const QString &quality,
      const std::function<void(const QString &url, const QString &codec, int bitrateKbps)>
          &onResolved,
      const std::function<void()> &onFallback);

  // HMAC-SHA256 подпись для /get-file-info.
  static QString fileInfoSign(
      qint64 ts,
      const QString &trackId,
      const QString &quality,
      const QString &codecsDelim,
      const QString &transports);

  // Общая логика запроса /download-info: выбирает лучший
  // стрим по выбранному кодеку/битрейту и резолвит подписанный
  // URL. Если preferredCodec непустой, отдаётся именно он.
  void requestDownloadInfo(
      const QString &trackId,
      const QString &preferredCodec,
      const std::function<void(const QString &url, const QString &codec)> &onResolved);

  void parseLrc(const QString &lrcText, TrackSupplementary &out) const;
};