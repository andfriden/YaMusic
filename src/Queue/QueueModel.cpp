#include "QueueModel.h"

#include "QueueService.h"


QueueModel::QueueModel(
    QueueService *queueService,
    QObject *parent)
    : QAbstractListModel(parent)
    , m_queueService(queueService)
{
    if (m_queueService == nullptr) {
        return;
    }


    connect(
        m_queueService,
        &QueueService::queueChanged,
        this,
        [this]()
        {
            reload();
        });


    connect(
        m_queueService,
        &QueueService::currentChanged,
        this,
        [this]()
        {
            updateCurrentIndex();
        });


    reload();
}


// =============================================================
// Model
// =============================================================

int QueueModel::rowCount(
    const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_tracks.count();
}


QVariant QueueModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid()) {
        return {};
    }


    if (
        index.row() < 0 ||
        index.row() >= m_tracks.size()
    ) {
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
            if (!track.artists.isEmpty()) {
                return track.artists
                    .first()
                    .name;
            }

            return "";
        }


        case ArtistIdRole:
        {
            if (!track.artists.isEmpty()) {
                return track.artists
                    .first()
                    .id;
            }

            return "";
        }


        case AlbumRole:
        {
            if (!track.albums.isEmpty()) {
                return track.albums
                    .first()
                    .title;
            }

            return "";
        }


        case AlbumIdRole:
        {
            if (!track.albums.isEmpty()) {
                return track.albums
                    .first()
                    .id;
            }

            return "";
        }


        case CoverUriRole:
            return track.coverUri;


        case DurationMsRole:
            return track.durationMs;


        case CurrentRole:
            return index.row() ==
                   m_currentIndex;
    }


    return {};
}


QHash<int, QByteArray>
QueueModel::roleNames() const
{
    QHash<int, QByteArray> roles;


    roles[IdRole] =
        "trackId";

    roles[TitleRole] =
        "title";

    roles[ArtistRole] =
        "artist";

    roles[ArtistIdRole] =
        "artistId";

    roles[AlbumRole] =
        "album";

    roles[AlbumIdRole] =
        "albumId";

    roles[CoverUriRole] =
        "coverUri";

    roles[DurationMsRole] =
        "durationMs";

    roles[CurrentRole] =
        "current";


    return roles;
}


// =============================================================
// Access
// =============================================================

Track QueueModel::trackAt(
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


QList<Track> QueueModel::tracks() const
{
    return m_tracks;
}


int QueueModel::count() const
{
    return m_tracks.count();
}


int QueueModel::currentIndex() const
{
    return m_currentIndex;
}


// =============================================================
// Synchronisation
// =============================================================

void QueueModel::reload()
{
    if (m_queueService == nullptr) {
        return;
    }


    beginResetModel();


    m_tracks =
        m_queueService->tracks();


    m_currentIndex =
        m_queueService->currentIndex();


    endResetModel();
}


void QueueModel::updateCurrentIndex()
{
    if (m_queueService == nullptr) {
        return;
    }


    const int newIndex =
        m_queueService->currentIndex();


    if (
        newIndex ==
        m_currentIndex
    ) {
        return;
    }


    const int oldIndex =
        m_currentIndex;


    m_currentIndex =
        newIndex;


    if (
        oldIndex >= 0 &&
        oldIndex < m_tracks.size()
    ) {
        emit dataChanged(
            index(
                oldIndex),
            index(
                oldIndex),
            { CurrentRole });
    }


    if (
        newIndex >= 0 &&
        newIndex < m_tracks.size()
    ) {
        emit dataChanged(
            index(
                newIndex),
            index(
                newIndex),
            { CurrentRole });
    }
}