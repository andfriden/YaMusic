#include "AppController.h"

#include "../Models/PersonalPlaylist.h"
#include "../Player/PlayerService.h"
#include "../Queue/QueueService.h"
#include "PlayerAccentService.h"

#include "../Yandex/Account/AccountService.h"
#include "../Yandex/Auth/YandexAuth.h"
#include "../Yandex/Catalog/AlbumService.h"
#include "../Yandex/Catalog/ArtistService.h"
#include "../Yandex/Catalog/ChartService.h"
#include "../Yandex/Catalog/GenreService.h"
#include "../Yandex/Catalog/SearchService.h"
#include "../Yandex/Catalog/TrackService.h"
#include "../Yandex/Personal/LikesService.h"
#include "../Yandex/Personal/NewPlaylistsService.h"
#include "../Yandex/Personal/PersonalLanding.h"
#include "../Yandex/Personal/PlaylistService.h"
#include "../Yandex/Personal/RecentListeningService.h"
#include "../Yandex/Personal/YandexPersonal.h"

AppController::AppController(QObject *parent)
    : QObject(parent)
    , m_auth(new YandexAuth(this))
    , m_accountService(new AccountService(m_auth, this))
    , m_searchService(new SearchService(m_auth, this))
    , m_trackService(new TrackService(m_auth, this))
    , m_yandexPersonal(new YandexPersonal(m_auth, this))
    , m_personalLanding(new PersonalLanding(m_auth, this))
    , m_recentListeningService(new RecentListeningService(m_auth, this))
    , m_playlistService(new PlaylistService(m_auth, this))
    , m_newPlaylistsService(new NewPlaylistsService(m_auth, m_playlistService, this))
    , m_likesService(new LikesService(m_auth, this))
    , m_albumService(new AlbumService(m_auth, this))
    , m_artistService(new ArtistService(m_auth, this))
    , m_chartService(new ChartService(m_auth, this))
    , m_genreService(new GenreService(m_auth, this))
    , m_playerService(new PlayerService(this))
    , m_queueService(new QueueService(this))
    , m_playerAccentService(new PlayerAccentService(this))
    , m_playbackController(new PlaybackController(
          m_trackService, m_playerService, m_queueService, this))
    , m_libraryController(new LibraryController(
          m_playlistService, m_artistService, m_likesService, m_playbackController, this))
    , m_personalController(new PersonalController(
          m_yandexPersonal, m_personalLanding, m_newPlaylistsService,
          m_recentListeningService, m_playbackController, m_playerService, this))
    , m_searchController(new SearchController(
          m_searchService, m_playbackController, this))
    , m_albumController(new AlbumController(
          m_albumService, m_artistService, m_playbackController, this))
    , m_artistController(new ArtistController(
          m_artistService, m_playbackController, this))
    , m_chartController(new ChartController(
          m_chartService, m_playbackController, this))
, m_genreController(new GenreController(m_genreService,  m_playlistService,
    m_playbackController,   this))
{
    connectAccount();
    connectSearch();
    connectLibrary();
    connectAlbum();
    connectPersonal();
    connectArtist();
    connectChart();
    connectGenre();
    connectPlayback();
    connectPlayer();

    connect(m_queueService, &QueueService::queueChanged, this, [this]() {
        emit queueChanged();
        emit playbackSourceChanged();
    });

    connect(m_queueService, &QueueService::currentChanged, this, [this]() {
        emit queueChanged();
    });

    connect(m_playerAccentService, &PlayerAccentService::accentColorChanged,
            this, &AppController::playerAccentChanged);

    m_accountService->loadAccount();
}

void AppController::connectAccount()
{
    connect(m_accountService, &AccountService::accountReceived, this,
            [this](const Account &account) {
                m_accountUid = QString::number(account.uid);
                m_recentListeningService->setUserId(m_accountUid);
                m_recentListeningService->load(50, 10);

                emit statusChanged(
                    QString("Выполнен вход: %1 (uid: %2)")
                        .arg(account.displayName)
                        .arg(account.uid));
            });

    connect(m_accountService, &AccountService::errorOccurred,
            this, &AppController::statusChanged);
}

void AppController::connectSearch()
{
    connect(m_searchController, &SearchController::statusChanged,
            this, &AppController::statusChanged);

    connect(m_searchController, &SearchController::searchingChanged,
            this, &AppController::searchingChanged);
}

void AppController::connectLibrary()
{
    connect(m_libraryController, &LibraryController::statusChanged,
            this, &AppController::statusChanged);

    connect(m_libraryController, &LibraryController::loadingLibraryPlaylistsChanged,
            this, &AppController::loadingLibraryPlaylistsChanged);

    connect(m_libraryController, &LibraryController::loadingLikedTracksChanged,
            this, &AppController::loadingLikedTracksChanged);

    connect(m_libraryController, &LibraryController::loadingPlaylistChanged,
            this, &AppController::loadingPlaylistChanged);

    connect(m_libraryController, &LibraryController::loadingArtistChanged,
            this, &AppController::loadingArtistChanged);

    connect(m_libraryController, &LibraryController::currentPlaylistChanged,
            this, &AppController::currentPlaylistChanged);

    connect(m_libraryController, &LibraryController::currentArtistChanged,
            this, &AppController::currentArtistChanged);
}

