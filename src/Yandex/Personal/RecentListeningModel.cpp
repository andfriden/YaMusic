#include "RecentListeningModel.h"

#include <QStringList>

RecentListeningModel::RecentListeningModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}

int RecentListeningModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_tracks.size();
}

QVariant RecentListeningModel::data(
    const QModelIndex &index,
    int role) const
{
    if (
        !index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_tracks.size()
    ) {
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

        for (
            const Artist &artist :
            track.artists
        ) {

            if (
                !artist.name.isEmpty()
            ) {

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

    case AlbumIdRole:
        if (!track.albums.isEmpty()) {
            return track.albums
                .first()
                .id;
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
RecentListeningModel::roleNames() const
{
    return {
        {IdRole, "trackId"},
        {TitleRole, "title"},
        {ArtistRole, "artist"},
        {ArtistIdRole, "artistId"},
        {AlbumRole, "album"},
        {AlbumIdRole, "albumId"},
        {CoverUriRole, "coverUri"},
        {DurationMsRole, "durationMs"}
    };
}

void RecentListeningModel::setTracks(
    const QList<Track> &tracks)
{
    beginResetModel();

    m_tracks =
        tracks;

    endResetModel();
}

void RecentListeningModel::clear()
{
    beginResetModel();

    m_tracks.clear();

    endResetModel();
}

Track RecentListeningModel::trackAt(
    int index) const
{
    if (
        index < 0 ||
        index >= m_tracks.size()
    ) {
        return {};
    }

    return m_tracks.at(
        index);
}

int RecentListeningModel::count() const
{
    return m_tracks.size();
}