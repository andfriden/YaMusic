#include "AlbumModel.h"

AlbumModel::AlbumModel(QObject *parent) : TrackListModelBase(parent) {}

QVariant AlbumModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= m_tracks.size()) {
    return {};
  }

  const Track &track = m_tracks.at(index.row());

  switch (role) {
  case IdRole:
    return track.id;

  case TitleRole:
    return track.title;

  case ArtistRole:
    return track.artists.isEmpty() ? QString() : track.artists.first().name;

  case ArtistIdRole:
    return track.artists.isEmpty() ? QString() : track.artists.first().id;

  case AlbumIdRole:
    return track.albums.isEmpty() ? QString() : track.albums.first().id;

  case CoverUriRole:
    return track.coverUri;

  case DurationMsRole:
    return track.durationMs;

  case LikedRole:
    return track.liked;
  }
  return {};
}

QHash<int, QByteArray> AlbumModel::roleNames() const {
  return {{IdRole, "trackId"},
          {TitleRole, "title"},
          {ArtistRole, "artist"},
          {ArtistIdRole, "artistId"},
          {AlbumIdRole, "albumId"},
          {CoverUriRole, "coverUri"},
          {DurationMsRole, "durationMs"},
          {LikedRole, "liked"}};
}

void AlbumModel::setAlbum(const AlbumDetails &album) {
  m_album = album;
  setTracks(album.tracks);
  emit albumChanged();
}

void AlbumModel::clear() {
  TrackListModelBase::clear();
  m_album = AlbumDetails{};
  emit albumChanged();
}

QString AlbumModel::title() const {
  return m_album.album.title;
}

QString AlbumModel::coverUri() const {
  return m_album.album.coverUri;
}

int AlbumModel::trackCount() const {
  return m_tracks.size();
}

void AlbumModel::setTrackLiked(const QString &trackId, bool liked) {
  TrackListModelBase::setTrackLiked(trackId, liked, LikedRole);
}
