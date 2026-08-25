#include "AlbumController.h"

#include "../Playback/PlaybackController.h"
#include "../Queue/QueueService.h"

#include "../Yandex/Catalog/AlbumService.h"
#include "../Yandex/Catalog/ArtistService.h"

#include <QDebug>

AlbumController::AlbumController(
    AlbumService *albumService,
    ArtistService *artistService,
    PlaybackController *playbackController,
    QObject *parent)
    : QObject(parent)
    , m_albumService(
          albumService)
    , m_artistService(
          artistService)
    , m_playbackController(
          playbackController)
    , m_albumModel(
          new AlbumModel(this))
    , m_otherAlbumsModel(
          new ArtistAlbumsModel(this))
{
    qDebug()
        << "==================================================";

    qDebug()
        << "AlbumController CREATED"
        << "| album service:"
        << m_albumService
        << "| artist service:"
        << m_artistService
        << "| playback:"
        << m_playbackController
        << "| album model:"
        << m_albumModel
        << "| other albums model:"
        << m_otherAlbumsModel;

    qDebug()
        << "==================================================";

    if (
        m_albumService == nullptr
    ) {
        qDebug()
            << "AlbumController:"
            << "AlbumService is null";

        return;
    }

    connect(
        m_albumService,
        &AlbumService::albumReceived,
        this,
        [this](
            const AlbumDetails &album) {

            qDebug()
                << "##################################################";

            qDebug()
                << "AlbumController::albumReceived";

            qDebug()
                << "SERVICE:"
                << "id ="
                << album.album.id
                << "| title ="
                << album.album.title
                << "| tracks ="
                << album.tracks.size();

            m_loading =
                false;

            m_albumId =
                album.album.id;

            m_albumModel
                ->setAlbum(
                    album);

            qDebug()
                << "AlbumController MODEL:"
                << "count ="
                << m_albumModel->count()
                << "| tracks ="
                << m_albumModel
                       ->tracks()
                       .size();

            loadOtherAlbumsForCurrentArtist(
                album);

            emit loadingChanged();

            emit albumChanged();

            emit statusChanged(
                QString(
                    "Альбом загружен: %1")
                    .arg(
                        album.album.title));

            qDebug()
                << "AlbumController SIGNALS:"
                << "| loading:"
                << m_loading
                << "| model count:"
                << m_albumModel->count()
                << "| other albums:"
                << m_otherAlbumsModel->count();

            qDebug()
                << "##################################################";
        });

    connect(
        m_albumService,
        &AlbumService::errorOccurred,
        this,
        [this](
            const QString &message) {

            m_loading =
                false;

            qDebug()
                << "AlbumController ERROR:"
                << message;

            emit loadingChanged();

            emit statusChanged(
                QString(
                    "Ошибка загрузки альбома: %1")
                    .arg(
                        message));
        });

    if (
        m_artistService == nullptr
    ) {
        return;
    }

    connect(
        m_artistService,
        &ArtistService::artistAlbumsReceived,
        this,
        [this](
            const QList<Album> &albums) {

            if (
                m_currentArtistId.isEmpty()
            ) {
                return;
            }

            QList<Album> filteredAlbums;

            for (
                const Album &album :
                albums
            ) {
                if (
                    album.id.isEmpty()
                ) {
                    continue;
                }

                if (
                    album.id ==
                    m_albumId
                ) {
                    continue;
                }

                filteredAlbums.append(
                    album);
            }

            m_otherAlbumsModel
                ->setAlbums(
                    filteredAlbums);

            qDebug()
                << "AlbumController other albums loaded:"
                << "| artist:"
                << m_currentArtistId
                << "| albums:"
                << filteredAlbums.size();
        });
}

void AlbumController::loadOtherAlbumsForCurrentArtist(
    const AlbumDetails &album)
{
    m_currentArtistId.clear();

    m_otherAlbumsModel
        ->clear();

    if (
        m_artistService == nullptr
    ) {
        qDebug()
            << "AlbumController:"
            << "ArtistService is null";

        return;
    }

    for (
        const Track &track :
        album.tracks
    ) {
        if (
            track.artists.isEmpty()
        ) {
            continue;
        }

        const Artist &artist =
            track.artists.first();

        const QString artistId =
            artist.id.trimmed();

        if (
            artistId.isEmpty()
        ) {
            continue;
        }

        m_currentArtistId =
            artistId;

        qDebug()
            << "AlbumController resolving artist albums:"
            << "artist id:"
            << m_currentArtistId
            << "| artist:"
            << artist.name;

        m_artistService
            ->loadArtistAlbums(
                m_currentArtistId);

        return;
    }

    qDebug()
        << "AlbumController:"
        << "artist id not found in album tracks";
}

