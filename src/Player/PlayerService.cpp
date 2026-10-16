#include "PlayerService.h"
#include "CrossfadeCurve.h"

namespace {

// Шаг рампы перекрёстного затухания: 30 мс незаметны на слух, но дают
// ровный переход и не грузят цикл событий.
constexpr int kFadeTickMs = 30;

} // namespace

PlayerService::PlayerService(QObject *parent)
    : QObject(parent), m_player(this), m_audioOutput(this), m_secondPlayer(this),
      m_secondOutput(this) {
  m_audioOutput.setVolume(1.0f);
  m_audioOutput.setMuted(false);
  m_player.setAudioOutput(&m_audioOutput);

  // Второй голос молчит, пока его не позовут в перекрёстный переход.
  m_secondOutput.setVolume(0.0f);
  m_secondOutput.setMuted(true);
  m_secondPlayer.setAudioOutput(&m_secondOutput);

  // Сигналы обоих голосов приходят сюда: наружу пропускаем только те, что
  // относятся к активному. Затухающий голос для внешнего мира молчит —
  // иначе он присылал бы и позицию, и «воспроизведение остановлено».
  auto connectVoice = [this](QMediaPlayer *player, bool second) {
    connect(player, &QMediaPlayer::playbackStateChanged, this,
            [this, second](QMediaPlayer::PlaybackState state) {
              if (second != m_secondVoiceActive) return;

              emit playingChanged();

              switch (state) {
              case QMediaPlayer::PlayingState:
                emit playbackStarted();
                break;
              case QMediaPlayer::PausedState:
                emit playbackPaused();
                break;
              case QMediaPlayer::StoppedState:
                emit playbackStopped();
                break;
              default:
                break;
              }
            });

    connect(player, &QMediaPlayer::mediaStatusChanged, this,
            [this, second](QMediaPlayer::MediaStatus status) {
              if (second != m_secondVoiceActive) return;

              if (status == QMediaPlayer::EndOfMedia) emit playbackFinished();
            });

    connect(player, &QMediaPlayer::positionChanged, this,
            [this, second](qint64 position) {
              if (second != m_secondVoiceActive) return;

              emit positionChanged(position);
            });

    connect(player, &QMediaPlayer::durationChanged, this,
            [this, second](qint64 duration) {
              if (second != m_secondVoiceActive) return;

              emit durationChanged(duration);
            });

    connect(player, &QMediaPlayer::errorOccurred, this,
            [this, second](QMediaPlayer::Error, const QString &errorString) {
              if (second != m_secondVoiceActive) return;

              emit errorOccurred(errorString);
            });
  };

  connectVoice(&m_player, false);
  connectVoice(&m_secondPlayer, true);

  m_fadeTimer.setInterval(kFadeTickMs);
  connect(&m_fadeTimer, &QTimer::timeout, this, [this]() {
    m_fadeElapsedMs += kFadeTickMs;

    if (m_fadeElapsedMs >= m_crossfadeMs) {
      finishFade();
      return;
    }

    applyVolume();
  });
}

QMediaPlayer &PlayerService::activePlayer() {
  return m_secondVoiceActive ? m_secondPlayer : m_player;
}

QAudioOutput &PlayerService::activeOutput() {
  return m_secondVoiceActive ? m_secondOutput : m_audioOutput;
}

QMediaPlayer &PlayerService::idlePlayer() {
  return m_secondVoiceActive ? m_player : m_secondPlayer;
}

QAudioOutput &PlayerService::idleOutput() {
  return m_secondVoiceActive ? m_audioOutput : m_secondOutput;
}

bool PlayerService::isPlaying() const {
  return (m_secondVoiceActive ? m_secondPlayer : m_player).playbackState()
      == QMediaPlayer::PlayingState;
}

QString PlayerService::currentUrl() const {
  return m_currentUrl;
}

qint64 PlayerService::position() const {
  return (m_secondVoiceActive ? m_secondPlayer : m_player).position();
}

