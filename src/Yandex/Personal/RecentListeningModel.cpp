#include "RecentListeningModel.h"

#include <QDebug>
#include <QRandomGenerator>
#include <QStringList>

RecentListeningModel::RecentListeningModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
    qDebug()
        << "RecentListeningModel CREATED";
}

int RecentListeningModel::rowCount(
    const QModelIndex &parent) const
{
    if (
        parent.isValid()
    ) {
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
    qDebug()
        << "========================================";

    qDebug()
        << "RecentListeningModel::setTracks";

    qDebug()
        << "incoming count:"
        << tracks.size();

    for (
        int i = 0;
        i < tracks.size();
        ++i
    ) {
        const Track &track =
            tracks.at(i);

        qDebug()
            << "track"
            << i
            << "| id:"
            << track.id
            << "| title:"
            << track.title
            << "| artist count:"
            << track.artists.size()
            << "| album count:"
            << track.albums.size()
            << "| cover:"
            << track.coverUri;
    }

    beginResetModel();

    m_tracks =
        tracks;

    endResetModel();

    qDebug()
        << "stored count:"
        << m_tracks.size();

    qDebug()
        << "========================================";
}

void RecentListeningModel::clear()
{
    qDebug()
        << "RecentListeningModel::clear"
        << "| previous count:"
        << m_tracks.size();

    beginResetModel();

    m_tracks.clear();

    endResetModel();

    qDebug()
        << "RecentListeningModel::clear"
        << "| new count:"
        << m_tracks.size();
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

QVariantMap RecentListeningModel::trackDataAt(
    int index) const
{
    qDebug()
        << "RecentListeningModel::trackDataAt"
        << "| index:"
        << index
        << "| count:"
        << m_tracks.size();

    QVariantMap result;

    if (
        index < 0 ||
        index >= m_tracks.size()
    ) {
        qDebug()
            << "trackDataAt: invalid index";

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

    qDebug()
        << "trackDataAt result:"
        << result;

    return result;
}

QVariantList
RecentListeningModel::randomTrackData(
    int limit) const
{
    qDebug()
        << "========================================";

    qDebug()
        << "RecentListeningModel::randomTrackData";

    qDebug()
        << "stored count:"
        << m_tracks.size();

    qDebug()
        << "requested limit:"
        << limit;

    QVariantList result;

    if (
        m_tracks.isEmpty()
    ) {
        qDebug()
            << "randomTrackData: model is EMPTY";

        qDebug()
            << "========================================";

        return result;
    }

    if (
        limit <= 0
    ) {
        qDebug()
            << "randomTrackData: invalid limit";

        qDebug()
            << "========================================";

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

    qDebug()
        << "indexes created:"
        << indexes.size();

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

    qDebug()
        << "resultCount:"
        << resultCount;

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

        qDebug()
            << "random item:"
            << i
            << "| sourceIndex:"
            << sourceIndex
            << "| id:"
            << track.id
            << "| title:"
            << track.title
            << "| artist:"
            << artistName
            << "| album:"
            << albumTitle
            << "| cover:"
            << track.coverUri;

        result.append(
            item);
    }

    qDebug()
        << "randomTrackData result size:"
        << result.size();

    qDebug()
        << "========================================";

    return result;
}

QList<Track>
RecentListeningModel::tracks() const
{
    return m_tracks;
}

int RecentListeningModel::count() const
{
    return m_tracks.size();
}