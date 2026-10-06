#pragma once

#include "../Models/Track.h"
#include <QObject>
#include <QPointer>

class QNetworkAccessManager;
class QNetworkReply;
class TrackService;

// Скачивает трек в «Downloads» с метаданными (теги) и вложенным
// артворком.
//
// Поток:
//   1. TrackService::loadDownloadUrl(trackId) → downloadUrlReceived.
//   2. Скачиваем аудио во временный файл (атомарно, чтобы не
//      оставлять битые файлы при обрыве).
//   3. Скачиваем обложку (coverUri).
//   4. TagWriter записывает теги и картинку.
//   5. Переносим во «Загрузки» (или каталог по умолчанию).
//
// Состояние выставляется через enum DownloadState и Q_PROPERTY,
// чтобы QML мог показывать прогресс и статус кнопки.
class DownloadService : public QObject {
  Q_OBJECT

  // Состояние скачивания (Idle/Downloading/Done/Error) для UI.
  Q_PROPERTY(int state READ state NOTIFY stateChanged)
  Q_PROPERTY(double progress READ progress NOTIFY stateChanged)

public:
  enum DownloadState {
    Idle = 0,     // Не скачиваем ничего.
    Downloading,  // Активно качаем аудио/обложку.
    Done,         // Успешно завершено.
    Error         // Не удалось скачать/записать.
  };

  Q_ENUM(DownloadState)

  explicit DownloadService(
      TrackService *trackService,
      QObject *parent = nullptr);

  DownloadState state() const;

  double progress() const;

  // true, когда текущий трек уже скачан (как признак «установлено»).
  bool isCurrentDownloaded() const;

  const Track &pendingTrack() const;

  Q_INVOKABLE void downloadCurrentTrack(const Track &track);

  void reset();

signals:
  void stateChanged();
  void trackChanged();
  void statusChanged(const QString &message);

private:
  void onDownloadUrl(
      const QString &trackId,
      const QString &url,
      const QString &codec);

  void downloadAudio(
      const Track &track,
      const QString &url,
      const QString &codec);

  void downloadCover(const Track &track);

  // Записывает теги и переносит аудио во «Загрузки».
  void writeTags(
      const Track &track,
      const QByteArray &coverData,
      const QString &mime = QStringLiteral("image/jpeg"));

  QString buildDestinationPath(
      const Track &track,
      const QString &codec) const;

  void finishWithError(const QString &message);

  void setState(DownloadState state);

  TrackService *m_trackService = nullptr;
  QNetworkAccessManager *m_network = nullptr;
  QPointer<QNetworkReply> m_activeReply;

  Track m_pendingTrack;
  QString m_pendingCodec;
  QByteArray m_audioData;
  DownloadState m_state = Idle;
  double m_progress = 0.0;

  QString m_startedTrackId;
};