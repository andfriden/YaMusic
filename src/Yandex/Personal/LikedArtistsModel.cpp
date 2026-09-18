#include "LikedArtistsModel.h"

LikedArtistsModel::LikedArtistsModel(QObject *parent) : QAbstractListModel(parent) {}

int LikedArtistsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return m_artists.size();
}

QVariant LikedArtistsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= m_artists.size()) return {};
  const Artist &artist = m_artists.at(index.row());

  switch (role) {
  case ArtistIdRole:
    return artist.id;
  case NameRole:
    return artist.name;
  case CoverUriRole:
    return artist.coverUri;
  default:
    return {};
  }
}

QHash<int, QByteArray> LikedArtistsModel::roleNames() const {
  return {{ArtistIdRole, "artistId"}, {NameRole, "name"}, {CoverUriRole, "coverUri"}};
}

// TODO(#180): добавить сортировку артистов по имени
void LikedArtistsModel::setArtists(const QList<Artist> &artists) {
  beginResetModel();
  m_artists = artists;
  endResetModel();
  emit countChanged();
}

void LikedArtistsModel::clear() {
  if (m_artists.isEmpty()) return;
  beginResetModel();
  m_artists.clear();
  endResetModel();
  emit countChanged();
}

Artist LikedArtistsModel::artistAt(int index) const {
  if (index < 0 || index >= m_artists.size()) return {};
  return m_artists.at(index);
}

int LikedArtistsModel::count() const {
  return m_artists.size();
}
