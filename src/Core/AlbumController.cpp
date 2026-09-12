#include "AlbumController.h"

#include "../Yandex/Catalog/AlbumService.h"
#include "../Yandex/Catalog/ArtistService.h"
#include "../Yandex/Personal/LikesService.h"

#include "../Playback/PlaybackController.h"

#include <QDebug>


AlbumController::AlbumController(
    AlbumService *albumService,
    ArtistService *artistService,
    PlaybackController *playbackController,
    LikesService *likesService,
    QObject *parent)
    : QObject(parent)
    , m_albumService(albumService)
    , m_artistService(artistService)
    , m_playbackController(playbackController)
    , m_likesService(likesService)
    , m_albumModel(new AlbumModel(this))
    , m_otherAlbumsModel(new ArtistAlbumsModel(this))
{
    Q_ASSERT(m_albumService);
    Q_ASSERT(m_artistService);
    Q_ASSERT(m_playbackController);

    connect(
        m_albumService,
        &AlbumService::albumReceived,
        this,
        [this](const AlbumDetails &album) {

            m_loading = false;
            emit loadingChanged();

            m_albumId =
                album.album.id;

            AlbumDetails albumWithLikes =
                album;

            if (m_likesService) {
                for (Track &track :
                     albumWithLikes.tracks) {

                    track.liked =
                        m_likesService->isLiked(
                            track.id);
                }
            }

            m_albumModel->setAlbum(
                albumWithLikes);

            emit albumChanged();

            loadOtherAlbumsForCurrentArtist(
                albumWithLikes);
        });

    connect(
        m_albumService,
        &AlbumService::errorOccurred,
        this,
        [this](const QString &message) {

            m_loading = false;
            emit loadingChanged();

            emit statusChanged(message);
        });

    /*
     * ArtistService загружает альбомы исполнителя,
     * но сам по себе не знает о модели AlbumController.
     *
     * Передаём полученные альбомы в модель
     * "Другие альбомы".
     */
    connect(
        m_artistService,
        &ArtistService::artistAlbumsReceived,
        this,
        [this](const QList<Album> &albums) {

            m_otherAlbumsModel->setAlbums(
                albums);
        });

    if (m_likesService) {

        connect(
            m_likesService,
            &LikesService::likeChanged,
            this,
            [this](
                const QString &trackId,
                bool liked) {

                m_albumModel->setTrackLiked(
                    trackId,
                    liked);
            });

        connect(
            m_likesService,
            &LikesService::tracksReceived,
            this,
            [this](const QList<Track> &tracks) {

                for (const Track &track : tracks) {

                    m_albumModel->setTrackLiked(
                        track.id,
                        true);
                }
            });
    }
}


void AlbumController::loadAlbum(
    const QString &id)
{
    if (id.isEmpty()) {
        return;
    }

    m_loading = true;

    emit loadingChanged();

    m_albumId = id;

    m_currentArtistId.clear();

    m_albumModel->clear();

    m_otherAlbumsModel->clear();

    emit albumChanged();

    m_albumService->loadAlbum(id);
}


void AlbumController::selectAlbumTrack(
    int index)
{
    if (!m_playbackController) {
        return;
    }

    const QList<Track> tracks =
        m_albumModel->tracks();

    if (index < 0 ||
        index >= tracks.size()) {
        return;
    }

    const Track track =
        tracks.at(index);

    emit trackSelected(track);

    m_playbackController->playTrack(
        track);
}


void AlbumController::playAlbum()
{
    if (!m_playbackController) {
        return;
    }

    const QList<Track> tracks =
        m_albumModel->tracks();

    if (tracks.isEmpty()) {
        return;
    }

    QueueService *queue =
        m_playbackController->queueService();

    if (!queue) {
        return;
    }

    queue->clear();

    queue->setSource(
        m_albumModel->title(),
        QStringLiteral("album"));

    queue->addTracks(tracks);

    queue->setCurrentIndex(0);

    m_playbackController->playCurrent();
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
    return m_albumModel
        ? m_albumModel->title()
        : QString();
}


QString AlbumController::albumCoverUri() const
{
    return m_albumModel
        ? m_albumModel->coverUri()
        : QString();
}


int AlbumController::albumTrackCount() const
{
    return m_albumModel
        ? m_albumModel->count()
        : 0;
}


void AlbumController::loadOtherAlbumsForCurrentArtist(
    const AlbumDetails &album)
{
    if (!m_artistService) {
        return;
    }

    QString artistId;

    if (!album.tracks.isEmpty()) {

        const Track &firstTrack =
            album.tracks.first();

        if (!firstTrack.artists.isEmpty()) {
            artistId =
                firstTrack.artists.first().id;
        }
    }

    if (artistId.isEmpty()) {
        m_otherAlbumsModel->clear();
        return;
    }

    m_currentArtistId =
        artistId;

    m_otherAlbumsModel->clear();

    m_artistService->loadArtistAlbums(
        artistId);
}