#include "AlbumModel.h"

#include <QStringList>

AlbumModel::AlbumModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}

int AlbumModel::rowCount(
    const QModelIndex &parent) const
{
    if (
        parent.isValid()
    ) {
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
        QStringList names;

        for (
            const Artist &artist :
            track.artists
        ) {

            if (
                !artist.name.isEmpty()
            ) {

                names.append(
                    artist.name);
            }
        }

        return names.join(
            ", ");
    }

    case ArtistIdRole:

        if (
            !track.artists.isEmpty()
        ) {

            return track.artists
                .first()
                .id;
        }

        return {};

    case CoverUriRole:
        return track.coverUri;

    case DurationMsRole:
        return track.durationMs;

    default:
        return {};
    }
}

QHash<int, QByteArray>
AlbumModel::roleNames() const
{
    return {
        {IdRole, "trackId"},
        {TitleRole, "title"},
        {ArtistRole, "artist"},
        {ArtistIdRole, "artistId"},
        {CoverUriRole, "coverUri"},
        {DurationMsRole, "durationMs"}
    };
}

void AlbumModel::setAlbum(
    const AlbumDetails &album)
{
    beginResetModel();

    m_album =
        album;

    m_tracks =
        album.tracks;

    endResetModel();
}

void AlbumModel::clear()
{
    beginResetModel();

    m_album = {};

    m_tracks.clear();

    endResetModel();
}

Track AlbumModel::trackAt(
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

QList<Track>
AlbumModel::tracks() const
{
    return m_tracks;
}

int AlbumModel::count() const
{
    return m_tracks.size();
}

QString AlbumModel::title() const
{
    return m_album.album.title;
}

QString AlbumModel::coverUri() const
{
    return m_album.album.coverUri;
}

int AlbumModel::trackCount() const
{
    return m_album.trackCount;
}