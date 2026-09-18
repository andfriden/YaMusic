#pragma once

#include "../../Models/Track.h"
#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QVariant>

class TrackListModelBase : public QAbstractListModel {
  Q_OBJECT

  Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
  using QAbstractListModel::QAbstractListModel;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  Track trackAt(int index) const;
  QList<Track> tracks() const;
  int count() const;
  void clear();
  void setTracks(const QList<Track> &tracks);
  bool setTrackLiked(const QString &trackId, bool liked, int likedRole);
  bool removeTrackById(const QString &trackId);

signals:
  void countChanged();

protected:
  QList<Track> m_tracks;
};