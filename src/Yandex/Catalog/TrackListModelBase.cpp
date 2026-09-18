#include "TrackListModelBase.h"
#include <QModelIndex>

int TrackListModelBase::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  return m_tracks.size();
}

Track TrackListModelBase::trackAt(int index) const {
  if (index < 0 || index >= m_tracks.size()) {
    return {};
  }
  return m_tracks.at(index);
}

QList<Track> TrackListModelBase::tracks() const {
  return m_tracks;
}

int TrackListModelBase::count() const {
  return m_tracks.size();
}

void TrackListModelBase::clear() {
  if (m_tracks.isEmpty()) {
    return;
  }

  beginResetModel();
  m_tracks.clear();
  endResetModel();
  emit countChanged();
}

void TrackListModelBase::setTracks(const QList<Track> &tracks) {
  beginResetModel();
  m_tracks = tracks;
  endResetModel();
  emit countChanged();
}

bool TrackListModelBase::setTrackLiked(const QString &trackId, bool liked, int likedRole) {
  for (int i = 0; i < m_tracks.size(); ++i) {
    if (m_tracks[i].id != trackId) continue;

    if (m_tracks[i].liked == liked) {
      return false;
    }

    m_tracks[i].liked = liked;
    emit dataChanged(index(i), index(i), {likedRole});
    return true;
  }
  return false;
}

bool TrackListModelBase::removeTrackById(const QString &trackId) {
  for (int i = 0; i < m_tracks.size(); ++i) {
    if (m_tracks[i].id != trackId) continue;
    beginRemoveRows(QModelIndex(), i, i);
    m_tracks.removeAt(i);
    endRemoveRows();
    return true;
  }
  return false;
}