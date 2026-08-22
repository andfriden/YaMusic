#include "ArtistModel.h"

#include <QStringList>

ArtistModel::ArtistModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}

int ArtistModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_tracks.size();
}

QVariant ArtistModel::data(
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
ArtistModel::roleNames() const
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

void ArtistModel::setArtist(
    const ArtistDetails &artist)
{
    beginResetModel();

    m_artist =
        artist;

    m_tracks =
        artist.tracks;

    endResetModel();
}

void ArtistModel::clear()
{
    beginResetModel();

    m_artist = {};

    m_tracks.clear();

    endResetModel();
}

Track ArtistModel::trackAt(
    int index) const
{
    if (index < 0 ||
        index >= m_tracks.size()) {

        return {};
    }

    return m_tracks.at(
        index);
}

int ArtistModel::count() const
{
    return m_tracks.size();
}

QString ArtistModel::id() const
{
    return m_artist.id;
}

QString ArtistModel::name() const
{
    return m_artist.name;
}

QString ArtistModel::coverUri() const
{
    return m_artist.coverUri;
}

QString ArtistModel::description() const
{
    return m_artist.description;
}

QString ArtistModel::genres() const
{
    return m_artist.genres.join(
        ", ");
}