#include "RecentListeningModel.h"
#include <QRandomGenerator>
#include <QStringList>

RecentListeningModel::RecentListeningModel(
    QObject *parent)
    : TrackListModelBase(parent)
{
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

        if (
            !track.artists.isEmpty()
        ) {
            return track.artists
                .first()
                .id;
        }

        return QString();

    case AlbumRole:

        if (
            !track.albums.isEmpty()
        ) {
            return track.albums
                .first()
                .title;
        }

        return QString();

    case AlbumIdRole:

        if (
            !track.albums.isEmpty()
        ) {
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
        {
            IdRole,
            "trackId"
        },
        {
            TitleRole,
            "title"
        },
        {
            ArtistRole,
            "artist"
        },
        {
            ArtistIdRole,
            "artistId"
        },
        {
            AlbumRole,
            "album"
        },
        {
            AlbumIdRole,
            "albumId"
        },
        {
            CoverUriRole,
            "coverUri"
        },
        {
            DurationMsRole,
            "durationMs"
        }
    };
}

void RecentListeningModel::setTracks(
    const QList<Track> &tracks)
{
    TrackListModelBase::setTracks(
        tracks);
}

QVariantMap RecentListeningModel::trackDataAt(
    int index) const
{

    QVariantMap result;

    if (
        index < 0 ||
        index >= m_tracks.size()
    ) {

        return result;
    }

    const Track &track =
        m_tracks.at(
            index);

    QString artistName;
    QString artistId;

    if (
        !track.artists.isEmpty()
    ) {
        artistName =
            track.artists
                .first()
                .name;

        artistId =
            track.artists
                .first()
                .id;
    }

    QString albumTitle;
    QString albumId;

    if (
        !track.albums.isEmpty()
    ) {
        albumTitle =
            track.albums
                .first()
                .title;

        albumId =
            track.albums
                .first()
                .id;
    }

    result.insert(
        "sourceIndex",
        index);

    result.insert(
        "trackId",
        track.id);

    result.insert(
        "title",
        track.title);

    result.insert(
        "artist",
        artistName);

    result.insert(
        "artistId",
        artistId);

    result.insert(
        "album",
        albumTitle);

    result.insert(
        "albumId",
        albumId);

    result.insert(
        "coverUri",
        track.coverUri);

    result.insert(
        "durationMs",
        track.durationMs);

    return result;
}

QVariantList
RecentListeningModel::randomTrackData(
    int limit) const
{

    QVariantList result;

    if (
        m_tracks.isEmpty()
    ) {

        return result;
    }

    if (
        limit <= 0
    ) {

        return result;
    }

    QList<int> indexes;

    indexes.reserve(
        m_tracks.size());

    for (
        int i = 0;
        i < m_tracks.size();
        ++i
    ) {
        indexes.append(i);
    }

    /*
     * Fisher-Yates shuffle.
     */

    for (
        int i = indexes.size() - 1;
        i > 0;
        --i
    ) {
        const int j =
            QRandomGenerator::global()
                ->bounded(i + 1);

        indexes.swapItemsAt(
            i,
            j);
    }

    const int resultCount =
        qMin(
            limit,
            indexes.size());

    for (
        int i = 0;
        i < resultCount;
        ++i
    ) {
        const int sourceIndex =
            indexes.at(i);

        const Track &track =
            m_tracks.at(
                sourceIndex);

        QVariantMap item;

        QString artistName;
        QString artistId;

        if (
            !track.artists.isEmpty()
        ) {
            artistName =
                track.artists
                    .first()
                    .name;

            artistId =
                track.artists
                    .first()
                    .id;
        }

        QString albumTitle;
        QString albumId;

        if (
            !track.albums.isEmpty()
        ) {
            albumTitle =
                track.albums
                    .first()
                    .title;

            albumId =
                track.albums
                    .first()
                    .id;
        }

        item.insert(
            "sourceIndex",
            sourceIndex);

        item.insert(
            "trackId",
            track.id);

        item.insert(
            "title",
            track.title);

        item.insert(
            "artist",
            artistName);

        item.insert(
            "artistId",
            artistId);

        item.insert(
            "album",
            albumTitle);

        item.insert(
            "albumId",
            albumId);

        item.insert(
            "coverUri",
            track.coverUri);

        item.insert(
            "durationMs",
            track.durationMs);

        result.append(
            item);
    }

    return result;
}