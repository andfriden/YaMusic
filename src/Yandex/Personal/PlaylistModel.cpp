#include "PlaylistModel.h"

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
}


int PlaylistModel::rowCount(
    const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_tracks.count();
}


QVariant PlaylistModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid())
        return {};


    if (index.row() < 0 ||
        index.row() >= m_tracks.size())
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
        {
            if (!track.artists.isEmpty())
                return track.artists.first().name;

            return "";
        }


        case ArtistIdRole:
        {
            if (!track.artists.isEmpty())
                return track.artists.first().id;

            return "";
        }


        case AlbumRole:
        {
            if (!track.albums.isEmpty())
                return track.albums.first().title;

            return "";
        }


        case AlbumIdRole:
        {
            if (!track.albums.isEmpty())
                return track.albums.first().id;

            return "";
        }


        case CoverUriRole:
            return track.coverUri;


        case DurationMsRole:
            return track.durationMs;
    }


    return {};
}


QHash<int, QByteArray>
PlaylistModel::roleNames() const
{
    QHash<int, QByteArray> roles;


    roles[IdRole] =
        "trackId";

    roles[TitleRole] =
        "title";

    roles[ArtistRole] =
        "artist";

    roles[ArtistIdRole] =
        "artistId";

    roles[AlbumRole] =
        "album";

    roles[AlbumIdRole] =
        "albumId";

    roles[CoverUriRole] =
        "coverUri";

    roles[DurationMsRole] =
        "durationMs";


    return roles;
}


void PlaylistModel::setPlaylist(
    const Playlist &playlist)
{
    beginResetModel();


    m_playlist =
        playlist;


    m_tracks =
        playlist.tracks;


    endResetModel();
}


void PlaylistModel::clear()
{
    beginResetModel();

    m_playlist =
        Playlist();

    m_tracks.clear();

    endResetModel();
}


Track PlaylistModel::trackAt(
    int index) const
{
    if (index < 0 ||
        index >= m_tracks.size())
    {
        return {};
    }


    return m_tracks.at(index);
}


QList<Track> PlaylistModel::tracks() const
{
    return m_tracks;
}


int PlaylistModel::count() const
{
    return m_tracks.count();
}


QString PlaylistModel::title() const
{
    return m_playlist.title;
}


int PlaylistModel::trackCount() const
{
    return m_tracks.count();
}