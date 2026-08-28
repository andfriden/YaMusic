#include "AppController.h"

#include "../Models/PersonalPlaylist.h"

#include "../Player/PlayerService.h"
#include "../Queue/QueueService.h"

#include "../Yandex/Account/AccountService.h"
#include "../Yandex/Auth/YandexAuth.h"

#include "../Yandex/Catalog/AlbumService.h"
#include "../Yandex/Catalog/ArtistService.h"
#include "../Yandex/Catalog/SearchService.h"
#include "../Yandex/Catalog/TrackService.h"

#include "../Yandex/Personal/PersonalLanding.h"
#include "../Yandex/Personal/RecentListeningService.h"
#include "../Yandex/Personal/PlaylistService.h"
#include "../Yandex/Personal/YandexPersonal.h"

#include <QDebug>

AppController::AppController(
    QObject *parent)
    : QObject(parent)
    , m_auth(
          new YandexAuth(this))
    , m_accountService(
          new AccountService(
              m_auth,
              this))
    , m_searchService(
          new SearchService(
              m_auth,
              this))
    , m_trackService(
          new TrackService(
              m_auth,
              this))
    , m_yandexPersonal(
          new YandexPersonal(
              m_auth,
              this))
    , m_personalLanding(
          new PersonalLanding(
              m_auth,
              this))
    , m_recentListeningService(
          new RecentListeningService(
              m_auth,
              this))
    , m_playlistService(
          new PlaylistService(
              m_auth,
              this))
    , m_albumService(
          new AlbumService(
              m_auth,
              this))
    , m_artistService(
          new ArtistService(
              m_auth,
              this))
    , m_playerService(
          new PlayerService(
              this))
    , m_queueService(
          new QueueService(
              this))
    , m_playbackController(
          new PlaybackController(
              m_trackService,
              m_playerService,
              m_queueService,
              this))
    , m_libraryController(
          new LibraryController(
              m_playlistService,
              m_artistService,
              m_playbackController,
              this))
    , m_personalController(
          new PersonalController(
              m_yandexPersonal,
              m_personalLanding,
              m_recentListeningService,
              m_playbackController,
              m_playerService,
              this))
    , m_searchController(
          new SearchController(
              m_searchService,
              m_playbackController,
              this))
    , m_albumController(
          new AlbumController(
              m_albumService,
              m_artistService,
              m_playbackController,
              this))
    , m_artistController(
          new ArtistController(
              m_artistService,
              m_playbackController,
              this))
{
    // =============================================================
    // Account
    // =============================================================

    connect(
        m_accountService,
        &AccountService::accountReceived,
        this,
        [this](
            const Account &account) {

            if (
                m_recentListeningService !=
                nullptr
            ) {
                m_recentListeningService
                    ->setUserId(
                        QString::number(
                            account.uid));

                m_recentListeningService
                    ->load(
                        50,
                        10);
            }

            emit statusChanged(
                QString(
                    "Выполнен вход: %1 (uid: %2)")
                    .arg(
                        account.displayName)
                    .arg(
                        account.uid));
        });

    connect(
        m_accountService,
        &AccountService::errorOccurred,
        this,
        &AppController::statusChanged);

    m_accountService
        ->loadAccount();

    // =============================================================
    // Search
    // =============================================================

    connect(
        m_searchController,
        &SearchController::statusChanged,
        this,
        &AppController::statusChanged);

    connect(
        m_searchController,
        &SearchController::searchingChanged,
        this,
        &AppController::searchingChanged);

    // =============================================================
    // Library
    // =============================================================

    connect(
        m_libraryController,
        &LibraryController::statusChanged,
        this,
        &AppController::statusChanged);

    connect(
        m_libraryController,
        &LibraryController::loadingPlaylistChanged,
        this,
        &AppController::loadingPlaylistChanged);

    connect(
        m_libraryController,
        &LibraryController::loadingArtistChanged,
        this,
        &AppController::loadingArtistChanged);

    connect(
        m_libraryController,
        &LibraryController::currentPlaylistChanged,
        this,
        &AppController::currentPlaylistChanged);

    connect(
        m_libraryController,
        &LibraryController::currentArtistChanged,
        this,
        &AppController::currentArtistChanged);

    // =============================================================
    // Album
    // =============================================================

    connect(
        m_albumController,
        &AlbumController::statusChanged,
        this,
        &AppController::statusChanged);

    connect(
        m_albumController,
        &AlbumController::loadingChanged,
        this,
        [this]() {
            emit loadingAlbumChanged();
        });

    connect(
        m_albumController,
        &AlbumController::albumChanged,
        this,
        [this]() {

            emit currentAlbumChanged();

            if (
                !m_playAlbumAfterLoad
            ) {
                return;
            }

            m_playAlbumAfterLoad = false;

            if (
                m_albumController == nullptr
            ) {
                return;
            }

            if (
                m_albumController
                    ->albumModel()
                    ->count() <= 0
            ) {
                emit statusChanged(
                    "В альбоме нет треков");

                return;
            }

            m_albumController
                ->playAlbum();
        });

    // =============================================================
    // Personal
    // =============================================================

    connect(
        m_personalController,
        &PersonalController::statusChanged,
        this,
        &AppController::statusChanged);

    connect(
        m_personalController,
        &PersonalController::loadingMyWaveChanged,
        this,
        &AppController::loadingMyWaveChanged);

    connect(
        m_personalController,
        &PersonalController::loadingMoreMyWaveChanged,
        this,
        &AppController::loadingMoreMyWaveChanged);

    connect(
        m_personalController,
        &PersonalController::loadingRecommendationsChanged,
        this,
        &AppController::loadingRecommendationsChanged);

    connect(
        m_personalController,
        &PersonalController::recommendationsLoaded,
        this,
        &AppController::recommendationsLoaded);

    connect(
        m_personalController,
        &PersonalController::personalPlaylistSelected,
        this,
        [this](
            const PersonalPlaylist &playlist) {

            m_libraryController
                ->loadPlaylist(
                    playlist.uid,
                    playlist.kind);
        });

    // =============================================================
    // Artist
    // =============================================================

    connect(
        m_artistController,
        &ArtistController::statusChanged,
        this,
        &AppController::statusChanged);

    connect(
        m_artistController,
        &ArtistController::artistChanged,
        this,
        &AppController::currentArtistChanged);

    connect(
        m_artistController,
        &ArtistController::similarArtistSelected,
        this,
        [this](
            const QString &artistId) {

            /*
             * Go through AppController::loadArtist()
             * so MainLayout receives the navigation request.
             */

            loadArtist(
                artistId);
        });

    // =============================================================
    // Playback
    // =============================================================

    connect(
        m_playbackController,
        &PlaybackController::currentTrackChanged,
        this,
        [this]() {

            emit currentTrackChanged();

            const Track track =
                m_playbackController
                    ->currentTrack();

            if (
                track.id.isEmpty()
            ) {
                return;
            }

            QString artistName;

            if (
                !track.artists.isEmpty()
            ) {
                artistName =
                    track.artists
                        .first()
                        .name;
            }

            const QString message =
                artistName.isEmpty()
                    ? QString(
                          "Выбран трек: %1")
                          .arg(
                              track.title)
                    : QString(
                          "Выбран трек: %1 — %2")
                          .arg(
                              track.title)
                          .arg(
                              artistName);

            qDebug()
                << message;

            emit statusChanged(
                message);
        });

    connect(
        m_playbackController,
        &PlaybackController::stateChanged,
        this,
        &AppController::playbackStateChanged);

    connect(
        m_playbackController,
        &PlaybackController::repeatModeChanged,
        this,
        &AppController::repeatModeChanged);

    connect(
        m_playbackController,
        &PlaybackController::shuffleChanged,
        this,
        &AppController::shuffleChanged);

    connect(
        m_playbackController,
        &PlaybackController::playbackError,
        this,
        &AppController::statusChanged);

    // =============================================================
    // Player
    // =============================================================

    connect(
        m_playerService,
        &PlayerService::playingChanged,
        this,
        [this]() {

            emit playingChanged();

            if (
                m_playerService
                    ->isPlaying()
            ) {
                emit statusChanged(
                    "Воспроизведение");
            }
        });

    connect(
        m_playerService,
        &PlayerService::positionChanged,
        this,
        [this](qint64) {
            emit positionChanged();
        });

    connect(
        m_playerService,
        &PlayerService::durationChanged,
        this,
        [this](qint64) {
            emit durationChanged();
        });

    connect(
        m_playerService,
        &PlayerService::playbackPaused,
        this,
        [this]() {
            emit statusChanged(
                "Пауза");
        });

    connect(
        m_playerService,
        &PlayerService::playbackStopped,
        this,
        [this]() {
            emit statusChanged(
                "Остановлено");
        });

    connect(
        m_playerService,
        &PlayerService::errorOccurred,
        this,
        [this](
            const QString &message) {

            emit statusChanged(
                QString(
                    "Ошибка воспроизведения: %1")
                    .arg(
                        message));
        });
}

