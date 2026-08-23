#include "AppController.h"

#include "../Player/PlayerService.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Account/AccountService.h"
#include "../Yandex/Auth/YandexAuth.h"
#include "../Yandex/Catalog/AlbumService.h"
#include "../Yandex/Catalog/ArtistService.h"
#include "../Yandex/Catalog/SearchService.h"
#include "../Yandex/Catalog/TrackService.h"
#include "../Yandex/Personal/PersonalLanding.h"
#include "../Yandex/Personal/PlaylistService.h"
#include "../Yandex/Personal/YandexPersonal.h"

#include <QDebug>

AppController::AppController(
    QObject *parent)
    : QObject(parent)
    , m_auth(
          new YandexAuth(
              this))
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
              m_albumService,
              m_artistService,
              m_playbackController,
              this))
    , m_personalController(
          new PersonalController(
              m_yandexPersonal,
              m_personalLanding,
              m_playbackController,
              m_playerService,
              this))
    , m_searchController(
          new SearchController(
              m_searchService,
              m_playbackController,
              this))
{
    /*
     * Account
     */

    connect(
        m_accountService,
        &AccountService::accountReceived,
        this,
        [this](
            const Account &account) {

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

    /*
     * Search
     */

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

    /*
     * Library
     */

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
        &LibraryController::loadingAlbumChanged,
        this,
        &AppController::loadingAlbumChanged);

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
        &LibraryController::currentAlbumChanged,
        this,
        &AppController::currentAlbumChanged);

    connect(
        m_libraryController,
        &LibraryController::currentArtistChanged,
        this,
        &AppController::currentArtistChanged);

    /*
     * Personal
     */

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

    connect(
        m_personalController,
        &PersonalController::recentListeningSelected,
        this,
        [this](
            const RecentListeningItem &item) {

            m_libraryController
                ->selectRecentListening(
                    item);
        });

    /*
     * Playback
     */

    connect(
        m_playbackController,
        &PlaybackController::currentTrackChanged,
        this,
        [this]() {

            emit currentTrackChanged();

            const Track track =
                m_playbackController
                    ->currentTrack();

            if (track.id.isEmpty()) {
                return;
            }

            QString artistName;

            if (!track.artists.isEmpty()) {

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

    /*
     * Player
     */

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

void AppController::loadArtist(
    const QString &id)
{
    m_libraryController
        ->loadArtist(
            id);
}

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
    int index)
{
    m_personalController
        ->selectPersonalPlaylist(
            index);
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
    m_libraryController
        ->selectAlbumTrack(
            index);
}

void AppController::selectArtistTrack(
    int index)
{
    m_libraryController
        ->selectArtistTrack(
            index);
}

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
        mode <
            QueueService::RepeatOff ||
        mode >
            QueueService::RepeatOne
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

/*
 * Search
 */

SearchModel *
AppController::searchModel() const
{
    return m_searchController
        ->model();
}

bool AppController::isSearching() const
{
    return m_searchController
        ->isSearching();
}

/*
 * Personal models
 */

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

RecentListeningModel *
AppController::recentListeningModel() const
{
    return m_personalController
        ->recentListeningModel();
}

/*
 * Library models
 */

PlaylistModel *
AppController::playlistModel() const
{
    return m_libraryController
        ->playlistModel();
}

AlbumModel *
AppController::albumModel() const
{
    return m_libraryController
        ->albumModel();
}

ArtistModel *
AppController::artistModel() const
{
    return m_libraryController
        ->artistModel();
}

/*
 * General state
 */

bool AppController::isPlaying() const
{
    return m_playerService
        ->isPlaying();
}

bool AppController::isLoadingMyWave() const
{
    return m_personalController
        ->isLoadingMyWave();
}

bool AppController::isLoadingMoreMyWave() const
{
    return m_personalController
        ->isLoadingMoreMyWave();
}

bool AppController::isLoadingRecommendations() const
{
    return m_personalController
        ->isLoadingRecommendations();
}

bool AppController::isLoadingPlaylist() const
{
    return m_libraryController
        ->isLoadingPlaylist();
}

bool AppController::isLoadingAlbum() const
{
    return m_libraryController
        ->isLoadingAlbum();
}

bool AppController::isLoadingArtist() const
{
    return m_libraryController
        ->isLoadingArtist();
}

/*
 * Library state
 */

QString AppController::currentPlaylistTitle() const
{
    return m_libraryController
        ->currentPlaylistTitle();
}

int AppController::currentPlaylistTrackCount() const
{
    return m_libraryController
        ->currentPlaylistTrackCount();
}

QString AppController::currentAlbumTitle() const
{
    return m_libraryController
        ->currentAlbumTitle();
}

int AppController::currentAlbumTrackCount() const
{
    return m_libraryController
        ->currentAlbumTrackCount();
}

QString AppController::currentAlbumCoverUri() const
{
    return m_libraryController
        ->currentAlbumCoverUri();
}

QString AppController::currentArtistName() const
{
    return m_libraryController
        ->currentArtistName();
}

QString AppController::currentArtistCoverUri() const
{
    return m_libraryController
        ->currentArtistCoverUri();
}

QString AppController::currentArtistGenres() const
{
    return m_libraryController
        ->currentArtistGenres();
}

int AppController::currentArtistTrackCount() const
{
    return m_libraryController
        ->currentArtistTrackCount();
}

/*
 * Playback state
 */

QString AppController::currentTrackTitle() const
{
    return m_playbackController
        ->currentTrack()
        .title;
}

QString AppController::currentTrackArtist() const
{
    const Track track =
        m_playbackController
            ->currentTrack();

    if (track.artists.isEmpty()) {
        return {};
    }

    return track.artists
        .first()
        .name;
}

QString AppController::currentTrackArtistId() const
{
    const Track track =
        m_playbackController
            ->currentTrack();

    if (track.artists.isEmpty()) {
        return {};
    }

    return track.artists
        .first()
        .id;
}

QString AppController::currentTrackCoverUri() const
{
    return m_playbackController
        ->currentTrack()
        .coverUri;
}

qint64 AppController::position() const
{
    return m_playerService
        ->position();
}

qint64 AppController::duration() const
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

int AppController::repeatMode() const
{
    return static_cast<int>(
        m_playbackController
            ->repeatMode());
}

bool AppController::shuffleEnabled() const
{
    return m_playbackController
        ->shuffleEnabled();
}