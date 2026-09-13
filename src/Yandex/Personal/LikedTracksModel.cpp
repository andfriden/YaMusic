#include "LikedTracksModel.h"

LikedTracksModel::LikedTracksModel(
    QObject *parent)
    : TrackListModelBase(parent)
{
}

// Model

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

// Tracks

void LikedTracksModel::setTracks(
    const QList<Track> &tracks)
{
    QList<Track> liked =
        tracks;

    for (
        int i = 0;
        i < liked.size();
        ++i
    ) {
        liked[i].liked =
            true;
    }

    TrackListModelBase::setTracks(
        liked);
}

void LikedTracksModel::clear()
{
    TrackListModelBase::clear();
}

// Like state

void LikedTracksModel::setTrackLiked(
    const QString &trackId,
    bool liked)
{
    TrackListModelBase::setTrackLiked(
        trackId,
        liked,
        LikedRole);
}

void LikedTracksModel::removeTrack(
    const QString &trackId)
{
    removeTrackById(trackId);
}