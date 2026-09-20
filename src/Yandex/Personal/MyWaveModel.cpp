#include "MyWaveModel.h"

#include <QStringList>

MyWaveModel::MyWaveModel(QObject *parent)
    : TrackListModelBase(parent) {}

QVariant MyWaveModel::data(
    const QModelIndex &index,
    int role) const {

  if (!index.isValid() ||
      index.row() < 0 ||
      index.row() >= m_tracks.size()) {
    return {};
  }

  const Track &track =
      m_tracks.at(index.row());

  switch (role) {

  case IdRole:
    return track.id;

  case TitleRole:
    return track.title;

  case ArtistRole: {
    QStringList names;

    for (const Artist &artist : track.artists) {
      if (!artist.name.isEmpty()) {
        names.append(artist.name);
      }
    }

    return names.join(", ");
  }

  case ArtistIdRole:
    return track.artists.isEmpty()
        ? QString()
        : track.artists.first().id;

  case AlbumRole:
    return track.albums.isEmpty()
        ? QString()
        : track.albums.first().title;

  case AlbumIdRole:
    return track.albums.isEmpty()
        ? QString()
        : track.albums.first().id;

  case CoverUriRole:
    return track.coverUri;

  case DurationMsRole:
    return track.durationMs;

  default:
    return {};
  }
}

QHash<int, QByteArray> MyWaveModel::roleNames() const {
  return {
      {IdRole, "trackId"},
      {TitleRole, "title"},
      {ArtistRole, "artist"},
      {ArtistIdRole, "artistId"},
      {AlbumRole, "album"},
      {AlbumIdRole, "albumId"},
      {CoverUriRole, "coverUri"},
      {DurationMsRole, "durationMs"}
  };
}

void MyWaveModel::setTracks(
    const QList<Track> &tracks) {

  TrackListModelBase::setTracks(tracks);
}

void MyWaveModel::appendTracks(
    const QList<Track> &tracks) {

  QList<Track> newTracks;

  for (const Track &track : tracks) {

    if (track.id.isEmpty()) {
      continue;
    }

    bool exists = false;

    for (const Track &existing : m_tracks) {
      if (existing.id == track.id) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      newTracks.append(track);
    }
  }

  if (newTracks.isEmpty()) {
    return;
  }

  const int first =
      m_tracks.size();

  const int last =
      first + newTracks.size() - 1;

  beginInsertRows(
      QModelIndex(),
      first,
      last);

  m_tracks.append(newTracks);

  endInsertRows();

  // Используем countChanged() из TrackListModelBase.
  emit countChanged();
}

// TODO(#180): ограничить историю волны последними 200 треками
Track MyWaveModel::lastTrack() const {

  if (m_tracks.isEmpty()) {
    return {};
  }

  return m_tracks.last();
}