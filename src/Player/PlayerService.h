#pragma once

#include <QAudioOutput>
#include <QIODevice>
#include <QMediaPlayer>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QUrl>

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

public slots:
  void play();
  void playUrl(const QString &url);
  void playDevice(QIODevice *device);
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

private:
  QMediaPlayer m_player;
  QAudioOutput m_audioOutput;
  QString m_currentUrl;
  qint64 m_trackDurationMs = -1;
  QPointer<QIODevice> m_sourceDevice;
};