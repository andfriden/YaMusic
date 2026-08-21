#include "SearchModel.h"

#include <QHash>
#include <QStringList>

// Creates an empty search model.
SearchModel::SearchModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

// Returns the number of search results.
int SearchModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_tracks.size();
}

// Returns data for a specific search result.
QVariant SearchModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_tracks.size()) {
        return {};
    }

    const SearchTrack &track =
        m_tracks.at(index.row());

    switch (role) {
    case IdRole:
        return track.id;

    case TitleRole:
        return track.title;

    case ArtistRole:
    {
        QStringList artistNames;

        for (const SearchArtist &artist : track.artists) {
            if (!artist.name.isEmpty()) {
                artistNames.append(artist.name);
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

// Returns names for the model roles exposed to QML.
QHash<int, QByteArray> SearchModel::roleNames() const
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

// Replaces the current results with new search results.
void SearchModel::setResults(
    const SearchResults &results)
{
    beginResetModel();

    m_tracks = results.tracks;

    endResetModel();
}

// Removes all search results.
void SearchModel::clear()
{
    beginResetModel();

    m_tracks.clear();

    endResetModel();
}

// Returns a search track by index.
SearchTrack SearchModel::trackAt(int index) const
{
    if (index < 0 ||
        index >= m_tracks.size()) {
        return {};
    }

    return m_tracks.at(index);
}