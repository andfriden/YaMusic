#include "MyWaveModel.h"

#include <QStringList>

MyWaveModel::MyWaveModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}

int MyWaveModel::rowCount(
    const QModelIndex &parent) const
{
    if (
        parent.isValid()
    ) {
        return 0;
    }

    return m_tracks.size();
}

QVariant MyWaveModel::data(
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

    case CoverUriRole:
        return track.coverUri;

    case DurationMsRole:
        return track.durationMs;

    default:
        return {};
    }
}

QHash<int, QByteArray>
MyWaveModel::roleNames() const
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
            CoverUriRole,
            "coverUri"
        },
        {
            DurationMsRole,
            "durationMs"
        }
    };
}

void MyWaveModel::setTracks(
    const QList<Track> &tracks)
{
    beginResetModel();

    m_tracks =
        tracks;

    endResetModel();

    emit countChanged();
}

void MyWaveModel::appendTracks(
    const QList<Track> &tracks)
{
    if (
        tracks.isEmpty()
    ) {
        return;
    }

    QList<Track> newTracks;

    for (
        const Track &track :
        tracks
    ) {

        if (
            track.id.isEmpty()
        ) {
            continue;
        }

        bool alreadyExists =
            false;

        for (
            const Track &existing :
            m_tracks
        ) {

            if (
                existing.id ==
                track.id
            ) {

                alreadyExists =
                    true;

                break;
            }
        }

        if (
            !alreadyExists
        ) {

            newTracks.append(
                track);
        }
    }

    if (
        newTracks.isEmpty()
    ) {
        return;
    }

    const int first =
        m_tracks.size();

    const int last =
        first +
        newTracks.size() -
        1;

    beginInsertRows(
        QModelIndex(),
        first,
        last);

    m_tracks.append(
        newTracks);

    endInsertRows();

    emit countChanged();
}

void MyWaveModel::clear()
{
    if (
        m_tracks.isEmpty()
    ) {
        return;
    }

    beginResetModel();

    m_tracks.clear();

    endResetModel();

    emit countChanged();
}

Track MyWaveModel::trackAt(
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

Track MyWaveModel::lastTrack() const
{
    if (
        m_tracks.isEmpty()
    ) {
        return {};
    }

    return m_tracks.last();
}

QList<Track> MyWaveModel::tracks() const
{
    return m_tracks;
}

int MyWaveModel::count() const
{
    return m_tracks.size();
}