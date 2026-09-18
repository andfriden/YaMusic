#pragma once

#include <QObject>
#include <QString>

class SystemMediaControls : public QObject {
  Q_OBJECT

public:
  enum class PlaybackStatus { Stopped, Playing, Paused };

  struct Metadata {
    QString title;
    QString artist;
    QString album;
    QString coverUrl;
    qint64 durationMs = 0;
    QString trackId;
  };

  explicit SystemMediaControls(QObject *parent = nullptr);
  ~SystemMediaControls() override;

  // Реализация конкретной платформы.
  // Вызывать только один раз, после создания объекта.
  void setEnabled(bool enabled);

  bool isEnabled() const;

  void setMetadata(const Metadata &metadata);
  void setPlaybackStatus(PlaybackStatus status);
  void setPosition(qint64 positionMs);
  void setDuration(qint64 durationMs);

  // Опциональные свойства (использует MPRIS):
  // LoopStatus в терминах MPRIS: "None" | "Playlist" | "Track".
  void setLoopStatus(const QString &loopStatus);
  void setShuffle(bool enabled);

signals:
  void togglePlayPauseRequested();
  void playRequested();
  void pauseRequested();
  void nextRequested();
  void previousRequested();
  void seekRequested(qint64 positionMs);

protected:
  virtual void platformSetEnabled(bool enabled) = 0;
  virtual void platformSetMetadata(const Metadata &metadata) = 0;
  virtual void platformSetPlaybackStatus(PlaybackStatus status) = 0;
  virtual void platformSetPosition(qint64 positionMs) = 0;
  virtual void platformSetDuration(qint64 durationMs) = 0;

  // По умолчанию — no-op; переопределяет MPRIS.
  virtual void platformSetLoopStatus(const QString &loopStatus);
  virtual void platformSetShuffle(bool enabled);

protected:
  bool m_enabled = false;
  Metadata m_metadata;
  PlaybackStatus m_status = PlaybackStatus::Stopped;
  qint64 m_positionMs = 0;
  qint64 m_durationMs = 0;
  QString m_loopStatus = QStringLiteral("None");
  bool m_shuffle = false;

  // Аксессоры для платформенных реализаций,
  // чей код лежит в .cpp (MPRIS Impl).
  const Metadata &mediaMetadata() const { return m_metadata; }
  PlaybackStatus mediaPlaybackStatus() const { return m_status; }
  qint64 mediaPositionMs() const { return m_positionMs; }
  qint64 mediaDurationMs() const { return m_durationMs; }
  const QString &mediaLoopStatus() const { return m_loopStatus; }
  bool mediaShuffle() const { return m_shuffle; }
};