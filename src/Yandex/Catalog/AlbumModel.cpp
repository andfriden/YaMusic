#include "AlbumModel.h"


AlbumModel::AlbumModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


// =============================================================
// Model
// =============================================================

int AlbumModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_tracks.size();
}


QVariant AlbumModel::data(
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
        m_tracks.at(index.row());


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
AlbumModel::roleNames() const
{
    return {
        { IdRole, "trackId" },
        { TitleRole, "title" },
        { ArtistRole, "artist" },
        { ArtistIdRole, "artistId" },
        { CoverUriRole, "coverUri" },
        { DurationMsRole, "durationMs" },
        { LikedRole, "liked" }
    };
}


// =============================================================
// Album
// =============================================================

void AlbumModel::setAlbum(
    const AlbumDetails &album)
{
    beginResetModel();

    m_album =
        album;

    m_tracks =
        album.tracks;

    endResetModel();

    emit countChanged();

    emit albumChanged();
}


void AlbumModel::clear()
{
    if (m_tracks.isEmpty())
    {
        return;
    }


    beginResetModel();

    m_album =
        AlbumDetails{};

    m_tracks.clear();

    endResetModel();

    emit countChanged();

    emit albumChanged();
}


// =============================================================
// Tracks
// =============================================================

Track AlbumModel::trackAt(
    int index) const
{
    if (
        index < 0 ||
        index >= m_tracks.size()
    )
    {
        return {};
    }

    return m_tracks.at(index);
}


QList<Track>
AlbumModel::tracks() const
{
    return m_tracks;
}


int AlbumModel::count() const
{
    return m_tracks.size();
}


QString
AlbumModel::title() const
{
    return m_album.album.title;
}


QString
AlbumModel::coverUri() const
{
    return m_album.album.coverUri;
}


int
AlbumModel::trackCount() const
{
    return m_tracks.size();
}


// =============================================================
// Like state
// =============================================================

void AlbumModel::setTrackLiked(
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
