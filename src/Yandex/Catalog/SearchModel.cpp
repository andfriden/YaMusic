#include "SearchModel.h"

#include <QStringList>

SearchModel::SearchModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SearchModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_tracks.size();
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

            for (const Artist &artist : track.artists) {

                if (!artist.name.isEmpty()) {
                    artistNames.append(
                        artist.name);
                }
            }

            return artistNames.join(", ");
        }

        case AlbumRole:
            if (!track.albums.isEmpty()) {
                return track.albums.first().title;
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
        {AlbumRole, "album"},
        {CoverUriRole, "coverUri"},
        {DurationMsRole, "durationMs"}
    };
}

void SearchModel::setResults(
    const SearchResults &results)
{
    beginResetModel();

    m_tracks =
        results.tracks;

    endResetModel();
}

void SearchModel::clear()
{
    beginResetModel();

    m_tracks.clear();

    endResetModel();
}

Track SearchModel::trackAt(
    int index) const
{
    if (index < 0 ||
        index >= m_tracks.size()) {
        return {};
        }

    return m_tracks.at(index);
}