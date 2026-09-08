#include "LikedTracksModel.h"


LikedTracksModel::LikedTracksModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


// =============================================================
// Model
// =============================================================

int LikedTracksModel::rowCount(
    const QModelIndex &parent) const
{
    if (
        parent.isValid()
    )
    {
        return 0;
    }

    return m_tracks.size();
}


QVariant LikedTracksModel::data(
    const QModelIndex &index,
    int role) const
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
            return track.artists.isEmpty()
                ? QString()
                : track.artists.first().name;

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

        case LikedRole:
            return track.liked;
    }


    return {};
}


QHash<int, QByteArray>
LikedTracksModel::roleNames() const
{
    return {
        { TrackIdRole, "trackId" },
        { TitleRole, "title" },
        { ArtistRole, "artist" },
        { ArtistIdRole, "artistId" },
        { AlbumRole, "album" },
        { AlbumIdRole, "albumId" },
        { CoverUriRole, "coverUri" },
        { DurationMsRole, "durationMs" },
        { LikedRole, "liked" }
    };
}


// =============================================================
// Tracks
// =============================================================

void LikedTracksModel::setTracks(
    const QList<Track> &tracks)
{
    beginResetModel();

    m_tracks =
        tracks;

    for (
        Track &track :
        m_tracks
    )
    {
        track.liked =
            true;
    }

    endResetModel();
}


void LikedTracksModel::clear()
{
    if (
        m_tracks.isEmpty()
    )
    {
        return;
    }


    beginResetModel();

    m_tracks.clear();

    endResetModel();
}


QList<Track>
LikedTracksModel::tracks() const
{
    return m_tracks;
}


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


int
LikedTracksModel::count() const
{
    return m_tracks.size();
}


// =============================================================
// Like state
// =============================================================

void LikedTracksModel::setTrackLiked(
    const QString &trackId,
    bool liked)
{
    for (
        int i = 0;
        i < m_tracks.size();
        ++i
    )
    {
        if (
            m_tracks[i].id != trackId
        )
        {
            continue;
        }


        if (
            m_tracks[i].liked == liked
        )
        {
            return;
        }


        m_tracks[i].liked =
            liked;


        emit dataChanged(
            index(i),
            index(i),
            { LikedRole });

        return;
    }
}


void LikedTracksModel::removeTrack(
    const QString &trackId)
{
    for (
        int i = 0;
        i < m_tracks.size();
        ++i
    )
    {
        if (
            m_tracks[i].id != trackId
        )
        {
            continue;
        }


        beginRemoveRows(
            QModelIndex(),
            i,
            i);

        m_tracks.removeAt(
            i);

        endRemoveRows();

        return;
    }
}