#include "GenreStationModel.h"

GenreStationModel::GenreStationModel(
    QObject *parent)
    : TrackListModelBase(parent)
{
}

QVariant GenreStationModel::data(
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
            if (track.artists.isEmpty()) {
                return QString();
            }

            QStringList names;

            for (const Artist &artist : track.artists) {
                if (!artist.name.isEmpty()) {
                    names.append(artist.name);
                }
            }

            return names.join(", ");
        }

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

        default:
            return {};
    }
}

QHash<int, QByteArray>
GenreStationModel::roleNames() const
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

void GenreStationModel::appendTracks(
    const QList<Track> &tracks)
{
    if (tracks.isEmpty()) {
        return;
    }

    const int firstNew =
        m_tracks.size();
    const int lastNew =
        firstNew + tracks.size() - 1;

    beginInsertRows(
        QModelIndex(),
        firstNew,
        lastNew);

    m_tracks.append(
        tracks);

    endInsertRows();

    emit countChanged();
}

QString GenreStationModel::lastTrackId() const
{
    if (m_tracks.isEmpty()) {
        return {};
    }

    return m_tracks.last().id;
}