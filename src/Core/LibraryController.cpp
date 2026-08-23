#include "LibraryController.h"

#include "../Queue/QueueService.h"

#include <QDebug>

LibraryController::LibraryController(
    PlaylistService *playlistService,
    AlbumService *albumService,
    ArtistService *artistService,
    PlaybackController *playbackController,
    QObject *parent)
    : QObject(parent)
    , m_playlistService(playlistService)
    , m_albumService(albumService)
    , m_artistService(artistService)
    , m_playbackController(playbackController)
    , m_playlistModel(
          new PlaylistModel(this))
    , m_albumModel(
          new AlbumModel(this))
    , m_artistModel(
          new ArtistModel(this))
{
    /*
     * Playlist
     */

    if (
        m_playlistService != nullptr
    ) {

        connect(
            m_playlistService,
            &PlaylistService::playlistReceived,
            this,
            [this](
                const Playlist &playlist) {

                m_loadingPlaylist =
                    false;

                emit loadingPlaylistChanged();

                m_playlistModel
                    ->setPlaylist(
                        playlist);

                m_currentPlaylistTitle =
                    playlist.title;

                m_currentPlaylistTrackCount =
                    playlist.trackCount;

                emit currentPlaylistChanged();

                qDebug()
                    << "Плейлист загружен:"
                    << playlist.title
                    << "| треков:"
                    << playlist.trackCount;

                emit statusChanged(
                    QString(
                        "Загружен плейлист: %1")
                        .arg(
                            playlist.title));
            });

        connect(
            m_playlistService,
            &PlaylistService::errorOccurred,
            this,
            [this](
                const QString &message) {

                m_loadingPlaylist =
                    false;

                emit loadingPlaylistChanged();

                qDebug()
                    << "Playlist error:"
                    << message;

                emit statusChanged(
                    QString(
                        "Ошибка загрузки плейлиста: %1")
                        .arg(
                            message));
            });
    }

    /*
     * Album
     */

    if (
        m_albumService != nullptr
    ) {

        connect(
            m_albumService,
            &AlbumService::albumReceived,
            this,
            [this](
                const AlbumDetails &album) {

                m_loadingAlbum =
                    false;

                emit loadingAlbumChanged();

                m_albumModel
                    ->setAlbum(
                        album);

                m_currentAlbumTitle =
                    album.album.title;

                m_currentAlbumTrackCount =
                    album.trackCount;

                m_currentAlbumCoverUri =
                    album.album.coverUri;

                emit currentAlbumChanged();

                qDebug()
                    << "Альбом загружен:"
                    << album.album.title
                    << "| id:"
                    << album.album.id
                    << "| треков:"
                    << album.tracks.size();

                emit statusChanged(
                    QString(
                        "Загружен альбом: %1")
                        .arg(
                            album.album.title));
            });

        connect(
            m_albumService,
            &AlbumService::errorOccurred,
            this,
            [this](
                const QString &message) {

                m_loadingAlbum =
                    false;

                emit loadingAlbumChanged();

                qDebug()
                    << "Album error:"
                    << message;

                emit statusChanged(
                    QString(
                        "Ошибка загрузки альбома: %1")
                        .arg(
                            message));
            });
    }

    /*
     * Artist
     */

    if (
        m_artistService != nullptr
    ) {

        connect(
            m_artistService,
            &ArtistService::artistReceived,
            this,
            [this](
                const ArtistDetails &artist) {

                m_loadingArtist =
                    false;

                emit loadingArtistChanged();

                m_artistModel
                    ->setArtist(
                        artist);

                m_currentArtistName =
                    artist.name;

                m_currentArtistCoverUri =
                    artist.coverUri;

                m_currentArtistGenres =
                    artist.genres.join(
                        ", ");

                m_currentArtistTrackCount =
                    artist.tracks.size();

                emit currentArtistChanged();

                qDebug()
                    << "Исполнитель загружен:"
                    << artist.name
                    << "| id:"
                    << artist.id
                    << "| треков:"
                    << artist.tracks.size();

                emit statusChanged(
                    QString(
                        "Загружен исполнитель: %1")
                        .arg(
                            artist.name));
            });

        connect(
            m_artistService,
            &ArtistService::errorOccurred,
            this,
            [this](
                const QString &message) {

                m_loadingArtist =
                    false;

                emit loadingArtistChanged();

                qDebug()
                    << "Artist error:"
                    << message;

                emit statusChanged(
                    QString(
                        "Ошибка загрузки исполнителя: %1")
                        .arg(
                            message));
            });
    }
}

void LibraryController::loadPlaylist(
    const QString &uid,
    int kind)
{
    loadPlaylistInternal(
        uid,
        kind,
        QString());
}

