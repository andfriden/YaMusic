#pragma once

#include "../Models/Track.h"
#include <QList>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QString>
#include <QTimer>

class QNetworkAccessManager;
class QNetworkReply;
class TrackCache;
class TrackService;

namespace Playback {

// Держит в кэше треки, которые вот-вот заиграют.
//
// Окно задаёт владелец — очередь после играющего трека. Качается по
// одному, строго в порядке очереди: докачка не должна отбирать сеть у
// трека, который слушают. Трек, ушедший из окна, отменяется; уже готовый
// файл остаётся в кэше до вытеснения по лимиту.
//
// Неудачи молчаливы: не скачалось — трек заиграет стримом, как играл бы
// без кэша, а в консоль ничего пугающего не попадёт.
class Preloader : public QObject {
  Q_OBJECT

public:
  explicit Preloader(TrackCache *cache, TrackService *trackService, QObject *parent = nullptr);

  void setEnabled(bool enabled);
  bool isEnabled() const;

  // Сколько треков вперёд держать готовыми (не считая играющего).
  void setAheadCount(int count);
  int aheadCount() const;

  // Треки после играющего, в порядке воспроизведения.
  void setUpcoming(const QList<Track> &tracks);

  // Путь готового файла трека или пустая строка.
  QString cachedPath(const Track &track) const;

  // Состояние окна для интерфейса.
  int windowCount() const;
  int readyCount() const;

signals:
  void readyChanged();
  void cached(const QString &trackId);

private:
  void step();
  void cancelPending();
  void handleStreamUrl(const QString &trackId, const QString &url);
  void startDownload(const QString &trackId, const QString &url);
  void finishCurrent();
  void markFailed(const QString &trackId);
  bool wanted(const QString &trackId) const;
  QList<Track> wantedTracks() const;

  TrackCache *m_cache = nullptr;
  TrackService *m_trackService = nullptr;
  QNetworkAccessManager *m_network = nullptr;
  QPointer<QNetworkReply> m_reply;
  QTimer m_stallTimer;

  QList<Track> m_upcoming;

  // Трек, для которого сейчас выясняется URL, и трек, который качается.
  QString m_resolvingTrackId;
  QString m_downloadingTrackId;

  QString m_codec;
  int m_bitrate = 0;
  QString m_writePath;

  QSet<QString> m_failedTrackIds;

  bool m_enabled = true;
  int m_aheadCount = 3;
};

} // namespace Playback
