#include "PlaylistModel.h"

#include <QDebug>


PlaylistModel::PlaylistModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


// =============================================================
// Model
// =============================================================

int PlaylistModel::rowCount(
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


// =============================================================
// Playlist
// =============================================================

void PlaylistModel::setPlaylist(
    const Playlist &playlist)
{
    beginResetModel();

    m_playlist =
        playlist;

    m_tracks =
        playlist.tracks;


    // =========================================================
    // Debug
    // =========================================================

    qDebug()
        << "========================================";

    qDebug()
        << "PlaylistModel::setPlaylist";

    qDebug()
        << "Playlist title:"
        << playlist.title;

    qDebug()
        << "Tracks count:"
        << m_tracks.size();


    for (
        int i = 0;
        i < m_tracks.size();
        ++i
    )
    {
        const Track &track =
            m_tracks.at(i);


        qDebug()
            << "Playlist track"
            << i
            << "| id:" << track.id
            << "| title:" << track.title
            << "| artist count:" << track.artists.size()
            << "| album count:" << track.albums.size()
            << "| liked:" << track.liked;


        if (
            !track.artists.isEmpty()
        )
        {
            qDebug()
                << "  artist:"
                << track.artists.first().name
                << "| artistId:"
                << track.artists.first().id;
        }
        else
        {
            qDebug()
                << "  artist: <EMPTY>";
        }


        if (
            !track.albums.isEmpty()
        )
        {
            qDebug()
                << "  album:"
                << track.albums.first().title
                << "| albumId:"
                << track.albums.first().id;
        }
        else
        {
            qDebug()
                << "  album: <EMPTY>";
        }
    }


    qDebug()
        << "========================================";


    endResetModel();
}


void PlaylistModel::clear()
{
    if (
        m_tracks.isEmpty()
    )
    {
        return;
    }


    beginResetModel();

    m_playlist =
        Playlist{};

    m_tracks.clear();

    endResetModel();
}


Track PlaylistModel::trackAt(
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


QList<Track>
PlaylistModel::tracks() const
{
    return m_tracks;
}


int PlaylistModel::count() const
{
    return m_tracks.size();
}


QString
PlaylistModel::title() const
{
    return m_playlist.title;
}


int
PlaylistModel::trackCount() const
{
    return m_tracks.size();
}


// =============================================================
// Like state
// =============================================================

void PlaylistModel::setTrackLiked(
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