void AppController::connectAlbum()
{
    connect(m_albumController, &AlbumController::statusChanged,
            this, &AppController::statusChanged);

    connect(m_albumController, &AlbumController::loadingChanged,
            this, &AppController::loadingAlbumChanged);

    connect(m_albumController, &AlbumController::albumChanged, this, [this]() {
        emit currentAlbumChanged();

        if (!m_playAlbumAfterLoad)
            return;

        m_playAlbumAfterLoad = false;

        if (m_albumController->albumModel()->count() <= 0) {
            emit statusChanged("В альбоме нет треков");
            return;
        }

        m_albumController->playAlbum();
    });
}

void AppController::connectPersonal()
{
    connect(m_personalController, &PersonalController::statusChanged,
            this, &AppController::statusChanged);

    connect(m_personalController, &PersonalController::loadingMyWaveChanged,
            this, &AppController::loadingMyWaveChanged);

    connect(m_personalController, &PersonalController::loadingMoreMyWaveChanged,
            this, &AppController::loadingMoreMyWaveChanged);

    connect(m_personalController, &PersonalController::loadingRecommendationsChanged,
            this, &AppController::loadingRecommendationsChanged);

    connect(m_personalController, &PersonalController::recommendationsLoaded,
            this, &AppController::recommendationsLoaded);

    connect(m_personalController, &PersonalController::personalPlaylistSelected,
            this, [this](const PersonalPlaylist &playlist) {
                m_libraryController->loadPlaylist(playlist.uid, playlist.kind);
            });
}

void AppController::connectArtist()
{
    connect(m_artistController, &ArtistController::statusChanged,
            this, &AppController::statusChanged);

    connect(m_artistController, &ArtistController::artistChanged,
            this, &AppController::currentArtistChanged);

    connect(m_artistController, &ArtistController::similarArtistSelected,
            this, [this](const QString &artistId) {
                loadArtist(artistId);
            });
}

void AppController::connectChart()
{
    connect(m_chartController, &ChartController::statusChanged,
            this, &AppController::statusChanged);
}

void AppController::connectGenre()
{
    connect(m_genreController, &GenreController::statusChanged,
            this, &AppController::statusChanged);
}

void AppController::connectPlayback()
{
    connect(m_playbackController, &PlaybackController::currentTrackChanged,
            this, [this]() {
                emit currentTrackChanged();

                const Track track = m_playbackController->currentTrack();
                if (track.id.isEmpty())
                    return;

                m_playerAccentService->updateForCover(track.coverUri);

                QString artistName;
                if (!track.artists.isEmpty())
                    artistName = track.artists.first().name;

                const QString message = artistName.isEmpty()
                    ? QString("Выбран трек: %1").arg(track.title)
                    : QString("Выбран трек: %1 — %2")
                          .arg(track.title)
                          .arg(artistName);

                emit statusChanged(message);
            });

    connect(m_playbackController, &PlaybackController::stateChanged,
            this, &AppController::playbackStateChanged);

    connect(m_playbackController, &PlaybackController::repeatModeChanged,
            this, &AppController::repeatModeChanged);

    connect(m_playbackController, &PlaybackController::shuffleChanged,
            this, &AppController::shuffleChanged);

    connect(m_playbackController, &PlaybackController::playbackError,
            this, &AppController::statusChanged);
}

void AppController::connectPlayer()
{
    connect(m_playerService, &PlayerService::playingChanged, this, [this]() {
        emit playingChanged();

        if (m_playerService->isPlaying())
            emit statusChanged("Воспроизведение");
    });

    connect(m_playerService, &PlayerService::positionChanged,
            this, [this](qint64) {
                emit positionChanged();
            });

    connect(m_playerService, &PlayerService::durationChanged,
            this, [this](qint64) {
                emit durationChanged();
            });

    connect(m_playerService, &PlayerService::volumeChanged,
            this, &AppController::volumeChanged);

    connect(m_playerService, &PlayerService::playbackPaused,
            this, [this]() {
                emit statusChanged("Пауза");
            });

    connect(m_playerService, &PlayerService::playbackStopped,
            this, [this]() {
                emit statusChanged("Остановлено");
            });

    connect(m_playerService, &PlayerService::errorOccurred,
            this, [this](const QString &message) {
                emit statusChanged(
                    QString("Ошибка воспроизведения: %1").arg(message));
            });
}

void AppController::testConnection()
{
    emit statusChanged("Приложение работает");
}

void AppController::testYandexApi()
{
    m_accountService->loadAccount();
    emit statusChanged("Проверка аккаунта Яндекс Музыки...");
}

void AppController::testSearch(const QString &query)
{
    m_searchController->search(query);
}

QString AppController::playbackSourceTitle() const
{
    return m_queueService->sourceTitle();
}

QString AppController::playbackSourceType() const
{
    return m_queueService->sourceType();
}

int AppController::queueCount() const
{
    return m_queueService->count();
}

