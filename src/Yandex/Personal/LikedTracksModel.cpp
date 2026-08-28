#include "LikedTracksModel.h"


// =============================================================
// Constructor
// =============================================================

LikedTracksModel::LikedTracksModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


// =============================================================
// Row count
// =============================================================

int LikedTracksModel::rowCount(
    const QModelIndex &parent
) const
{
    if (
        parent.isValid()
    )
    {
        return 0;
    }


    return m_tracks.size();
}


// =============================================================
// Data
// =============================================================

QVariant LikedTracksModel::data(
    const QModelIndex &index,
    int role
) const
{
    if (
        !index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_tracks.size()
    )
    {
        return {};
    }


    const Track &track =
        m_tracks.at(
            index.row());


    switch (role)
    {
        case TrackIdRole:
            return track.id;


        case TitleRole:
            return track.title;


        case ArtistRole:
        {
            if (
                track.artists.isEmpty()
            )
            {
                return {};
            }


            return track.artists
                .first()
                .name;
        }


        case ArtistIdRole:
        {
            if (
                track.artists.isEmpty()
            )
            {
                return {};
            }


            return track.artists
                .first()
                .id;
        }


        case AlbumRole:
        {
            if (
                track.albums.isEmpty()
            )
            {
                return {};
            }


            return track.albums
                .first()
                .title;
        }


        case AlbumIdRole:
        {
            if (
                track.albums.isEmpty()
            )
            {
                return {};
            }


            return track.albums
                .first()
                .id;
        }


        case CoverUriRole:
            return track.coverUri;


        case DurationMsRole:
            return track.durationMs;


        default:
            return {};
    }
}


// =============================================================
// Role names
// =============================================================

QHash<int, QByteArray>
LikedTracksModel::roleNames() const
{
    return {
        {
            TrackIdRole,
            "trackId"
        },

        {
            TitleRole,
            "title"
        },

        {
            ArtistRole,
            "artist"
        },

        {
            ArtistIdRole,
            "artistId"
        },

        {
            AlbumRole,
            "album"
        },

        {
            AlbumIdRole,
            "albumId"
        },

        {
            CoverUriRole,
            "coverUri"
        },

        {
            DurationMsRole,
            "durationMs"
        }
    };
}


// =============================================================
// Set tracks
// =============================================================

void LikedTracksModel::setTracks(
    const QList<Track> &tracks)
{
    beginResetModel();

    m_tracks =
        tracks;

    endResetModel();
}


// =============================================================
// Clear
// =============================================================

void LikedTracksModel::clear()
{
    beginResetModel();

    m_tracks.clear();

    endResetModel();
}


// =============================================================
// Tracks
// =============================================================

QList<Track>
LikedTracksModel::tracks() const
{
    return m_tracks;
}


// =============================================================
// Track at
// =============================================================

Track
LikedTracksModel::trackAt(
    int index) const
{
    if (
        index < 0 ||
        index >= m_tracks.size()
    )
    {
        return {};
    }


    return m_tracks.at(
        index);
}


// =============================================================
// Count
// =============================================================

int
LikedTracksModel::count() const
{
    return m_tracks.size();
}