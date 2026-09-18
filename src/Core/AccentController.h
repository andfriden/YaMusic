#pragma once

#include "PlayerAccentService.h"
#include <QColor>
#include <QObject>

class PlaybackController;

// Акцент из обложки трека — подписан на currentTrackChanged
class AccentController : public QObject {
  Q_OBJECT

  Q_PROPERTY(QColor playerAccent READ playerAccent NOTIFY playerAccentChanged)

public:
  explicit AccentController(PlayerAccentService *service, PlaybackController *playbackController,
                            QObject *parent = nullptr);

  QColor playerAccent() const;

signals:
  void playerAccentChanged();

private:
  PlayerAccentService *m_service = nullptr;
  PlaybackController *m_playbackController = nullptr;
};