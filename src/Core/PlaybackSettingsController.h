#pragma once

#include <QObject>

class PlayerService;

// Настройки воспроизведения для интерфейса. Сейчас это перекрёстное
// затухание между треками: 0 — обычный переход встык, как было раньше.
//
// Само затухание делает PlayerService, контроллер только хранит выбор,
// применяет его и помнит между запусками.
class PlaybackSettingsController : public QObject {
  Q_OBJECT

  Q_PROPERTY(int crossfadeSeconds READ crossfadeSeconds WRITE setCrossfadeSeconds NOTIFY
                 crossfadeSecondsChanged)

public:
  explicit PlaybackSettingsController(PlayerService *playerService, QObject *parent = nullptr);

  int crossfadeSeconds() const;

  void setCrossfadeSeconds(int seconds);

signals:
  void crossfadeSecondsChanged();

private:
  // Границы выбора: больше 12 секунд — это уже не переход между треками.
  static constexpr int kMaxCrossfadeSeconds = 12;

  PlayerService *m_playerService = nullptr;
  int m_crossfadeSeconds = 6;
};
