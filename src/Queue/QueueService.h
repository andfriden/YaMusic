#pragma once

#include "../Models/Track.h"
#include <QList>
#include <QObject>
#include <QString>

class QueueService : public QObject {
  Q_OBJECT

public:
  enum RepeatMode { RepeatOff = 0, RepeatOne, RepeatAll };

  Q_ENUM(RepeatMode)

  explicit QueueService(QObject *parent = nullptr);

  int count() const;
  int currentIndex() const;
  Track currentTrack() const;
  Track trackAt(int index) const;
  QList<Track> tracks() const;

  QString sourceTitle() const;
  QString sourceType() const;
  void setSource(const QString &title, const QString &type);
  void clearSource();

  void addTrack(const Track &track);
  void addTracks(const QList<Track> &tracks);
  void removeTrack(int index);
  void moveTrack(int from, int to);
  void clear();

  bool setCurrentIndex(int index);
  bool next();
  bool previous();
  bool hasNext() const;
  bool hasPrevious() const;

  RepeatMode repeatMode() const;
  void setRepeatMode(RepeatMode mode);
  void cycleRepeatMode();

  bool shuffleEnabled() const;
  void setShuffleEnabled(bool enabled);
  void toggleShuffle();

signals:
  void queueChanged();
  void currentChanged();
  void repeatModeChanged();
  void shuffleChanged();

private:
  void rebuildShuffledQueue();

private:
  QList<Track> m_tracks;
  QList<Track> m_originalTracks;
  int m_currentIndex = -1;
  QString m_sourceTitle;
  QString m_sourceType;
  RepeatMode m_repeatMode = RepeatOff;
  bool m_shuffleEnabled = false;
};