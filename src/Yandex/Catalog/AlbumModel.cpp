#include "AlbumModel.h"

#include <QDebug>
#include <QStringList>

AlbumModel::AlbumModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
    qDebug()
        << "AlbumModel CREATED";
}

int AlbumModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    const int result =
        m_tracks.size();

    qDebug()
        << "AlbumModel::rowCount ="
        << result;

    return result;
}

QVariant AlbumModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid()) {
        qDebug()
            << "AlbumModel::data invalid index";

        return {};
    }

    const int row =
        index.row();

    if (
        row < 0 ||
        row >= m_tracks.size()
    ) {
        qDebug()
            << "AlbumModel::data invalid row:"
            << row
            << "| size:"
            << m_tracks.size();

        return {};
    }

    const Track &track =
        m_tracks.at(row);

    switch (role) {

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
        ) {
            if (!artist.name.isEmpty()) {
                names.append(
                    artist.name);
            }
        }

        return names.join(", ");
    }

    case ArtistIdRole:
        if (!track.artists.isEmpty()) {
            return track.artists
                .first()
                .id;
        }

        return {};

    case CoverUriRole:
        return track.coverUri;

    case DurationMsRole:
        return track.durationMs;

    default:
        return {};
    }
}

QHash<int, QByteArray>
AlbumModel::roleNames() const
{
    return {
        {IdRole, "trackId"},
        {TitleRole, "title"},
        {ArtistRole, "artist"},
        {ArtistIdRole, "artistId"},
        {CoverUriRole, "coverUri"},
        {DurationMsRole, "durationMs"}
    };
}

void AlbumModel::setAlbum(
    const AlbumDetails &album)
{
    qDebug()
        << "==================================================";

    qDebug()
        << "AlbumModel::setAlbum";

    qDebug()
        << "BEFORE:"
        << "model tracks ="
        << m_tracks.size();

    qDebug()
        << "INCOMING:"
        << "album id ="
        << album.album.id
        << "| title ="
        << album.album.title
        << "| service tracks ="
        << album.tracks.size();

    beginResetModel();

    m_album =
        album;

    m_tracks =
        album.tracks;

    qDebug()
        << "AFTER:"
        << "model tracks ="
        << m_tracks.size();

    for (
        int i = 0;
        i < m_tracks.size();
        ++i
    ) {
        const Track &track =
            m_tracks.at(i);

        qDebug()
            << "AlbumModel TRACK"
            << i
            << "| id:"
            << track.id
            << "| title:"
            << track.title
            << "| artists:"
            << track.artists.size()
            << "| cover:"
            << track.coverUri;
    }

    endResetModel();

    qDebug()
        << "RESET FINISHED";

    emit countChanged();
    emit albumChanged();

    qDebug()
        << "SIGNALS EMITTED"
        << "| count:"
        << count();

    qDebug()
        << "==================================================";
}

void AlbumModel::clear()
{
    qDebug()
        << "==================================================";

    qDebug()
        << "AlbumModel::clear"
        << "| old count:"
        << m_tracks.size();

    beginResetModel();

    m_album = {};
    m_tracks.clear();

    endResetModel();

    emit countChanged();
    emit albumChanged();

    qDebug()
        << "AlbumModel::clear DONE"
        << "| count:"
        << m_tracks.size();

    qDebug()
        << "==================================================";
}

Track AlbumModel::trackAt(
    int index) const
{
    if (
        index < 0 ||
        index >= m_tracks.size()
    ) {
        qDebug()
            << "AlbumModel::trackAt invalid:"
            << index
            << "| size:"
            << m_tracks.size();

        return {};
    }

    return m_tracks.at(index);
}

QList<Track>
AlbumModel::tracks() const
{
    return m_tracks;
}

int AlbumModel::count() const
{
    return m_tracks.size();
}

QString AlbumModel::title() const
{
    return m_album.album.title;
}

QString AlbumModel::coverUri() const
{
    return m_album.album.coverUri;
}

int AlbumModel::trackCount() const
{
    return m_tracks.size();
}