// =============================================================
// Tests
// =============================================================

void AppController::testConnection()
{
    emit statusChanged(
        "Приложение работает");
}

void AppController::testYandexApi()
{
    m_accountService
        ->loadAccount();

    emit statusChanged(
        "Проверка аккаунта Яндекс Музыки...");
}

void AppController::testSearch(
    const QString &query)
{
    m_searchController
        ->search(
            query);
}

// =============================================================
// Artist
// =============================================================

void AppController::loadArtist(
    const QString &id)
{
    if (
        m_artistController == nullptr
    ) {
        return;
    }

    const QString artistId =
        id.trimmed();

    if (
        artistId.isEmpty()
    ) {
        emit statusChanged(
            "Некорректный исполнитель");

        return;
    }

    /*
     * First tell the navigation layer to open
     * ArtistPage, then load the artist data.
     */

    emit artistPageRequested(
        artistId);

    m_artistController
        ->loadArtist(
            artistId);
}

// =============================================================
// Album
// =============================================================

void AppController::loadAlbum(
    const QString &id)
{
    if (
        m_albumController == nullptr
    ) {
        return;
    }

    const QString albumId =
        id.trimmed();

    if (
        albumId.isEmpty()
    ) {
        emit statusChanged(
            "Некорректный альбом");

        return;
    }

    m_playAlbumAfterLoad =
        false;

    emit albumPageRequested(
        albumId);

    m_albumController
        ->loadAlbum(
            albumId);
}

