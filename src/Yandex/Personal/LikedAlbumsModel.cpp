#include "LikedAlbumsModel.h"

LikedAlbumsModel::LikedAlbumsModel(QObject *parent) : QAbstractListModel(parent) {}

int LikedAlbumsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return m_albums.size();
}

QVariant LikedAlbumsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= m_albums.size()) return {};
  const Album &album = m_albums.at(index.row());

  switch (role) {
  case AlbumIdRole:
    return album.id;
  case TitleRole:
    return album.title;
  case CoverUriRole:
    return album.coverUri;
  case YearRole:
    return album.year;
  default:
    return {};
  }
}

QHash<int, QByteArray> LikedAlbumsModel::roleNames() const {
  return {{AlbumIdRole, "albumId"},
          {TitleRole, "title"},
          {CoverUriRole, "coverUri"},
          {YearRole, "year"}};
}

// TODO(#180): добавить сортировку альбомов по году
void LikedAlbumsModel::setAlbums(const QList<Album> &albums) {
  beginResetModel();
  m_albums = albums;
  endResetModel();
  emit countChanged();
}

void LikedAlbumsModel::clear() {
  if (m_albums.isEmpty()) return;
  beginResetModel();
  m_albums.clear();
  endResetModel();
  emit countChanged();
}

Album LikedAlbumsModel::albumAt(int index) const {
  if (index < 0 || index >= m_albums.size()) return {};
  return m_albums.at(index);
}

int LikedAlbumsModel::count() const {
  return m_albums.size();
}