qint64 PlayerService::duration() const {
  if (m_trackDurationMs > 0) return m_trackDurationMs;

  return (m_secondVoiceActive ? m_secondPlayer : m_player).duration();
}

float PlayerService::volume() const {
  return m_volume;
}

bool PlayerService::isMuted() const {
  return m_muted;
}

int PlayerService::crossfadeMs() const {
  return m_crossfadeMs;
}

bool PlayerService::isCrossfading() const {
  return m_fading;
}

void PlayerService::setCrossfadeMs(int milliseconds) {
  m_crossfadeMs = qBound(0, milliseconds, 30000);

  // Переход мог стать короче уже идущего: доводим его до конца сейчас.
  if (m_fading && m_fadeElapsedMs >= m_crossfadeMs) finishFade();
}

void PlayerService::play() {
  activePlayer().play();
}

void PlayerService::playUrl(const QString &url) {
  if (url.isEmpty()) return;

  cancelFade();
  playOnActiveVoice(url, nullptr);
}

void PlayerService::playDevice(QIODevice *device) {
  if (!device) return;

  cancelFade();
  playOnActiveVoice({}, device);
}

void PlayerService::playUrlFading(const QString &url) {
  if (url.isEmpty()) return;

  // Без настроенного кроссфадинга или когда ничего не играет перехода нет:
  // трек просто сменяется, как раньше.
  if (m_crossfadeMs <= 0 || !isPlaying()) {
    playUrl(url);
    return;
  }

  const bool incomingIsSecond = !m_secondVoiceActive;
  QMediaPlayer &incoming = incomingIsSecond ? m_secondPlayer : m_player;
  QAudioOutput &incomingOutput = incomingIsSecond ? m_secondOutput : m_audioOutput;

  incomingOutput.setMuted(false);
  incomingOutput.setVolume(0.0f);
  incoming.setSource(QUrl(url));

  if (incomingIsSecond)
    m_secondSourceDevice.clear();
  else
    m_sourceDevice.clear();

  m_currentUrl = url;
  emit currentUrlChanged();

  // Входящий становится активным до play(): его сигналы должны быть
  // слышны, а сигналы уходящего — уже нет.
  m_fadeFromSecond = m_secondVoiceActive;
  m_secondVoiceActive = incomingIsSecond;

  incoming.play();

  startFade();
}

void PlayerService::playDeviceFading(QIODevice *device) {
  if (!device) return;

  if (m_crossfadeMs <= 0 || !isPlaying()) {
    playDevice(device);
    return;
  }

  const bool incomingIsSecond = !m_secondVoiceActive;
  QMediaPlayer &incoming = incomingIsSecond ? m_secondPlayer : m_player;
  QAudioOutput &incomingOutput = incomingIsSecond ? m_secondOutput : m_audioOutput;

  incomingOutput.setMuted(false);
  incomingOutput.setVolume(0.0f);
  incoming.setSourceDevice(device);

  if (incomingIsSecond)
    m_secondSourceDevice = device;
  else
    m_sourceDevice = device;

  m_currentUrl.clear();
  emit currentUrlChanged();

  m_fadeFromSecond = m_secondVoiceActive;
  m_secondVoiceActive = incomingIsSecond;

  incoming.play();

  startFade();
}

void PlayerService::playOnActiveVoice(const QString &url, QIODevice *device) {
  QMediaPlayer &player = activePlayer();

  if (device != nullptr) {
    m_currentUrl.clear();

    if (m_secondVoiceActive)
      m_secondSourceDevice = device;
    else
      m_sourceDevice = device;

    player.setSourceDevice(device);
    applyVolume();
    player.play();
    return;
  }

  if (m_currentUrl != url) {
    m_currentUrl = url;

    if (m_secondVoiceActive)
      m_secondSourceDevice.clear();
    else
      m_sourceDevice.clear();

    player.setSource(QUrl(url));
    emit currentUrlChanged();
  }

  applyVolume();
  player.play();
}

void PlayerService::setTrackDuration(qint64 durationMs) {
  if (m_trackDurationMs != durationMs) {
    m_trackDurationMs = durationMs;
    emit durationChanged(duration());
  }
}

