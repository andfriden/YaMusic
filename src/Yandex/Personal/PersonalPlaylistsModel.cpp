#include "PersonalPlaylistsModel.h"

PersonalPlaylistsModel::PersonalPlaylistsModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}

int PersonalPlaylistsModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_playlists.size();
}

QVariant PersonalPlaylistsModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_playlists.size()) {

        return {};
    }

    const PersonalPlaylist &playlist =
        m_playlists.at(index.row());

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

    case PreviewDescriptionRole:
        return playlist.previewDescription;

    case CoverUriRole:
        return playlist.coverUri;

    case TrackCountRole:
        return playlist.trackCount;

    case GeneratedPlaylistTypeRole:
        return playlist.generatedPlaylistType;

    default:
        return {};
    }
}

QHash<int, QByteArray>
PersonalPlaylistsModel::roleNames() const
{
    return {
        {IdRole, "playlistId"},
        {UidRole, "uid"},
        {KindRole, "kind"},
        {TitleRole, "title"},
        {DescriptionRole, "description"},
        {PreviewDescriptionRole, "previewDescription"},
        {CoverUriRole, "coverUri"},
        {TrackCountRole, "trackCount"},
        {GeneratedPlaylistTypeRole, "generatedPlaylistType"}
    };
}

void PersonalPlaylistsModel::setPlaylists(
    const QList<PersonalPlaylist> &playlists)
{
    beginResetModel();

    m_playlists = playlists;

    endResetModel();
}

void PersonalPlaylistsModel::clear()
{
    beginResetModel();

    m_playlists.clear();

    endResetModel();
}

PersonalPlaylist
PersonalPlaylistsModel::playlistAt(
    int index) const
{
    if (index < 0 ||
        index >= m_playlists.size()) {

        return {};
    }

    return m_playlists.at(index);
}

int PersonalPlaylistsModel::count() const
{
    return m_playlists.size();
}