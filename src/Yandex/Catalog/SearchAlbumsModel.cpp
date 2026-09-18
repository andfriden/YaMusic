#include "SearchAlbumsModel.h"

SearchAlbumsModel::SearchAlbumsModel(QObject *parent) : QAbstractListModel(parent) {}

int SearchAlbumsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return m_albums.size();
}

QVariant SearchAlbumsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= m_albums.size()) return {};

  const Album &album = m_albums.at(index.row());
  switch (role) {
  case IdRole:
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

QHash<int, QByteArray> SearchAlbumsModel::roleNames() const {
  return {{IdRole, "albumId"}, {TitleRole, "title"}, {CoverUriRole, "coverUri"}, {YearRole, "year"}};
}

void SearchAlbumsModel::setAlbums(const QList<Album> &albums) {
  beginResetModel();
  m_albums = albums;
  endResetModel();
}

void SearchAlbumsModel::appendAlbums(const QList<Album> &albums) {
  if (albums.isEmpty()) return;
  beginInsertRows(QModelIndex(), m_albums.size(), m_albums.size() + albums.size() - 1);
  m_albums.append(albums);
  endInsertRows();
}

void SearchAlbumsModel::clear() {
  beginResetModel();
  m_albums.clear();
  endResetModel();
}

Album SearchAlbumsModel::albumAt(int index) const {
  if (index < 0 || index >= m_albums.size()) return {};
  return m_albums.at(index);
}

int SearchAlbumsModel::count() const {
  return m_albums.size();
}