void PlayerService::pause() {
  activePlayer().pause();
}

void PlayerService::resume() {
  activePlayer().play();
}

void PlayerService::stop() {
  m_fadeTimer.stop();
  m_fading = false;
  m_fadeElapsedMs = 0;

  m_player.stop();
  m_secondPlayer.stop();

  m_sourceDevice.clear();
  m_secondSourceDevice.clear();
  applyVolume();
}

void PlayerService::togglePlayback() {
  if (isPlaying())
    pause();
  else
    resume();
}

void PlayerService::seek(qint64 position) {
  QMediaPlayer &player = activePlayer();
  const qint64 effectiveDuration = m_trackDurationMs > 0 ? m_trackDurationMs : player.duration();

  if (position <= 0) {
    player.setPosition(0);
    return;
  }

  // Для стримов из QIODevice ffmpeg умеет искать только в пределах
  // уже скачанного буфера; если длительность неизвестна, всё равно
  // даём команду плееру — он сам обработает недоступный seek.
  if (effectiveDuration > 0) {
    const qint64 clampedPosition = qBound(qint64(0), position, effectiveDuration);
    player.setPosition(clampedPosition);
  } else {
    player.setPosition(position);
  }
}

void PlayerService::setVolume(float volume) {
  m_volume = qBound(0.0f, volume, 1.0f);
  applyVolume();
  emit volumeChanged();
}

void PlayerService::setMuted(bool muted) {
  if (m_muted == muted) return;

  m_muted = muted;
  applyVolume();
  emit mutedChanged();
}

void PlayerService::toggleMute() {
  setMuted(!m_muted);
}

void PlayerService::applyVolume() {
  double firstFactor = 1.0;
  double secondFactor = 0.0;

  if (m_fading) {
    const double progress = m_crossfadeMs > 0
                                ? static_cast<double>(m_fadeElapsedMs)
                                      / static_cast<double>(m_crossfadeMs)
                                : 1.0;
    const Playback::CrossfadeGains gains = Playback::crossfadeGains(progress);

    // Гаснет тот голос, который был активным до перехода.
    secondFactor = m_fadeFromSecond ? gains.out : gains.in;
    firstFactor = m_fadeFromSecond ? gains.in : gains.out;
  } else {
    secondFactor = m_secondVoiceActive ? 1.0 : 0.0;
    firstFactor = m_secondVoiceActive ? 0.0 : 1.0;
  }

  m_audioOutput.setVolume(static_cast<float>(m_volume * firstFactor));
  m_secondOutput.setVolume(static_cast<float>(m_volume * secondFactor));
  m_audioOutput.setMuted(m_muted);
  m_secondOutput.setMuted(m_muted);
}

void PlayerService::startFade() {
  m_fadeElapsedMs = 0;
  m_fading = true;

  applyVolume();

  if (m_crossfadeMs <= 0) {
    finishFade();
    return;
  }

  m_fadeTimer.start();
}

void PlayerService::finishFade() {
  m_fadeTimer.stop();

  if (!m_fading) return;

  m_fading = false;
  m_fadeElapsedMs = 0;

  // Уходящий голос больше не нужен: глушим и отпускаем источник.
  stopVoice(m_fadeFromSecond, true);
  applyVolume();

  emit crossfadeFinished();
}

void PlayerService::cancelFade() {
  if (!m_fading) return;

  m_fadeTimer.stop();
  m_fading = false;
  m_fadeElapsedMs = 0;

  // Переход прервали (например, выбрали другой трек) — уходящий гасим сразу.
  stopVoice(m_fadeFromSecond, true);
  applyVolume();
}

void PlayerService::stopVoice(bool second, bool clearSource) {
  QMediaPlayer &player = second ? m_secondPlayer : m_player;

  player.stop();

  if (!clearSource) return;

  player.setSource(QUrl());

  if (second)
    m_secondSourceDevice.clear();
  else
    m_sourceDevice.clear();
}
