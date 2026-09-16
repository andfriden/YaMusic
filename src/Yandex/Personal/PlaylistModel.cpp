#include "PlaylistModel.h"

PlaylistModel::PlaylistModel(
    QObject *parent)
    : TrackListModelBase(parent)
{
}

// Model

QVariant PlaylistModel::data(
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
        case IdRole:
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
PlaylistModel::roleNames() const
{
    return {
        { IdRole, "trackId" },
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

// Playlist

void PlaylistModel::setPlaylist(
    const Playlist &playlist)
{
    m_playlist =
        playlist;

    setTracks(
        playlist.tracks);
}

void PlaylistModel::clear()
{
    TrackListModelBase::clear();

    m_playlist =
        Playlist{};
}

QString
PlaylistModel::title() const
{
    return m_playlist.title;
}

int
PlaylistModel::kind() const
{
    return m_playlist.kind;
}

int
PlaylistModel::trackCount() const
{
    return m_tracks.size();
}

// Like state

void PlaylistModel::setTrackLiked(
    const QString &trackId,
    bool liked)
{
    TrackListModelBase::setTrackLiked(
        trackId,
        liked,
        LikedRole);
}