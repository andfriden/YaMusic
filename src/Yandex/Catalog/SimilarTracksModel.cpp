#include "SimilarTracksModel.h"

SimilarTracksModel::SimilarTracksModel(
    QObject *parent)
    : TrackListModelBase(parent)
{
}

QVariant SimilarTracksModel::data(
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

        case SourceIndexRole:
            return index.row();

        case TrackIdRole:
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

        case CoverUriRole:
            return track.coverUri;

        case DurationMsRole:
            return track.durationMs;

        default:
            return {};
    }
}

QHash<int, QByteArray>
SimilarTracksModel::roleNames() const
{
    return {
        {SourceIndexRole, "sourceIndex"},
        {TrackIdRole, "trackId"},
        {TitleRole, "title"},
        {ArtistRole, "artist"},
        {ArtistIdRole, "artistId"},
        {CoverUriRole, "coverUri"},
        {DurationMsRole, "durationMs"}
    };
}

void SimilarTracksModel::setSimilarTracks(
    const QList<Track> &tracks)
{
    setTracks(tracks);
}