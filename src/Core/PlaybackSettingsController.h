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

  // Бесшовный переход: следующий трек готовится заранее и стартует без
  // паузы. Работает только когда перекрёстное затухание выключено.
  Q_PROPERTY(bool gaplessEnabled READ gaplessEnabled WRITE setGaplessEnabled NOTIFY
                 gaplessEnabledChanged)

public:
  explicit PlaybackSettingsController(PlayerService *playerService, QObject *parent = nullptr);

  int crossfadeSeconds() const;

  void setCrossfadeSeconds(int seconds);

  bool gaplessEnabled() const;
  void setGaplessEnabled(bool enabled);

signals:
  void crossfadeSecondsChanged();
  void gaplessEnabledChanged();

private:
  // Границы выбора: больше 12 секунд — это уже не переход между треками.
  static constexpr int kMaxCrossfadeSeconds = 12;

  PlayerService *m_playerService = nullptr;
  int m_crossfadeSeconds = 6;
  bool m_gaplessEnabled = true;
};