int AppController::queueCurrentIndex() const
{
    return m_queueService->currentIndex();
}

QVariantMap AppController::queueTrackData(int index) const
{
    QVariantMap result;
    const Track track = m_queueService->trackAt(index);

    if (track.id.isEmpty())
        return result;

    result.insert("id", track.id);
    result.insert("title", track.title);
    result.insert("coverUri", track.coverUri);
    result.insert("durationMs", track.durationMs);

    if (!track.artists.isEmpty()) {
        result.insert("artist", track.artists.first().name);
        result.insert("artistId", track.artists.first().id);
    } else {
        result.insert("artist", "");
        result.insert("artistId", "");
    }

    return result;
}

SearchModel *AppController::searchModel() const
{
    return m_searchController->model();
}

MyWaveModel *AppController::myWaveModel() const
{
    return m_personalController->myWaveModel();
}

PersonalPlaylistsModel *AppController::personalPlaylistsModel() const
{
    return m_personalController->personalPlaylistsModel();
}

PlaylistModel *AppController::playlistModel() const
{
    return m_libraryController->playlistModel();
}

RecentListeningModel *AppController::recentListeningModel() const
{
    return m_personalController->recentListeningModel();
}

AlbumController *AppController::albumController() const
{
    return m_albumController;
}

ArtistController *AppController::artistController() const
{
    return m_artistController;
}

ChartController *AppController::chartController() const
{
    return m_chartController;
}

GenreController *AppController::genreController() const
{
    return m_genreController;
}

bool AppController::isSearching() const
{
    return m_searchController->isSearching();
}

bool AppController::isPlaying() const
{
    return m_playerService->isPlaying();
}

bool AppController::isLoadingMyWave() const
{
    return m_personalController->isLoadingMyWave();
}

bool AppController::isLoadingMoreMyWave() const
{
    return m_personalController->isLoadingMoreMyWave();
}

bool AppController::isLoadingRecommendations() const
{
    return m_personalController->isLoadingRecommendations();
}

bool AppController::isLoadingPlaylist() const
{
    return m_libraryController->isLoadingPlaylist();
}

bool AppController::isLoadingAlbum() const
{
    return m_albumController->isLoading();
}

bool AppController::isLoadingArtist() const
{
    return m_artistController->isLoading();
}

QString AppController::currentPlaylistTitle() const
{
    return m_libraryController->currentPlaylistTitle();
}

QString AppController::currentPlaylistCoverUri() const
{
    return m_libraryController->currentPlaylistCoverUri();
}

int AppController::currentPlaylistTrackCount() const
{
    return m_libraryController->currentPlaylistTrackCount();
}

QString AppController::currentAlbumTitle() const
{
    return m_albumController->albumTitle();
}

int AppController::currentAlbumTrackCount() const
{
    return m_albumController->albumTrackCount();
}

QString AppController::currentAlbumCoverUri() const
{
    return m_albumController->albumCoverUri();
}

QString AppController::currentArtistName() const
{
    return m_artistController->artistName();
}

QString AppController::currentArtistCoverUri() const
{
    return m_artistController->artistCoverUri();
}

QString AppController::currentArtistGenres() const
{
    return m_artistController->artistGenres();
}

int AppController::currentArtistTrackCount() const
{
    return m_artistController->artistModel()->count();
}

QString AppController::currentTrackId() const
{
    return m_playbackController->currentTrack().id;
}

QString AppController::currentTrackTitle() const
{
    return m_playbackController->currentTrack().title;
}

QString AppController::currentTrackArtist() const
{
    const Track track = m_playbackController->currentTrack();
    return track.artists.isEmpty() ? QString() : track.artists.first().name;
}

QString AppController::currentTrackArtistId() const
{
    const Track track = m_playbackController->currentTrack();
    return track.artists.isEmpty() ? QString() : track.artists.first().id;
}

QString AppController::currentTrackAlbumTitle() const
{
    const Track track = m_playbackController->currentTrack();
    return track.albums.isEmpty() ? QString() : track.albums.first().title;
}

QString AppController::currentTrackAlbumId() const
{
    const Track track = m_playbackController->currentTrack();
    return track.albums.isEmpty() ? QString() : track.albums.first().id;
}

QString AppController::currentTrackCoverUri() const
{
    return m_playbackController->currentTrack().coverUri;
}

qint64 AppController::position() const
{
    return m_playerService->position();
}

qint64 AppController::duration() const
{
    return m_playerService->duration();
}

PlaybackController::PlaybackState AppController::playbackState() const
{
    return m_playbackController->state();
}

int AppController::repeatMode() const
{
    return static_cast<int>(m_playbackController->repeatMode());
}

bool AppController::shuffleEnabled() const
{
    return m_playbackController->shuffleEnabled();
}

float AppController::volume() const
{
    return m_playerService->volume();
}

void AppController::setVolume(float volume)
{
    m_playerService->setVolume(volume);
}

void AppController::loadGenres()
{
    m_genreController->loadGenres();
}

QColor AppController::playerAccent() const
{
    return m_playerAccentService->accentColor();
}