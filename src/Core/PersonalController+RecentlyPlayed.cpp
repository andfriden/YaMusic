#include "../Playback/PlaybackController.h"
#include <QCoreApplication>
#include "../Queue/QueueService.h"
#include "PersonalController.h"

void PersonalController::connectRecentlyPlayed() {
  connect(m_recentListeningService, &RecentListeningService::tracksReceived, this,
          [this](const QList<Track> &tracks) {
            m_recentListeningModel->setTracks(tracks);

            emit statusChanged(
                QCoreApplication::translate("YaMusic", "Недавно слушали: %1 треков").arg(tracks.size()));
          });

  connect(m_recentListeningService, &RecentListeningService::errorOccurred, this,
          [this](const QString &message) {
            emit statusChanged(QCoreApplication::translate("YaMusic", "Ошибка истории прослушивания: %1").arg(message));
          });
}

void PersonalController::selectRecentListening(int index) {
  const QList<Track> tracks = m_recentListeningModel->tracks();

  if (tracks.isEmpty()) {
    emit statusChanged(QCoreApplication::translate("YaMusic", "История прослушивания пуста"));
    return;
  }

  if (index < 0 || index >= tracks.size()) {
    emit statusChanged(QCoreApplication::translate("YaMusic", "Некорректный трек истории"));
    return;
  }

  m_myWaveQueueActive = false;
  m_waitingForMoreMyWave = false;

  if (m_myWaveTrackStarted) {
    stopCurrentMyWaveTrack("skip");
  }

  QueueService *queue = m_playbackController->queueService();
  queue->clear();
  queue->addTracks(tracks);
  queue->setCurrentIndex(index);

  const Track track = tracks.at(index);
  emit statusChanged(QCoreApplication::translate("YaMusic", "Выбран трек: %1").arg(track.title));
  m_playbackController->playTrack(track);
}