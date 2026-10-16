#pragma once

#include <QAudioOutput>
#include <QIODevice>
#include <QMediaPlayer>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QTimer>
#include <QUrl>

// Воспроизведение трека.
//
// Держит два голоса (плеер + выход): один играет, второй нужен, чтобы
// перейти в следующий трек перекрёстным затуханием, не обрывая текущий.
// Сигналы и позиция относятся только к активному голосу; затухающий
// молчит для внешнего мира, его глушат и останавливают в конце перехода.
class PlayerService : public QObject {
  Q_OBJECT

public:
  explicit PlayerService(QObject *parent = nullptr);

  bool isPlaying() const;
  QString currentUrl() const;
  qint64 position() const;
  qint64 duration() const;
  float volume() const;
  bool isMuted() const;

  // Длительность перекрёстного затухания: 0 — переход встык, как раньше.
  int crossfadeMs() const;
  void setCrossfadeMs(int milliseconds);

  bool isCrossfading() const;

public slots:
  void play();
  void playUrl(const QString &url);
  void playDevice(QIODevice *device);

  // Плавный переход в другой источник: уходящий голос гаснет, приходящий
  // набирает громкость за crossfadeMs. Активным приходящий становится
  // сразу, поэтому интерфейс показывает уже следующий трек.
  void playUrlFading(const QString &url);
  void playDeviceFading(QIODevice *device);

  void setTrackDuration(qint64 durationMs);
  void pause();
  void resume();
  void stop();
  void togglePlayback();
  void seek(qint64 position);
  void setVolume(float volume);
  void setMuted(bool muted);
  void toggleMute();

signals:
  void playingChanged();
  void playbackStarted();
  void playbackPaused();
  void playbackStopped();
  void playbackFinished();
  void currentUrlChanged();
  void positionChanged(qint64 position);
  void durationChanged(qint64 duration);
  void errorOccurred(const QString &message);
  void volumeChanged();
  void mutedChanged();

  // Переход закончился: уходящий голос остановлен.
  void crossfadeFinished();

private:
  void applyVolume();
  void startFade();
  void finishFade();
  void cancelFade();
  void stopVoice(bool second, bool clearSource);
  void playOnActiveVoice(const QString &url, QIODevice *device);

  // Голос, который сейчас играет, и голос, который молчит и ждёт перехода.
  QMediaPlayer &activePlayer();
  QAudioOutput &activeOutput();
  QMediaPlayer &idlePlayer();
  QAudioOutput &idleOutput();

  QMediaPlayer m_player;
  QAudioOutput m_audioOutput;
  QMediaPlayer m_secondPlayer;
  QAudioOutput m_secondOutput;

  QTimer m_fadeTimer;

  // Какой голос считается активным и идёт ли сейчас переход.
  bool m_secondVoiceActive = false;
  bool m_fading = false;
  bool m_fadeFromSecond = false;
  qint64 m_fadeElapsedMs = 0;
  int m_crossfadeMs = 0;

  float m_volume = 1.0f;
  bool m_muted = false;

  QString m_currentUrl;
  qint64 m_trackDurationMs = -1;

  // Устройства источников: держим их, пока трек играет.
  QPointer<QIODevice> m_sourceDevice;
  QPointer<QIODevice> m_secondSourceDevice;
};
