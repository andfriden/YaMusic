#include "SearchModel.h"
#include <QStringList>

SearchModel::SearchModel(
    QObject *parent)
    : TrackListModelBase(parent)
{
}

QVariant SearchModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_tracks.size()) {

        return {};
    }

    const Track &track =
        m_tracks.at(index.row());

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

            return artistNames.join(", ");
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
SearchModel::roleNames() const
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

void SearchModel::setResults(
    const SearchResults &results)
{
    setTracks(
        results.tracks);
}


void SearchModel::appendResults(
    const QList<Track> &tracks)
{
    if (tracks.isEmpty()) {
        return;
    }

    beginInsertRows(
        QModelIndex(),
        m_tracks.size(),
        m_tracks.size() + tracks.size() - 1);

    m_tracks.append(
        tracks);

    endInsertRows();

    emit countChanged();
}