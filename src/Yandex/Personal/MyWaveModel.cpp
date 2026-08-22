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
    if (parent.isValid()) {
        return 0;
    }

    return m_tracks.size();
}

QVariant MyWaveModel::data(
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
MyWaveModel::roleNames() const
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

void MyWaveModel::setTracks(
    const QList<Track> &tracks)
{
    beginResetModel();

    m_tracks = tracks;

    endResetModel();
}

void MyWaveModel::appendTracks(
    const QList<Track> &tracks)
{
    if (tracks.isEmpty()) {
        return;
    }

    const int first =
        m_tracks.size();

    const int last =
        first + tracks.size() - 1;

    beginInsertRows(
        QModelIndex(),
        first,
        last);

    m_tracks.append(
        tracks);

    endInsertRows();
}

void MyWaveModel::clear()
{
    beginResetModel();

    m_tracks.clear();

    endResetModel();
}

Track MyWaveModel::trackAt(
    int index) const
{
    if (index < 0 ||
        index >= m_tracks.size()) {

        return {};
    }

    return m_tracks.at(index);
}

Track MyWaveModel::lastTrack() const
{
    if (m_tracks.isEmpty()) {
        return {};
    }

    return m_tracks.last();
}

int MyWaveModel::count() const
{
    return m_tracks.size();
}