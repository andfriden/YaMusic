#include "TrackListModelBase.h"
#include <QModelIndex>

TrackListModelBase::TrackFilter TrackListModelBase::s_filter;
QList<TrackListModelBase *> TrackListModelBase::s_instances;

TrackListModelBase::TrackListModelBase(QObject *parent) : QAbstractListModel(parent) {
  s_instances.append(this);
}

TrackListModelBase::~TrackListModelBase() {
  s_instances.removeAll(this);
}

void TrackListModelBase::setTrackFilter(TrackFilter filter) {
  s_filter = std::move(filter);
  refreshTrackFilters();
}

void TrackListModelBase::refreshTrackFilters() {
  const QList<TrackListModelBase *> instances = s_instances;

  for (TrackListModelBase *model : instances) {
    if (model != nullptr) {
      model->rebuildVisibleTracks();
    }
  }
}

bool TrackListModelBase::acceptsTrack(const Track &track) const {
  return !s_filter || s_filter(track);
}

void TrackListModelBase::rebuildVisibleTracks() {
  QList<Track> visible;
  visible.reserve(m_sourceTracks.size());

  for (const Track &track : m_sourceTracks) {
    if (acceptsTrack(track)) {
      visible.append(track);
    }
  }

  beginResetModel();
  m_tracks = visible;
  endResetModel();
  emit countChanged();
}

int TrackListModelBase::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return m_tracks.size();
}

Track TrackListModelBase::trackAt(int index) const {
  if (index < 0 || index >= m_tracks.size()) return {};
  return m_tracks.at(index);
}

QList<Track> TrackListModelBase::tracks() const {
  return m_tracks;
}

int TrackListModelBase::count() const {
  return m_tracks.size();
}

int TrackListModelBase::sourceIndex(int index) const {
  if (index < 0 || index >= m_tracks.size()) return -1;

  const QString trackId = m_tracks.at(index).id;
  if (trackId.isEmpty()) return -1;

  for (int i = 0; i < m_sourceTracks.size(); ++i) {
    if (m_sourceTracks.at(i).id == trackId) {
      return i;
    }
  }

  return -1;
}

void TrackListModelBase::clear() {
  if (m_sourceTracks.isEmpty() && m_tracks.isEmpty()) return;

  beginResetModel();
  m_sourceTracks.clear();
  m_tracks.clear();
  endResetModel();
  emit countChanged();
}

void TrackListModelBase::setTracks(const QList<Track> &tracks) {
  m_sourceTracks = tracks;
  rebuildVisibleTracks();
}

bool TrackListModelBase::setTrackLiked(const QString &trackId, bool liked, int likedRole) {
  bool found = false;

  for (int i = 0; i < m_sourceTracks.size(); ++i) {
    if (m_sourceTracks[i].id != trackId) continue;
    m_sourceTracks[i].liked = liked;
    found = true;
    break;
  }

  for (int i = 0; i < m_tracks.size(); ++i) {
    if (m_tracks[i].id != trackId) continue;

    m_tracks[i].liked = liked;
    emit dataChanged(index(i), index(i), {likedRole});
    return true;
  }

  return found;
}

bool TrackListModelBase::removeTrackById(const QString &trackId) {
  bool removed = false;

  for (int i = m_sourceTracks.size() - 1; i >= 0; --i) {
    if (m_sourceTracks.at(i).id == trackId) {
      m_sourceTracks.removeAt(i);
      removed = true;
    }
  }

  if (!removed) return false;

  rebuildVisibleTracks();
  return true;
}

void TrackListModelBase::appendTracksFiltered(const QList<Track> &tracks) {
  if (tracks.isEmpty()) return;

  m_sourceTracks.append(tracks);

  QList<Track> visible;
  visible.reserve(tracks.size());

  for (const Track &track : tracks) {
    if (acceptsTrack(track)) {
      visible.append(track);
    }
  }

  if (visible.isEmpty()) return;

  const int first = m_tracks.size();
  const int last = first + visible.size() - 1;

  beginInsertRows(QModelIndex(), first, last);
  m_tracks.append(visible);
  endInsertRows();

  emit countChanged();
}
