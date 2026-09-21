#include "YnisonReporter.h"

#include "../Player/PlayerService.h"
#include "../Queue/QueueService.h"

#include <QDateTime>
#include <QJsonDocument>

namespace {

// Как часто публикуем прогресс в Ynison (как ma-provider).
constexpr int ProgressIntervalMs = 5 * 1000;

QString repeatModeName(QueueService::RepeatMode mode) {
  switch (mode) {
  case QueueService::RepeatOne:
    return QStringLiteral("ONE");
  case QueueService::RepeatAll:
    return QStringLiteral("ALL");
  case QueueService::RepeatOff:
  default:
    return QStringLiteral("NONE");
  }
}

// Версия обновления: все поля строками (иначе сервер отвечает 500).
QJsonObject versionBlock(const QString &deviceId) {
  return QJsonObject{
      {"device_id", deviceId},
      {"version", QString::number(
           QDateTime::currentMSecsSinceEpoch() * 1000)},
      {"timestamp_ms", "0"},
  };
}

} // namespace

YnisonReporter::YnisonReporter(
    YnisonClient *client,
    PlayerService *player,
    QueueService *queue,
    QObject *parent)
    : QObject(parent),
      m_client(client),
      m_player(player),
      m_queue(queue) {
  Q_ASSERT(m_client != nullptr);
  Q_ASSERT(m_player != nullptr);
  Q_ASSERT(m_queue != nullptr);

  m_progressTimer.setInterval(ProgressIntervalMs);
  connect(&m_progressTimer, &QTimer::timeout, this,
          &YnisonReporter::onProgressTick);

  connect(m_player, &PlayerService::playbackStarted, this,
          &YnisonReporter::onPlaybackStarted);
  connect(m_player, &PlayerService::playbackPaused, this,
          &YnisonReporter::onPlaybackPaused);
  connect(m_player, &PlayerService::playbackFinished, this,
          &YnisonReporter::onPlaybackFinished);
  connect(m_player, &PlayerService::playbackStopped, this,
          &YnisonReporter::onPlaybackPaused);

  connect(m_queue, &QueueService::currentChanged, this,
          &YnisonReporter::onTrackChanged);
  connect(m_queue, &QueueService::queueChanged, this,
          &YnisonReporter::onQueueChanged);
}

void YnisonReporter::start() {
  // Ничего не публикуем до реального старта воспроизведения:
  // сервер отклоняет очередь без активного плеера (500 / 400030001).
}

void YnisonReporter::stop() {
  m_progressTimer.stop();
}

void YnisonReporter::onPlaybackStarted() {
  m_playing = true;
  m_progressTimer.start();

  // Захватываем активность устройства — именно активное устройство-плеер
  // пишется в историю.
  if (m_client->isConnected())
    m_client->sendActiveDevice();

  publishQueueState();
  publishStatus(false);
}

void YnisonReporter::onPlaybackPaused() {
  m_playing = false;
  m_progressTimer.stop();
  publishStatus(true);
}

void YnisonReporter::onPlaybackFinished() {
  m_playing = false;
  m_progressTimer.stop();

  // Финальный прогресс на длительности трека — сервер фиксирует
  // завершённое прослушивание в истории.
  const qint64 durationMs = m_player->duration();
  if (durationMs > 0) {
    publishStatus(false);
    m_client->sendPlayingStatus(durationMs, durationMs, false);
  }

  // После завершения плеер сам двигает очередь; публикация нового
  // состояния произойдёт по currentChanged.
}

void YnisonReporter::onTrackChanged() {
  if (m_queue->currentTrack().id.isEmpty())
    return;

  m_currentTrackId = m_queue->currentTrack().id;

  // Публикуем очередь только когда плеер реально играет.
  if (m_playing)
    publishQueueState();
}

void YnisonReporter::onQueueChanged() {
  // Не публикуем очередь без активного воспроизведения: сервер
  // отвечает 500 / 400030001 на очередь без играющего плеера.
}

void YnisonReporter::onProgressTick() {
  if (!m_playing)
    return;

  const qint64 positionMs = m_player->position();
  const qint64 durationMs = m_player->duration();

  if (durationMs <= 0)
    return;

  // Кламп: Ynison отклоняет progress > duration (400030001).
  const qint64 progressMs = qMin(positionMs, durationMs);
  m_client->sendPlayingStatus(progressMs, durationMs, false);
}

void YnisonReporter::publishQueueState() {
  m_client->sendPlayerState(buildPlayerState());
}

void YnisonReporter::publishStatus(bool paused) {
  const qint64 durationMs = m_player->duration();
  if (durationMs <= 0)
    return;

  m_client->sendPlayingStatus(
      qMin(m_player->position(), durationMs),
      durationMs,
      paused);
}

QJsonObject YnisonReporter::buildPlayerState() const {
  return QJsonObject{
      {"status", QJsonObject{
          {"paused", !m_playing},
          {"progress_ms", QString::number(
               qMax<qint64>(0, m_player->position()))},
          {"duration_ms", QString::number(
               qMax<qint64>(0, m_player->duration()))},
          {"playback_speed", 1},
          {"version", versionBlock(m_client->deviceId())},
      }},
      {"player_queue", buildQueue()},
  };
}

QJsonObject YnisonReporter::buildQueue() const {
  QJsonArray playables;

  const QList<Track> tracks = m_queue->tracks();
  for (const Track &track : tracks)
    playables.append(buildPlayable(track));

  const int index = m_queue->currentIndex();

  return QJsonObject{
      {"current_playable_index", index},
      {"entity_id", ""},
      {"entity_type", "VARIOUS"},
      {"entity_context", "BASED_ON_ENTITY_BY_DEFAULT"},
      {"playable_list", playables},
      {"options", QJsonObject{
          {"repeat_mode", repeatModeName(m_queue->repeatMode())},
      }},
      {"version", versionBlock(m_client->deviceId())},
  };
}

QJsonObject YnisonReporter::buildPlayable(const Track &track) const {
  QString albumId;
  if (!track.albums.isEmpty())
    albumId = track.albums.first().id;

  return QJsonObject{
      {"playable_id", track.id},
      {"album_id_optional", albumId},
      {"playable_type", "TRACK"},
      {"from", "desktop-ya-music"},
      {"title", track.title},
      {"cover_url_optional", track.coverUri},
  };
}