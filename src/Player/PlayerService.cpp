#include "PlayerService.h"

PlayerService::PlayerService(QObject *parent)
    : QObject(parent), m_player(this), m_audioOutput(this) {
  m_audioOutput.setVolume(1.0f);
  m_audioOutput.setMuted(false);
  m_player.setAudioOutput(&m_audioOutput);

  connect(&m_player, &QMediaPlayer::playbackStateChanged, this,
          [this](QMediaPlayer::PlaybackState state) {
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

  connect(&m_player, &QMediaPlayer::mediaStatusChanged, this,
          [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia)
              emit playbackFinished();
          });

  connect(&m_player, &QMediaPlayer::positionChanged, this,
          [this](qint64 position) { emit positionChanged(position); });

  connect(&m_player, &QMediaPlayer::durationChanged, this,
          [this](qint64 duration) { emit durationChanged(duration); });

  connect(&m_player, &QMediaPlayer::errorOccurred, this,
          [this](QMediaPlayer::Error, const QString &errorString) {
            emit errorOccurred(errorString);
          });

  connect(&m_audioOutput, &QAudioOutput::volumeChanged, this, &PlayerService::volumeChanged);
  connect(&m_audioOutput, &QAudioOutput::mutedChanged, this, &PlayerService::mutedChanged);
}

bool PlayerService::isPlaying() const {
  return m_player.playbackState() == QMediaPlayer::PlayingState;
}

QString PlayerService::currentUrl() const {
  return m_currentUrl;
}

qint64 PlayerService::position() const {
  return m_player.position();
}

qint64 PlayerService::duration() const {
  if (m_trackDurationMs > 0)
    return m_trackDurationMs;
  return m_player.duration();
}

float PlayerService::volume() const {
  return m_audioOutput.volume();
}

bool PlayerService::isMuted() const {
  return m_audioOutput.isMuted();
}

void PlayerService::play() {
  m_player.play();
}

void PlayerService::playUrl(const QString &url) {
  if (url.isEmpty()) return;

  if (m_currentUrl != url) {
    m_currentUrl = url;
    m_sourceDevice.clear();
    m_player.setSource(QUrl(url));
    emit currentUrlChanged();
  }

  m_player.play();
}

/*
 * Воспроизведение из QIODevice (буферизованный сетевой поток).
 *
 * FFmpeg на macOS не умеет надёжно играть HTTPS-аудио напрямую
 * (SecureTransport падает с -9806), поэтому аудио качается через
 * QNetworkAccessManager (свой SSL-стек Qt), а плееру отдаётся уже
 * распарсенный поток данных. Жизненным циклом устройства управляет
 * PlayerService: при смене источника старый прокси останавливается.
 */
void PlayerService::playDevice(QIODevice *device) {
  if (!device) return;

  m_currentUrl.clear();
  m_sourceDevice = device;
  m_player.setSourceDevice(device);
  m_player.play();
}

void PlayerService::setTrackDuration(qint64 durationMs) {
  if (m_trackDurationMs != durationMs) {
    m_trackDurationMs = durationMs;
    emit durationChanged(duration());
  }
}

void PlayerService::pause() {
  m_player.pause();
}

void PlayerService::resume() {
  m_player.play();
}

void PlayerService::stop() {
  m_player.stop();
}

void PlayerService::togglePlayback() {
  if (isPlaying())
    pause();
  else
    resume();
}

void PlayerService::seek(qint64 position) {
  const qint64 effectiveDuration =
      m_trackDurationMs > 0 ? m_trackDurationMs : m_player.duration();

  if (position <= 0) {
    m_player.setPosition(0);
    return;
  }

  // Для стримов из QIODevice ffmpeg умеет искать только в пределах
  // уже скачанного буфера; если длительность неизвестна, всё равно
  // даём команду плееру — он сам обработает недоступный seek.
  if (effectiveDuration > 0) {
    const qint64 clampedPosition = qBound(qint64(0), position, effectiveDuration);
    m_player.setPosition(clampedPosition);
  } else {
    m_player.setPosition(position);
  }
}

void PlayerService::setVolume(float volume) {
  const float clampedVolume = qBound(0.0f, volume, 1.0f);
  m_audioOutput.setVolume(clampedVolume);
}

void PlayerService::setMuted(bool muted) {
  if (m_audioOutput.isMuted() == muted) return;
  m_audioOutput.setMuted(muted);
}

void PlayerService::toggleMute() {
  const bool muted = m_audioOutput.isMuted();
  m_audioOutput.setMuted(!muted);
}