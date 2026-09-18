#include "ChartModel.h"

ChartModel::ChartModel(QObject *parent) : TrackListModelBase(parent) {}

QVariant ChartModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= m_tracks.size())
    return {};

  const Track &track = m_tracks.at(index.row());

  switch (role) {
  case IdRole:
    return track.id;
  case TitleRole:
    return track.title;
  case ArtistRole: {
    QStringList names;
    for (const Artist &artist : track.artists) {
      if (!artist.name.isEmpty())
        names.append(artist.name);
    }
    return names.join(", ");
  }
  case ArtistIdRole: {
    if (track.artists.isEmpty())
      return QString();
    return track.artists.first().id;
  }
  case CoverUriRole:
    return track.coverUri;
  case DurationMsRole:
    return track.durationMs;
  case PositionRole:
    return index.row() + 1;
  default:
    return {};
  }
}

QHash<int, QByteArray> ChartModel::roleNames() const {
  return {{IdRole, "trackId"},        {TitleRole, "title"},       {ArtistRole, "artist"},
          {ArtistIdRole, "artistId"}, {CoverUriRole, "coverUri"}, {DurationMsRole, "durationMs"},
          {PositionRole, "position"}};
}

void ChartModel::setTracks(const QList<Track> &tracks) {
  TrackListModelBase::setTracks(tracks);
}