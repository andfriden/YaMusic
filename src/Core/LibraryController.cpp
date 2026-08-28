#include "LibraryController.h"

#include "../Queue/QueueService.h"

#include <QDebug>


// =============================================================
// Constructor
// =============================================================

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

    , m_libraryPlaylistsModel(
          new LibraryPlaylistsModel(this))

    , m_playlistModel(
          new PlaylistModel(this))

    , m_artistModel(
          new ArtistModel(this))
{
    // =============================================================
    // Playlist service
    // =============================================================

    if (
        m_playlistService != nullptr
    )
    {
        // ---------------------------------------------------------
        // Single playlist
        // ---------------------------------------------------------

        connect(
            m_playlistService,
            &PlaylistService::playlistReceived,
            this,
            [this](
                const Playlist &playlist)
            {
                m_loadingPlaylist =
                    false;

                emit loadingPlaylistChanged();


                m_playlistModel
                    ->setPlaylist(
                        playlist);


                m_currentPlaylistTitle =
                    playlist.title;


                m_currentPlaylistCoverUri =
                    playlist.coverUri;


                m_currentPlaylistTrackCount =
                    playlist.trackCount;


                emit currentPlaylistChanged();


                qDebug()
                    << "Плейлист загружен:"
                    << playlist.title
                    << "| треков:"
                    << playlist.trackCount;
            });


        // ---------------------------------------------------------
        // User playlists
        // ---------------------------------------------------------

        connect(
            m_playlistService,
            &PlaylistService::userPlaylistsReceived,
            this,
            [this](
                const QList<PersonalPlaylist> &playlists)
            {
                m_loadingLibraryPlaylists =
                    false;

                emit loadingLibraryPlaylistsChanged();


                m_libraryPlaylistsModel
                    ->setPlaylists(
                        playlists);


                qDebug()
                    << "Плейлистов в медиатеке:"
                    << playlists.size();
            });


        // ---------------------------------------------------------
        // Service error
        // ---------------------------------------------------------

        connect(
            m_playlistService,
            &PlaylistService::errorOccurred,
            this,
            [this](
                const QString &message)
            {
                m_loadingPlaylist =
                    false;

                m_loadingLibraryPlaylists =
                    false;


                emit loadingPlaylistChanged();

                emit loadingLibraryPlaylistsChanged();


                qDebug()
                    << "Playlist error:"
                    << message;


                emit statusChanged(
                    QString(
                        "Ошибка плейлистов: %1")
                    .arg(
                        message));
            });
    }


    // =============================================================
    // Artist service
    // =============================================================

    if (
        m_artistService != nullptr
    )
    {
        connect(
            m_artistService,
            &ArtistService::artistReceived,
            this,
            [this](
                const ArtistDetails &artist)
            {
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
            });


        connect(
            m_artistService,
            &ArtistService::errorOccurred,
            this,
            [this](
                const QString &message)
            {
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


// =============================================================
// Library playlists
// =============================================================

void LibraryController::loadUserPlaylists(
    const QString &uid)
{
    const QString userUid =
        uid.trimmed();


    if (
        userUid.isEmpty()
    )
    {
        emit statusChanged(
            "UID пользователя не указан");

        return;
    }


    if (
        m_playlistService == nullptr
    )
    {
        emit statusChanged(
            "Сервис плейлистов недоступен");

        return;
    }


    m_loadingLibraryPlaylists =
        true;

    emit loadingLibraryPlaylistsChanged();


    m_libraryPlaylistsModel
        ->clear();


    m_playlistService
        ->loadUserPlaylists(
            userUid);
}


void LibraryController::selectLibraryPlaylist(
    int index)
{
    if (
        m_libraryPlaylistsModel == nullptr
    )
    {
        emit statusChanged(
            "Модель плейлистов недоступна");

        return;
    }


    const PersonalPlaylist playlist =
        m_libraryPlaylistsModel
            ->playlistAt(
                index);


    if (
        playlist.uid.isEmpty() ||
        playlist.kind <= 0
    )
    {
        emit statusChanged(
            "Некорректный плейлист");

        return;
    }


    loadPlaylist(
        playlist.uid,
        playlist.kind);
}


LibraryPlaylistsModel *
LibraryController::libraryPlaylistsModel() const
{
    return m_libraryPlaylistsModel;
}


bool
LibraryController::isLoadingLibraryPlaylists() const
{
    return m_loadingLibraryPlaylists;
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
    )
    {
        emit statusChanged(
            "Некорректный плейлист");

        return;
    }


    if (
        m_playlistService == nullptr
    )
    {
        emit statusChanged(
            "Сервис плейлистов недоступен");

        return;
    }


    m_loadingPlaylist =
        true;

    emit loadingPlaylistChanged();


    m_playlistModel
        ->clear();


    m_currentPlaylistTitle.clear();

    m_currentPlaylistCoverUri.clear();

    m_currentPlaylistTrackCount =
        0;


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


void LibraryController::selectPlaylistTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    )
    {
        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }


    const QList<Track> tracks =
        m_playlistModel
            ->tracks();


    if (
        index < 0 ||
        index >= tracks.size()
    )
    {
        emit statusChanged(
            "Некорректный индекс трека");

        return;
    }


    const Track track =
        tracks.at(
            index);


    if (
        track.id.isEmpty()
    )
    {
        emit statusChanged(
            "Некорректный трек плейлиста");

        return;
    }


    QueueService *queue =
        m_playbackController
            ->queueService();


    if (
        queue == nullptr
    )
    {
        emit statusChanged(
            "Очередь воспроизведения недоступна");

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


PlaylistModel *
LibraryController::playlistModel() const
{
    return m_playlistModel;
}


bool
LibraryController::isLoadingPlaylist() const
{
    return m_loadingPlaylist;
}


QString
LibraryController::currentPlaylistTitle() const
{
    return m_currentPlaylistTitle;
}


QString
LibraryController::currentPlaylistCoverUri() const
{
    return m_currentPlaylistCoverUri;
}


int
LibraryController::currentPlaylistTrackCount() const
{
    return m_currentPlaylistTrackCount;
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
    )
    {
        emit statusChanged(
            "ID исполнителя не указан");

        return;
    }


    if (
        m_artistService == nullptr
    )
    {
        emit statusChanged(
            "Сервис исполнителя недоступен");

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


    m_artistService
        ->loadArtist(
            artistId);
}


void LibraryController::selectArtistTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    )
    {
        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }


    const QList<Track> tracks =
        m_artistModel
            ->tracks();


    if (
        index < 0 ||
        index >= tracks.size()
    )
    {
        emit statusChanged(
            "Некорректный индекс трека исполнителя");

        return;
    }


    const Track track =
        tracks.at(
            index);


    if (
        track.id.isEmpty()
    )
    {
        emit statusChanged(
            "Некорректный трек исполнителя");

        return;
    }


    QueueService *queue =
        m_playbackController
            ->queueService();


    if (
        queue == nullptr
    )
    {
        emit statusChanged(
            "Очередь воспроизведения недоступна");

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


ArtistModel *
LibraryController::artistModel() const
{
    return m_artistModel;
}


bool
LibraryController::isLoadingArtist() const
{
    return m_loadingArtist;
}


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