void AlbumController::loadAlbum(
    const QString &id)
{
    const QString requestedId =
        id.trimmed();

    qDebug()
        << "==================================================";

    qDebug()
        << "AlbumController::loadAlbum"
        << "| requested id:"
        << requestedId;

    if (
        requestedId.isEmpty()
    ) {
        emit statusChanged(
            "ID альбома не указан");

        return;
    }

    if (
        m_albumService == nullptr
    ) {
        emit statusChanged(
            "Сервис альбома недоступен");

        return;
    }

    if (
        m_loading
    ) {
        qDebug()
            << "AlbumController:"
            << "already loading";

        return;
    }

    m_loading =
        true;

    emit loadingChanged();

    m_albumId.clear();

    m_currentArtistId.clear();

    m_albumModel
        ->clear();

    m_otherAlbumsModel
        ->clear();

    emit albumChanged();

    emit statusChanged(
        QString(
            "Загрузка альбома: %1")
            .arg(
                requestedId));

    m_albumService
        ->loadAlbum(
            requestedId);

    qDebug()
        << "==================================================";
}

void AlbumController::selectAlbumTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }

    if (
        m_albumModel == nullptr
    ) {
        emit statusChanged(
            "AlbumModel недоступен");

        return;
    }

    const QList<Track> tracks =
        m_albumModel->tracks();

    if (
        tracks.isEmpty()
    ) {
        emit statusChanged(
            "В альбоме нет треков");

        return;
    }

    if (
        index < 0 ||
        index >= tracks.size()
    ) {
        emit statusChanged(
            "Некорректный индекс трека");

        return;
    }

    const Track track =
        tracks.at(index);

    if (
        track.id.isEmpty()
    ) {
        emit statusChanged(
            "Некорректный трек альбома");

        return;
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue == nullptr
    ) {
        emit statusChanged(
            "Очередь воспроизведения недоступна");

        return;
    }

    queue->clear();

    queue->addTracks(
        tracks);

    queue->setCurrentIndex(
        index);

    qDebug()
        << "Album track selected:"
        << track.title
        << "| index:"
        << index
        << "| queue tracks:"
        << tracks.size();

    emit trackSelected(
        track);

    m_playbackController
        ->playTrack(
            track);
}

void AlbumController::playAlbum()
{
    if (
        m_playbackController == nullptr
    ) {
        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }

    if (
        m_albumModel == nullptr
    ) {
        emit statusChanged(
            "AlbumModel недоступен");

        return;
    }

    const QList<Track> tracks =
        m_albumModel->tracks();

    if (
        tracks.isEmpty()
    ) {
        emit statusChanged(
            "В альбоме нет доступных треков");

        return;
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue == nullptr
    ) {
        emit statusChanged(
            "Очередь воспроизведения недоступна");

        return;
    }

    queue->clear();

    queue->addTracks(
        tracks);

    queue->setCurrentIndex(
        0);

    const Track &track =
        tracks.first();

    qDebug()
        << "Album playback started:"
        << albumTitle()
        << "| tracks:"
        << tracks.size();

    emit trackSelected(
        track);

    m_playbackController
        ->playTrack(
            track);

    emit statusChanged(
        QString(
            "Воспроизведение альбома: %1")
            .arg(
                albumTitle()));
}

AlbumModel *
AlbumController::albumModel() const
{
    return m_albumModel;
}

ArtistAlbumsModel *
AlbumController::otherAlbumsModel() const
{
    return m_otherAlbumsModel;
}

bool AlbumController::isLoading() const
{
    return m_loading;
}

QString AlbumController::albumId() const
{
    return m_albumId;
}

QString AlbumController::albumTitle() const
{
    if (
        m_albumModel == nullptr
    ) {
        return {};
    }

    return m_albumModel->title();
}

QString AlbumController::albumCoverUri() const
{
    if (
        m_albumModel == nullptr
    ) {
        return {};
    }

    const QString albumCover =
        m_albumModel->coverUri();

    if (
        !albumCover.isEmpty()
    ) {
        return albumCover;
    }

    const QList<Track> tracks =
        m_albumModel->tracks();

    for (
        const Track &track :
        tracks
    ) {
        if (
            !track.coverUri.isEmpty()
        ) {
            return track.coverUri;
        }
    }

    return {};
}

int AlbumController::albumTrackCount() const
{
    if (
        m_albumModel == nullptr
    ) {
        return 0;
    }

    return m_albumModel->count();
}