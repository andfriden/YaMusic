#include "LibraryPlaylistsModel.h"


LibraryPlaylistsModel::LibraryPlaylistsModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


// =============================================================
// Row count
// =============================================================

int LibraryPlaylistsModel::rowCount(
    const QModelIndex &parent
) const
{
    if (
        parent.isValid()
    )
    {
        return 0;
    }


    return m_playlists.size();
}


// =============================================================
// Data
// =============================================================

QVariant LibraryPlaylistsModel::data(
    const QModelIndex &index,
    int role
) const
{
    if (
        !index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_playlists.size()
    )
    {
        return {};
    }


    const PersonalPlaylist &playlist =
        m_playlists.at(
            index.row());


    switch (role)
    {
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


// =============================================================
// Roles
// =============================================================

QHash<int, QByteArray>
LibraryPlaylistsModel::roleNames() const
{
    return {
        {
            IdRole,
            "id"
        },
        {
            UidRole,
            "uid"
        },
        {
            KindRole,
            "kind"
        },
        {
            TitleRole,
            "title"
        },
        {
            DescriptionRole,
            "description"
        },
        {
            CoverUriRole,
            "coverUri"
        },
        {
            TrackCountRole,
            "trackCount"
        }
    };
}


// =============================================================
// Set playlists
// =============================================================

void LibraryPlaylistsModel::setPlaylists(
    const QList<PersonalPlaylist> &playlists
)
{
    beginResetModel();

    m_playlists =
        playlists;

    endResetModel();
}


// =============================================================
// Clear
// =============================================================

void LibraryPlaylistsModel::clear()
{
    beginResetModel();

    m_playlists.clear();

    endResetModel();
}


// =============================================================
// Count
// =============================================================

int LibraryPlaylistsModel::count() const
{
    return m_playlists.size();
}


// =============================================================
// Playlist at
// =============================================================

PersonalPlaylist
LibraryPlaylistsModel::playlistAt(
    int index) const
{
    if (
        index < 0 ||
        index >= m_playlists.size()
    )
    {
        return {};
    }


    return m_playlists.at(
        index);
}