void AppController::playAlbum(
    const QString &id)
{
    if (
        m_albumController == nullptr
    ) {
        return;
    }

    const QString albumId =
        id.trimmed();

    if (
        albumId.isEmpty()
    ) {
        emit statusChanged(
            "Некорректный альбом");

        return;
    }

    m_playAlbumAfterLoad =
        true;

    emit albumPageRequested(
        albumId);

    m_albumController
        ->loadAlbum(
            albumId);
}

// =============================================================
// Personal
// =============================================================

void AppController::loadMyWave()
{
    m_personalController
        ->loadMyWave();
}

void AppController::loadMoreMyWave()
{
    m_personalController
        ->loadMoreMyWave();
}

void AppController::loadRecommendations()
{
    m_personalController
        ->loadRecommendations();
}

// =============================================================
// Selection
// =============================================================

void AppController::selectSearchResult(
    int index)
{
    m_searchController
        ->selectResult(
            index);
}

void AppController::selectMyWaveTrack(
    int index)
{
    m_personalController
        ->selectMyWaveTrack(
            index);
}

void AppController::selectPersonalPlaylist(
    const QString &uid,
    int kind)
{
    if (
        m_personalController == nullptr
    ) {
        return;
    }


    emit playlistPageRequested();


    m_personalController
        ->selectPersonalPlaylist(
            uid,
            kind);
}

