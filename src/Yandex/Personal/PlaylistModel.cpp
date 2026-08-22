#include "PlaylistModel.h"

#include <QStringList>

PlaylistModel::PlaylistModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}

int PlaylistModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_tracks.size();
}

QVariant PlaylistModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_tracks.size()) {

        return {};
    }

    const Track &track =
        m_tracks.at(
            index.row());

    switch (role) {

    case IdRole:
        return track.id;

    case TitleRole:
        return track.title;

    case ArtistRole:
    {
        QStringList artistNames;

        for (const Artist &artist :
             track.artists) {

            if (!artist.name.isEmpty()) {
                artistNames.append(
                    artist.name);
            }
        }

        return artistNames.join(
            ", ");
    }

    case ArtistIdRole:

        if (!track.artists.isEmpty()) {
            return track.artists
                .first()
                .id;
        }

        return QString();

    case AlbumRole:

        if (!track.albums.isEmpty()) {
            return track.albums
                .first()
                .title;
        }

        return QString();

    case CoverUriRole:
        return track.coverUri;

    case DurationMsRole:
        return track.durationMs;

    default:
        return {};
    }
}

QHash<int, QByteArray>
PlaylistModel::roleNames() const
{
    return {
        {IdRole, "trackId"},
        {TitleRole, "title"},
        {ArtistRole, "artist"},
        {ArtistIdRole, "artistId"},
        {AlbumRole, "album"},
        {CoverUriRole, "coverUri"},
        {DurationMsRole, "durationMs"}
    };
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

    m_playlist = {};

    m_tracks.clear();

    endResetModel();
}

Track PlaylistModel::trackAt(
    int index) const
{
    if (index < 0 ||
        index >= m_tracks.size()) {

        return {};
    }

    return m_tracks.at(
        index);
}

int PlaylistModel::count() const
{
    return m_tracks.size();
}

QString PlaylistModel::title() const
{
    return m_playlist.title;
}

int PlaylistModel::trackCount() const
{
    return m_playlist.trackCount;
}