void LibraryController::loadPlaylistInternal(
    const QString &uid,
    int kind,
    const QString &statusTitle)
{
    const QString playlistUid =
        uid.trimmed();

    if (
        playlistUid.isEmpty() ||
        kind <= 0
    ) {

        emit statusChanged(
            "Некорректный плейлист");

        return;
    }

    m_loadingPlaylist =
        true;

    emit loadingPlaylistChanged();

    m_playlistModel
        ->clear();

    m_currentPlaylistTitle.clear();

    m_currentPlaylistTrackCount =
        0;

    emit currentPlaylistChanged();

    qDebug()
        << "Загрузка плейлиста:"
        << statusTitle
        << "| uid:"
        << playlistUid
        << "| kind:"
        << kind;

    if (
        !statusTitle.isEmpty()
    ) {

        emit statusChanged(
            QString(
                "Загрузка плейлиста: %1")
                .arg(
                    statusTitle));
    }

    if (
        m_playlistService != nullptr
    ) {

        m_playlistService
            ->loadPlaylist(
                playlistUid,
                kind);
    }
}

void LibraryController::loadAlbum(
    const QString &id)
{
    loadAlbumInternal(
        id,
        QString());
}

void LibraryController::loadAlbumInternal(
    const QString &id,
    const QString &statusTitle)
{
    const QString albumId =
        id.trimmed();

    if (
        albumId.isEmpty()
    ) {

        emit statusChanged(
            "Некорректный альбом");

        return;
    }

    m_loadingAlbum =
        true;

    emit loadingAlbumChanged();

    m_albumModel
        ->clear();

    m_currentAlbumTitle.clear();

    m_currentAlbumTrackCount =
        0;

    m_currentAlbumCoverUri.clear();

    emit currentAlbumChanged();

    qDebug()
        << "Загрузка альбома:"
        << statusTitle
        << "| id:"
        << albumId;

    if (
        !statusTitle.isEmpty()
    ) {

        emit statusChanged(
            QString(
                "Загрузка альбома: %1")
                .arg(
                    statusTitle));
    }

    if (
        m_albumService != nullptr
    ) {

        m_albumService
            ->loadAlbum(
                albumId);
    }
}

void LibraryController::loadArtist(
    const QString &id)
{
    const QString artistId =
        id.trimmed();

    if (
        artistId.isEmpty()
    ) {

        emit statusChanged(
            "ID исполнителя не указан");

        return;
    }

    m_loadingArtist =
        true;

    emit loadingArtistChanged();

    m_artistModel
        ->clear();

    m_currentArtistName.clear();

    m_currentArtistCoverUri.clear();

    m_currentArtistGenres.clear();

    m_currentArtistTrackCount =
        0;

    emit currentArtistChanged();

    qDebug()
        << "Загрузка исполнителя:"
        << artistId;

    emit statusChanged(
        QString(
            "Загрузка исполнителя: %1")
            .arg(
                artistId));

    if (
        m_artistService != nullptr
    ) {

        m_artistService
            ->loadArtist(
                artistId);
    }
}

void LibraryController::selectPlaylistTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    const Track track =
        m_playlistModel
            ->trackAt(
                index);

    if (
        track.id.isEmpty()
    ) {

        emit statusChanged(
            "Некорректный трек плейлиста");

        return;
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue != nullptr
    ) {

        queue->clear();

        queue->addTracks(
            m_playlistModel
                ->tracks());

        queue->setCurrentIndex(
            index);
    }

    m_playbackController
        ->playTrack(
            track);
}

void LibraryController::selectAlbumTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    const Track track =
        m_albumModel
            ->trackAt(
                index);

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
        queue != nullptr
    ) {

        queue->clear();

        queue->addTracks(
            m_albumModel
                ->tracks());

        queue->setCurrentIndex(
            index);
    }

    m_playbackController
        ->playTrack(
            track);
}

void LibraryController::selectArtistTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    const Track track =
        m_artistModel
            ->trackAt(
                index);

    if (
        track.id.isEmpty()
    ) {

        emit statusChanged(
            "Некорректный трек исполнителя");

        return;
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue != nullptr
    ) {

        queue->clear();

        queue->addTracks(
            m_artistModel
                ->tracks());

        queue->setCurrentIndex(
            index);
    }

    m_playbackController
        ->playTrack(
            track);
}

PlaylistModel *
LibraryController::playlistModel() const
{
    return m_playlistModel;
}

AlbumModel *
LibraryController::albumModel() const
{
    return m_albumModel;
}

ArtistModel *
LibraryController::artistModel() const
{
    return m_artistModel;
}

bool LibraryController::isLoadingPlaylist() const
{
    return m_loadingPlaylist;
}

bool LibraryController::isLoadingAlbum() const
{
    return m_loadingAlbum;
}

bool LibraryController::isLoadingArtist() const
{
    return m_loadingArtist;
}

QString LibraryController::currentPlaylistTitle() const
{
    return m_currentPlaylistTitle;
}

int LibraryController::currentPlaylistTrackCount() const
{
    return m_currentPlaylistTrackCount;
}

QString LibraryController::currentAlbumTitle() const
{
    return m_currentAlbumTitle;
}

int LibraryController::currentAlbumTrackCount() const
{
    return m_currentAlbumTrackCount;
}

QString LibraryController::currentAlbumCoverUri() const
{
    return m_currentAlbumCoverUri;
}

QString LibraryController::currentArtistName() const
{
    return m_currentArtistName;
}

QString LibraryController::currentArtistCoverUri() const
{
    return m_currentArtistCoverUri;
}

QString LibraryController::currentArtistGenres() const
{
    return m_currentArtistGenres;
}

int LibraryController::currentArtistTrackCount() const
{
    return m_currentArtistTrackCount;
}