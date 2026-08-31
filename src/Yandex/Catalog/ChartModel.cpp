#include "ChartModel.h"


ChartModel::ChartModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


int ChartModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_tracks.size();
}


QVariant ChartModel::data(
    const QModelIndex &index,
    int role) const
{
    if (
        !index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_tracks.size()
    )
    {
        return {};
    }


    const Track &track =
        m_tracks.at(
            index.row());


    switch (role)
    {
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
            )
            {
                if (!artist.name.isEmpty())
                {
                    names.append(
                        artist.name);
                }
            }

            return names.join(
                ", ");
        }


        case ArtistIdRole:
        {
            if (track.artists.isEmpty())
            {
                return QString();
            }

            return track.artists
                .first()
                .id;
        }


        case CoverUriRole:
            return track.coverUri;


        case DurationMsRole:
            return track.durationMs;


        case PositionRole:
            return index.row() + 1;


        default:
            return {};
    }
}


QHash<int, QByteArray>
ChartModel::roleNames() const
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
            CoverUriRole,
            "coverUri"
        },
        {
            DurationMsRole,
            "durationMs"
        },
        {
            PositionRole,
            "position"
        }
    };
}


void ChartModel::setTracks(
    const QList<Track> &tracks)
{
    beginResetModel();

    m_tracks =
        tracks;

    endResetModel();
}


void ChartModel::clear()
{
    beginResetModel();

    m_tracks.clear();

    endResetModel();
}


Track ChartModel::trackAt(
    int index) const
{
    if (
        index < 0 ||
        index >= m_tracks.size()
    )
    {
        return {};
    }

    return m_tracks.at(
        index);
}


int ChartModel::count() const
{
    return m_tracks.size();
}