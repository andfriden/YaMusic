#pragma once

#include "../Models/Track.h"
#include "YnisonClient.h"

#include <QJsonArray>
#include <QObject>
#include <QString>
#include <QTimer>

class PlayerService;
class QueueService;

// Связывает плеер с Ynison и публикует события воспроизведения,
// чтобы сервер Яндекс.Музыки писал их в «Историю прослушивания».
//
// Паттерн (из ma-provider-yandex-ynison):
//  - при смене трека отправляем update_player_state с полной очередью
//    (playable_list, current_playable_index) — сервер фиксирует трек;
//  - каждые 5 секунд шлём update_playing_status с не-paused прогрессом
//    (именно поток растущего прогресса от активного плеера учитывается
//    в истории);
//  - при завершении трека сначала шлём финальный progress=duration,
//    затем update_player_state со сдвинутым индексом.
class YnisonReporter : public QObject {
  Q_OBJECT

public:
  explicit YnisonReporter(
      YnisonClient *client,
      PlayerService *player,
      QueueService *queue,
      QObject *parent = nullptr);

public slots:
  void start();
  void stop();

private:
  void onPlaybackStarted();
  void onPlaybackPaused();
  void onPlaybackFinished();
  void onTrackChanged();
  void onQueueChanged();
  void onProgressTick();

  void publishQueueState();
  void publishStatus(bool paused);

  QJsonObject buildPlayerState() const;
  QJsonObject buildQueue() const;
  QJsonObject buildPlayable(const Track &track) const;

  YnisonClient *m_client = nullptr;
  PlayerService *m_player = nullptr;
  QueueService *m_queue = nullptr;

  QTimer m_progressTimer;
  QString m_currentTrackId;
  bool m_playing = false;
};