void AppController::selectPlaylistTrack(
    int index)
{
    m_libraryController
        ->selectPlaylistTrack(
            index);
}

void AppController::selectRecentListening(
    int index)
{
    m_personalController
        ->selectRecentListening(
            index);
}

void AppController::selectAlbumTrack(
    int index)
{
    if (
        m_albumController == nullptr
    ) {
        return;
    }

    m_albumController
        ->selectAlbumTrack(
            index);
}

void AppController::selectArtistTrack(
    int index)
{
    if (
        m_artistController == nullptr
    ) {
        return;
    }

    m_artistController
        ->selectTrack(
            index);
}

void AppController::selectSimilarArtist(
    int index)
{
    if (
        m_artistController == nullptr
    ) {
        return;
    }

    m_artistController
        ->selectSimilarArtist(
            index);
}

// =============================================================
// Playback
// =============================================================

void AppController::play()
{
    m_playerService
        ->play();
}

void AppController::pause()
{
    m_playerService
        ->pause();
}

void AppController::stop()
{
    m_playerService
        ->stop();
}

void AppController::next()
{
    if (
        !m_playbackController
            ->next()
    ) {
        emit statusChanged(
            "Следующего трека нет");
    }
}

void AppController::previous()
{
    if (
        !m_playbackController
            ->previous()
    ) {
        emit statusChanged(
            "Предыдущего трека нет");
    }
}

void AppController::cycleRepeat()
{
    m_playbackController
        ->cycleRepeatMode();
}

void AppController::setRepeatMode(
    int mode)
{
    if (
        mode < QueueService::RepeatOff ||
        mode > QueueService::RepeatOne
    ) {
        return;
    }

    m_playbackController
        ->setRepeatMode(
            static_cast<
                QueueService::RepeatMode>(
                mode));
}

void AppController::toggleShuffle()
{
    m_playbackController
        ->toggleShuffle();
}

void AppController::setShuffle(
    bool enabled)
{
    m_playbackController
        ->setShuffleEnabled(
            enabled);
}

void AppController::seek(
    qint64 position)
{
    m_playerService
        ->seek(
            position);
}

// =============================================================
// Models
// =============================================================

SearchModel *
AppController::searchModel() const
{
    return m_searchController
        ->model();
}

MyWaveModel *
AppController::myWaveModel() const
{
    return m_personalController
        ->myWaveModel();
}

PersonalPlaylistsModel *
AppController::personalPlaylistsModel() const
{
    return m_personalController
        ->personalPlaylistsModel();
}

PlaylistModel *
AppController::playlistModel() const
{
    return m_libraryController
        ->playlistModel();
}

RecentListeningModel *
AppController::recentListeningModel() const
{
    return m_personalController
        ->recentListeningModel();
}

AlbumController *
AppController::albumController() const
{
    return m_albumController;
}

ArtistController *
AppController::artistController() const
{
    return m_artistController;
}

// =============================================================
// State
// =============================================================

bool
AppController::isSearching() const
{
    return m_searchController
        ->isSearching();
}

bool
AppController::isPlaying() const
{
    return m_playerService
        ->isPlaying();
}

bool
AppController::isLoadingMyWave() const
{
    return m_personalController
        ->isLoadingMyWave();
}

bool
AppController::isLoadingMoreMyWave() const
{
    return m_personalController
        ->isLoadingMoreMyWave();
}

bool
AppController::isLoadingRecommendations() const
{
    return m_personalController
        ->isLoadingRecommendations();
}

bool
AppController::isLoadingPlaylist() const
{
    return m_libraryController
        ->isLoadingPlaylist();
}

