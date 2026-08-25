#include "LibraryController.h"

#include "../Queue/QueueService.h"

#include <QDebug>

LibraryController::LibraryController(
    PlaylistService *playlistService,
    ArtistService *artistService,
    PlaybackController *playbackController,
    QObject *parent)
    : QObject(parent)
    , m_playlistService(
          playlistService)
    , m_artistService(
          artistService)
    , m_playbackController(
          playbackController)
    , m_playlistModel(
          new PlaylistModel(this))
    , m_artistModel(
          new ArtistModel(this))
{
    // =============================================================
    // Playlist
    // =============================================================

    if (
        m_playlistService != nullptr
    ) {

        connect(
            m_playlistService,
            &PlaylistService::playlistReceived,
            this,
            [this](
                const Playlist &playlist) {

                m_loadingPlaylist = false;

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

                m_loadingPlaylist = false;

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

    // =============================================================
    // Artist
    // =============================================================

    if (
        m_artistService != nullptr
    ) {

        connect(
            m_artistService,
            &ArtistService::artistReceived,
            this,
            [this](
                const ArtistDetails &artist) {

                m_loadingArtist = false;

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

                m_loadingArtist = false;

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

// =============================================================
// Playlist
// =============================================================

void LibraryController::loadPlaylist(
    const QString &uid,
    int kind)
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

    if (
        m_playlistService == nullptr
    ) {

        emit statusChanged(
            "Сервис плейлистов недоступен");

        return;
    }

    m_loadingPlaylist = true;

    emit loadingPlaylistChanged();

    m_playlistModel
        ->clear();

    m_currentPlaylistTitle.clear();

    m_currentPlaylistTrackCount = 0;

    emit currentPlaylistChanged();

    qDebug()
        << "Загрузка плейлиста:"
        << "| uid:"
        << playlistUid
        << "| kind:"
        << kind;

    m_playlistService
        ->loadPlaylist(
            playlistUid,
            kind);
}

// =============================================================
// Artist
// =============================================================

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

    if (
        m_artistService == nullptr
    ) {

        emit statusChanged(
            "Сервис исполнителя недоступен");

        return;
    }

    m_loadingArtist = true;

    emit loadingArtistChanged();

    m_artistModel
        ->clear();

    m_currentArtistName.clear();

    m_currentArtistCoverUri.clear();

    m_currentArtistGenres.clear();

    m_currentArtistTrackCount = 0;

    emit currentArtistChanged();

    qDebug()
        << "Загрузка исполнителя:"
        << artistId;

    emit statusChanged(
        QString(
            "Загрузка исполнителя: %1")
        .arg(
            artistId));

    m_artistService
        ->loadArtist(
            artistId);
}

// =============================================================
// Playlist playback
// =============================================================

void LibraryController::selectPlaylistTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        emit statusChanged(
            "PlaybackController недоступен");

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
        queue == nullptr
    ) {

        emit statusChanged(
            "Очередь воспроизведения недоступна");

        return;
    }

    const QList<Track> tracks =
        m_playlistModel
            ->tracks();

    if (
        index < 0 ||
        index >= tracks.size()
    ) {

        emit statusChanged(
            "Некорректный индекс трека");

        return;
    }

    queue->clear();

    queue->addTracks(
        tracks);

    queue->setCurrentIndex(
        index);

    m_playbackController
        ->playTrack(
            track);
}

// =============================================================
// Artist playback
// =============================================================

void LibraryController::selectArtistTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        emit statusChanged(
            "PlaybackController недоступен");

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
        queue == nullptr
    ) {

        emit statusChanged(
            "Очередь воспроизведения недоступна");

        return;
    }

    const QList<Track> tracks =
        m_artistModel
            ->tracks();

    if (
        index < 0 ||
        index >= tracks.size()
    ) {

        emit statusChanged(
            "Некорректный индекс трека");

        return;
    }

    queue->clear();

    queue->addTracks(
        tracks);

    queue->setCurrentIndex(
        index);

    m_playbackController
        ->playTrack(
            track);
}

// =============================================================
// Models
// =============================================================

PlaylistModel *
LibraryController::playlistModel() const
{
    return m_playlistModel;
}

ArtistModel *
LibraryController::artistModel() const
{
    return m_artistModel;
}

// =============================================================
// Loading state
// =============================================================

bool LibraryController::isLoadingPlaylist() const
{
    return m_loadingPlaylist;
}

bool LibraryController::isLoadingArtist() const
{
    return m_loadingArtist;
}

// =============================================================
// Playlist state
// =============================================================

QString
LibraryController::currentPlaylistTitle() const
{
    return m_currentPlaylistTitle;
}

int
LibraryController::currentPlaylistTrackCount() const
{
    return m_currentPlaylistTrackCount;
}

// =============================================================
// Artist state
// =============================================================

QString
LibraryController::currentArtistName() const
{
    return m_currentArtistName;
}

QString
LibraryController::currentArtistCoverUri() const
{
    return m_currentArtistCoverUri;
}

QString
LibraryController::currentArtistGenres() const
{
    return m_currentArtistGenres;
}

int
LibraryController::currentArtistTrackCount() const
{
    return m_currentArtistTrackCount;
}