#include "SearchPlaylistsModel.h"

SearchPlaylistsModel::SearchPlaylistsModel(QObject *parent) : QAbstractListModel(parent) {}

int SearchPlaylistsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  return m_playlists.size();
}

QVariant SearchPlaylistsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= m_playlists.size()) {
    return {};
  }

  const PersonalPlaylist &playlist = m_playlists.at(index.row());

  switch (role) {
  case IdRole:
    return playlist.id;

  case UidRole:
    return playlist.uid;

  case KindRole:
    return playlist.kind;

  case TitleRole:
    return playlist.title;

  case DescriptionRole:
    return playlist.description;

  case CoverUriRole:
    return playlist.coverUri;

  case TrackCountRole:
    return playlist.trackCount;

  default:
    return {};
  }
}

QHash<int, QByteArray> SearchPlaylistsModel::roleNames() const {
  return {{IdRole, "id"},
          {UidRole, "uid"},
          {KindRole, "kind"},
          {TitleRole, "title"},
          {DescriptionRole, "description"},
          {CoverUriRole, "coverUri"},
          {TrackCountRole, "trackCount"}};
}

void SearchPlaylistsModel::setPlaylists(const QList<PersonalPlaylist> &playlists) {
  beginResetModel();
  m_playlists = playlists;
  endResetModel();
}

void SearchPlaylistsModel::appendPlaylists(const QList<PersonalPlaylist> &playlists) {
  if (playlists.isEmpty()) {
    return;
  }

  beginInsertRows(QModelIndex(), m_playlists.size(), m_playlists.size() + playlists.size() - 1);
  m_playlists.append(playlists);
  endInsertRows();
}

void SearchPlaylistsModel::clear() {
  beginResetModel();
  m_playlists.clear();
  endResetModel();
}

int SearchPlaylistsModel::count() const {
  return m_playlists.size();
}

PersonalPlaylist SearchPlaylistsModel::playlistAt(int index) const {
  if (index < 0 || index >= m_playlists.size()) {
    return {};
  }
  return m_playlists.at(index);
}