bool
AppController::isLoadingAlbum() const
{
    if (
        m_albumController == nullptr
    ) {
        return false;
    }

    return m_albumController
        ->isLoading();
}

bool
AppController::isLoadingArtist() const
{
    if (
        m_artistController == nullptr
    ) {
        return false;
    }

    return m_artistController
        ->isLoading();
}

// =============================================================
// Current playlist
// =============================================================

QString
AppController::currentPlaylistTitle() const
{
    return m_libraryController
        ->currentPlaylistTitle();
}

int
AppController::currentPlaylistTrackCount() const
{
    return m_libraryController
        ->currentPlaylistTrackCount();
}

QString
AppController::currentPlaylistCoverUri() const
{
    return m_libraryController
        ->currentPlaylistCoverUri();
}

// =============================================================
// Current album
// =============================================================

QString
AppController::currentAlbumTitle() const
{
    if (
        m_albumController == nullptr
    ) {
        return {};
    }

    return m_albumController
        ->albumTitle();
}

int
AppController::currentAlbumTrackCount() const
{
    if (
        m_albumController == nullptr
    ) {
        return 0;
    }

    return m_albumController
        ->albumTrackCount();
}

QString
AppController::currentAlbumCoverUri() const
{
    if (
        m_albumController == nullptr
    ) {
        return {};
    }

    return m_albumController
        ->albumCoverUri();
}

// =============================================================
// Current artist
// =============================================================

QString
AppController::currentArtistName() const
{
    if (
        m_artistController == nullptr
    ) {
        return {};
    }

    return m_artistController
        ->artistName();
}

QString
AppController::currentArtistCoverUri() const
{
    if (
        m_artistController == nullptr
    ) {
        return {};
    }

    return m_artistController
        ->artistCoverUri();
}

QString
AppController::currentArtistGenres() const
{
    if (
        m_artistController == nullptr
    ) {
        return {};
    }

    return m_artistController
        ->artistGenres();
}

int
AppController::currentArtistTrackCount() const
{
    if (
        m_artistController == nullptr
    ) {
        return 0;
    }

    return m_artistController
        ->artistModel()
        ->count();
}

// =============================================================
// Current track
// =============================================================

QString
AppController::currentTrackTitle() const
{
    return m_playbackController
        ->currentTrack()
        .title;
}

QString
AppController::currentTrackArtist() const
{
    const Track track =
        m_playbackController
            ->currentTrack();

    if (
        track.artists.isEmpty()
    ) {
        return {};
    }

    return track.artists
        .first()
        .name;
}

QString
AppController::currentTrackArtistId() const
{
    const Track track =
        m_playbackController
            ->currentTrack();

    if (
        track.artists.isEmpty()
    ) {
        return {};
    }

    return track.artists
        .first()
        .id;
}

QString
AppController::currentTrackAlbumTitle() const
{
    const Track track =
        m_playbackController
            ->currentTrack();

    if (
        track.albums.isEmpty()
    ) {
        return {};
    }

    return track.albums
        .first()
        .title;
}

QString
AppController::currentTrackAlbumId() const
{
    const Track track =
        m_playbackController
            ->currentTrack();

    if (
        track.albums.isEmpty()
    ) {
        return {};
    }

    return track.albums
        .first()
        .id;
}

QString
AppController::currentTrackCoverUri() const
{
    return m_playbackController
        ->currentTrack()
        .coverUri;
}

// =============================================================
// Playback state
// =============================================================

qint64
AppController::position() const
{
    return m_playerService
        ->position();
}

qint64
AppController::duration() const
{
    return m_playerService
        ->duration();
}

PlaybackController::PlaybackState
AppController::playbackState() const
{
    return m_playbackController
        ->state();
}

int
AppController::repeatMode() const
{
    return static_cast<int>(
        m_playbackController
            ->repeatMode());
}

bool
AppController::shuffleEnabled() const
{
    return m_playbackController
        ->shuffleEnabled();
}