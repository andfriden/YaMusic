#include "AppController.h"
#include "../Models/PersonalPlaylist.h"
#include "../Player/PlayerService.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Account/AccountService.h"
#include "../Yandex/Auth/YandexAuth.h"
#include "../Yandex/Catalog/AlbumService.h"
#include "../Yandex/Catalog/ArtistService.h"
#include "../Yandex/Catalog/ChartService.h"
#include "../Yandex/Catalog/GenreService.h"
#include "../Yandex/Catalog/SearchService.h"
#include "../Yandex/Catalog/StationService.h"
#include "../Yandex/Catalog/TrackService.h"
#include "../Yandex/Personal/LikesService.h"
#include "../Yandex/Personal/NewPlaylistsService.h"
#include "../Yandex/Personal/PersonalLanding.h"
#include "../Yandex/Personal/PlaylistService.h"
#include "../Yandex/Personal/RecentListeningService.h"
#include "../Yandex/Personal/YandexPersonal.h"
#include "PlayerAccentService.h"

#include <QClipboard>
#include <QGuiApplication>

AppController::AppController(YandexAuth *auth, AccountService *accountService, QObject *parent)
    : QObject(parent), m_auth(auth), m_accountService(accountService),
      m_searchService(new SearchService(m_auth, this)),
      m_trackService(new TrackService(m_auth, this)),
      m_yandexPersonal(new YandexPersonal(m_auth, this)),
      m_personalLanding(new PersonalLanding(m_auth, this)),
      m_recentListeningService(new RecentListeningService(m_auth, this)),
      m_playlistService(new PlaylistService(m_auth, this)),
      m_newPlaylistsService(new NewPlaylistsService(m_auth, m_playlistService, this)),
      m_likesService(new LikesService(m_auth, this)),
      m_albumService(new AlbumService(m_auth, this)),
      m_artistService(new ArtistService(m_auth, this)),
      m_chartService(new ChartService(m_auth, this)),
      m_genreService(new GenreService(m_auth, this)),
      m_stationService(new StationService(m_auth, this)), m_playerService(new PlayerService(this)),
      m_queueService(new QueueService(this)), m_playerAccentService(new PlayerAccentService(this)),
      m_playbackController(
          new PlaybackController(m_trackService, m_playerService, m_queueService, this)),
      m_libraryController(new LibraryController(m_playlistService, m_artistService, m_likesService,
                                                m_playbackController, this)),
      m_personalController(new PersonalController(m_yandexPersonal, m_personalLanding,
                                                  m_newPlaylistsService, m_recentListeningService,
                                                  m_playbackController, m_playerService, this)),
      m_searchController(new SearchController(m_searchService, m_playbackController, this)),
      m_similarTracksModel(new SimilarTracksModel(this)),
      m_albumController(new AlbumController(m_albumService, m_artistService, m_playbackController,
                                            m_likesService, this)),
      m_artistController(new ArtistController(m_artistService, m_playbackController, this)),
      m_chartController(new ChartController(m_chartService, m_playbackController, this)),
      m_genreController(new GenreController(m_genreService, m_playlistService, m_stationService,
                                            m_playbackController, this)),
      m_lyricsController(
          new LyricsController(m_trackService, m_playbackController, m_playerService, this)),
      m_themeController(new ThemeController(this)),
      m_accentController(new AccentController(m_playerAccentService, m_playbackController, this)) {
  Q_ASSERT(m_auth != nullptr);
  Q_ASSERT(m_accountService != nullptr);

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
  connect(m_queueService, &QueueService::currentChanged, this, &AppController::queueChanged);

  connect(m_likesService, &LikesService::likeChanged, this, [this](const QString &trackId, bool) {
    if (trackId == currentTrackId()) {
      emit currentTrackChanged();
    }
  });

  connect(m_likesService, &LikesService::albumLikeChanged, this,
          [this](const QString &, bool) { emit currentAlbumChanged(); });

  connect(m_likesService, &LikesService::artistLikeChanged, this,
          [this](const QString &, bool) { emit currentArtistChanged(); });

  connect(m_trackService, &TrackService::similarTracksReceived, this,
          [this](const QList<Track> &tracks) { m_similarTracksModel->setSimilarTracks(tracks); });

  // TODO(#482): вынести uid-провайдер в PlaybackController
  m_accountService->loadAccount();
}

void AppController::connectAccount() {
  connect(m_accountService, &AccountService::accountReceived, this, [this](const Account &account) {
    m_accountUid = QString::number(account.uid);
    m_recentListeningService->setUserId(m_accountUid);
    m_recentListeningService->load(50, 10);
    m_libraryController->setUserId(m_accountUid);

    m_playbackController->setUidProvider([this]() -> QString { return m_accountUid; });

    // Список личных плейлистов нужен в пикере «Добавить в плейлист»
    // на всех страницах — грузим сразу при входе, а не только
    // при открытии Медиатеки.
    m_libraryController->loadUserPlaylists(m_accountUid);
    m_personalController->loadMyWave();
    m_personalController->loadRecommendations();

    emit statusChanged(
        QStringLiteral("Выполнен вход: %1 (uid: %2)").arg(account.displayName).arg(account.uid));
  });
  connect(m_accountService, &AccountService::errorOccurred, this, &AppController::statusChanged);
}

void AppController::connectSearch() {
  connect(m_searchController, &SearchController::statusChanged, this,
          &AppController::statusChanged);

  connect(m_searchController, &SearchController::searchingChanged, this,
          &AppController::searchingChanged);

  connect(m_searchController, &SearchController::artistClicked, this,
          [this](const QString &artistId) { loadArtist(artistId); });

  connect(m_searchController, &SearchController::albumClicked, this,
          [this](const QString &albumId) { loadAlbum(albumId); });

  connect(m_searchController, &SearchController::playlistClicked, this,
          [this](const QString &uid, int kind) { selectPersonalPlaylist(uid, kind); });
}

void AppController::connectLibrarySignals() {
  connect(m_libraryController, &LibraryController::loadingLibraryPlaylistsChanged, this,
          &AppController::loadingLibraryPlaylistsChanged);

  connect(m_libraryController, &LibraryController::loadingLikedTracksChanged, this,
          &AppController::loadingLikedTracksChanged);

  connect(m_libraryController, &LibraryController::loadingLikedAlbumsChanged, this,
          &AppController::loadingLikedAlbumsChanged);

  connect(m_libraryController, &LibraryController::loadingLikedArtistsChanged, this,
          &AppController::loadingLikedArtistsChanged);

  connect(m_libraryController, &LibraryController::loadingPlaylistChanged, this,
          &AppController::loadingPlaylistChanged);

  connect(m_libraryController, &LibraryController::loadingArtistChanged, this,
          &AppController::loadingArtistChanged);
}

void AppController::connectLibrary() {
  connect(m_libraryController, &LibraryController::statusChanged, this,
          &AppController::statusChanged);
  connectLibrarySignals();

  connect(m_libraryController, &LibraryController::currentPlaylistChanged, this,
          &AppController::currentPlaylistChanged);

  connect(m_libraryController, &LibraryController::similarPlaylistsChanged, this,
          &AppController::similarPlaylistsChanged);

  connect(m_libraryController, &LibraryController::currentArtistChanged, this,
          &AppController::currentArtistChanged);
}

void AppController::connectAlbum() {
  connect(m_albumController, &AlbumController::statusChanged, this, &AppController::statusChanged);

  connect(m_albumController, &AlbumController::loadingChanged, this,
          &AppController::loadingAlbumChanged);

  connect(m_albumController, &AlbumController::albumChanged, this, [this]() {
    emit currentAlbumChanged();

    if (!m_playAlbumAfterLoad) {
      return;
    }

    m_playAlbumAfterLoad = false;

    if (m_albumController->albumModel()->count() <= 0) {
      emit statusChanged("В альбоме нет треков");
      return;
    }

    m_albumController->playAlbum();
  });
}

void AppController::connectPersonal() {
  connect(m_personalController, &PersonalController::statusChanged, this,
          &AppController::statusChanged);

  connect(m_personalController, &PersonalController::loadingMyWaveChanged, this,
          &AppController::loadingMyWaveChanged);

  connect(m_personalController, &PersonalController::loadingMoreMyWaveChanged, this,
          &AppController::loadingMoreMyWaveChanged);

  connect(m_personalController, &PersonalController::loadingRecommendationsChanged, this,
          &AppController::loadingRecommendationsChanged);

  connect(m_personalController, &PersonalController::recommendationsLoaded, this,
          &AppController::recommendationsLoaded);

  connect(m_personalController, &PersonalController::personalPlaylistSelected, this,
          [this](const PersonalPlaylist &playlist) {
            m_libraryController->loadPlaylist(playlist.uid, playlist.kind);

            // Для персональных подборок («Собираем для вас»)
            // сервер не отдаёт similar-entities, поэтому
            // подставляем остальные плейлисты из того же раздела.

            m_libraryController->setSimilarPlaylistsFallback(
                m_personalController->recommendationPlaylistsData(playlist.uid, playlist.kind));
          });
}

void AppController::connectArtist() {
  connect(m_artistController, &ArtistController::statusChanged, this,
          &AppController::statusChanged);

  connect(m_artistController, &ArtistController::artistChanged, this,
          &AppController::currentArtistChanged);

  connect(m_artistController, &ArtistController::similarArtistSelected, this,
          [this](const QString &artistId) { loadArtist(artistId); });
}

void AppController::connectChart() {
  connect(m_chartController, &ChartController::statusChanged, this, &AppController::statusChanged);
}

void AppController::connectGenre() {
  connect(m_genreController, &GenreController::statusChanged, this, &AppController::statusChanged);
}

void AppController::connectPlayback() {
  connect(m_playbackController, &PlaybackController::currentTrackChanged, this, [this]() {
    emit currentTrackChanged();
    const Track track = m_playbackController->currentTrack();

    if (track.id.isEmpty()) {
      return;
    }

    m_trackService->loadSimilarTracks(track.id);
    QString artistName;
    if (!track.artists.isEmpty()) artistName = track.artists.first().name;

    const QString message = artistName.isEmpty()
                                ? QStringLiteral("Выбран трек: %1").arg(track.title)
                                : QStringLiteral("Выбран трек: %1 — %2").arg(track.title).arg(artistName);
    emit statusChanged(message);
  });

  connect(m_playbackController, &PlaybackController::stateChanged, this,
          &AppController::playbackStateChanged);

  connect(m_playbackController, &PlaybackController::repeatModeChanged, this,
          &AppController::repeatModeChanged);

  connect(m_playbackController, &PlaybackController::shuffleChanged, this,
          &AppController::shuffleChanged);

  connect(m_playbackController, &PlaybackController::playbackError, this,
          &AppController::statusChanged);

  connect(m_playbackController, &PlaybackController::playlistExhausted, this,
          [this](const QString &sourceType, const QString &) {
            if (sourceType == "playlist") {
              const QVariantList similar = m_libraryController->similarPlaylists();

              for (const QVariant &item : similar) {
                const QVariantMap map = item.toMap();
                const QString uid = map.value("uid").toString();
                const int kind = map.value("kind").toInt();
                if (uid.isEmpty() || kind <= 0) continue;
                selectPersonalPlaylist(uid, kind);
                emit statusChanged("Похожий плейлист");
                return;
              }

            } else if (sourceType == "artist") {
              const SimilarArtistsModel *similar = m_artistController->similarArtistsModel();

              if (similar->count() <= 0) {
                return;
              }

              const Artist first = similar->artistAt(0);

              if (first.id.isEmpty()) {
                return;
              }

              loadArtist(first.id);
              emit statusChanged(QStringLiteral("Похожий исполнитель: %1").arg(first.name));

            } else if (sourceType == "album") {
              const QString artistId = currentTrackArtistId();
              if (artistId.isEmpty()) return;
              loadArtist(artistId);
              emit statusChanged("Исполнитель альбома");
            }
          });
}

void AppController::connectPlayer() {
  connect(m_playerService, &PlayerService::playingChanged, this, [this]() {
    emit playingChanged();
    if (m_playerService->isPlaying()) emit statusChanged("Воспроизведение");
  });

  connect(m_playerService, &PlayerService::positionChanged, this, [this](qint64) {
    emit positionChanged();
    m_playbackController->systemMediaControls()->setPosition(m_playerService->position());
  });

  connect(m_playerService, &PlayerService::durationChanged, this,
          [this](qint64) { emit durationChanged(); });
  connect(m_playerService, &PlayerService::volumeChanged, this, &AppController::volumeChanged);

  connect(m_playerService, &PlayerService::playbackPaused, this,
          [this]() { emit statusChanged("Пауза"); });

  connect(m_playerService, &PlayerService::playbackStopped, this,
          [this]() { emit statusChanged("Остановлено"); });

  connect(m_playerService, &PlayerService::errorOccurred, this, [this](const QString &message) {
    emit statusChanged(QStringLiteral("Ошибка воспроизведения: %1").arg(message));
  });
}

void AppController::toggleLike(const QString &trackId, bool liked) {
  if (m_accountUid.isEmpty()) {
    emit statusChanged("Пользователь не авторизован");
    return;
  }

  const QString id = trackId.trimmed();

  if (id.isEmpty()) {
    emit statusChanged("ID трека не указан");
    return;
  }

  if (liked) {
    m_likesService->removeLike(m_accountUid, id);
  } else {
    m_likesService->addLike(m_accountUid, id);
  }
}

void AppController::toggleAlbumLike() {
  const QString albumId = m_albumController->albumId();

  // почему-то accountReceived иногда приходит дважды — завёл баг
  if (m_accountUid.isEmpty() || albumId.isEmpty()) {
    return;
  }

  if (m_likesService->isAlbumLiked(albumId)) {
    m_likesService->removeAlbumLike(m_accountUid, albumId);
  } else {
    m_likesService->addAlbumLike(m_accountUid, albumId);
  }
}

void AppController::toggleArtistLike() {
  const QString artistId = m_artistController->artistId();

  if (m_accountUid.isEmpty() || artistId.isEmpty()) {
    return;
  }

  if (m_likesService->isArtistLiked(artistId)) {
    m_likesService->removeArtistLike(m_accountUid, artistId);
  } else {
    m_likesService->addArtistLike(m_accountUid, artistId);
  }
}

QString AppController::playbackSourceTitle() const {
  return m_queueService->sourceTitle();
}

QString AppController::playbackSourceType() const {
  return m_queueService->sourceType();
}

int AppController::queueCount() const {
  return m_queueService->count();
}

int AppController::queueCurrentIndex() const {
  return m_queueService->currentIndex();
}

QVariantMap AppController::queueTrackData(int index) const {
  QVariantMap result;
  const Track track = m_queueService->trackAt(index);
  if (track.id.isEmpty()) return result;
  result.insert("id", track.id);
  result.insert("title", track.title);
  result.insert("coverUri", track.coverUri);
  result.insert("durationMs", static_cast<qlonglong>(track.durationMs));

  if (!track.artists.isEmpty()) {
    result.insert("artist", track.artists.first().name);
    result.insert("artistId", track.artists.first().id);
  } else {
    result.insert("artist", "");
    result.insert("artistId", "");
  }
  return result;
}

SearchModel *AppController::searchModel() const {
  return m_searchController->model();
}

SearchArtistsModel *AppController::searchArtistsModel() const {
  return m_searchController->artistsModel();
}

SearchAlbumsModel *AppController::searchAlbumsModel() const {
  return m_searchController->albumsModel();
}

SearchPlaylistsModel *AppController::searchPlaylistsModel() const {
  return m_searchController->playlistsModel();
}

SimilarTracksModel *AppController::similarTracksModel() const {
  return m_similarTracksModel;
}

MyWaveModel *AppController::myWaveModel() const {
  return m_personalController->myWaveModel();
}

PersonalPlaylistsModel *AppController::personalPlaylistsModel() const {
  return m_personalController->personalPlaylistsModel();
}

PlaylistModel *AppController::playlistModel() const {
  return m_libraryController->playlistModel();
}

RecentListeningModel *AppController::recentListeningModel() const {
  return m_personalController->recentListeningModel();
}

AlbumController *AppController::albumController() const {
  return m_albumController;
}

ArtistController *AppController::artistController() const {
  return m_artistController;
}

ChartController *AppController::chartController() const {
  return m_chartController;
}

GenreController *AppController::genreController() const {
  return m_genreController;
}

LyricsController *AppController::lyricsController() const {
  return m_lyricsController;
}

ThemeController *AppController::themeController() const {
  return m_themeController;
}

AccentController *AppController::accentController() const {
  return m_accentController;
}

bool AppController::isSearching() const {
  return m_searchController->isSearching();
}

bool AppController::isPlaying() const {
  return m_playerService->isPlaying();
}

bool AppController::isLoadingMyWave() const {
  return m_personalController->isLoadingMyWave();
}

bool AppController::isLoadingMoreMyWave() const {
  return m_personalController->isLoadingMoreMyWave();
}

bool AppController::isLoadingRecommendations() const {
  return m_personalController->isLoadingRecommendations();
}

bool AppController::isLoadingPlaylist() const {
  return m_libraryController->isLoadingPlaylist();
}

bool AppController::isLoadingAlbum() const {
  return m_albumController->isLoading();
}

bool AppController::isLoadingArtist() const {
  return m_artistController->isLoading();
}

QString AppController::currentPlaylistTitle() const {
  return m_libraryController->currentPlaylistTitle();
}

QString AppController::currentPlaylistCoverUri() const {
  return m_libraryController->currentPlaylistCoverUri();
}

int AppController::currentPlaylistTrackCount() const {
  return m_libraryController->currentPlaylistTrackCount();
}

int AppController::currentPlaylistKind() const {
  return m_libraryController->currentPlaylistKind();
}

QVariantList AppController::similarPlaylists() const {
  return m_libraryController->similarPlaylists();
}

QString AppController::currentAlbumTitle() const {
  return m_albumController->albumTitle();
}

int AppController::currentAlbumTrackCount() const {
  return m_albumController->albumTrackCount();
}

QString AppController::currentAlbumCoverUri() const {
  return m_albumController->albumCoverUri();
}

bool AppController::currentAlbumLiked() const {
  const QString albumId = m_albumController->albumId();

  if (albumId.isEmpty()) {
    return false;
  }
  return m_likesService->isAlbumLiked(albumId);
}

QString AppController::currentArtistName() const {
  return m_artistController->artistName();
}

QString AppController::currentArtistCoverUri() const {
  return m_artistController->artistCoverUri();
}

QString AppController::currentArtistGenres() const {
  return m_artistController->artistGenres();
}

int AppController::currentArtistTrackCount() const {
  return m_artistController->artistModel()->count();
}

bool AppController::currentArtistLiked() const {
  const QString artistId = m_artistController->artistId();
  if (artistId.isEmpty()) return false;
  return m_likesService->isArtistLiked(artistId);
}

QString AppController::currentTrackId() const {
  return m_playbackController->currentTrack().id;
}

bool AppController::currentTrackLiked() const {
  const QString trackId = currentTrackId();
  if (trackId.isEmpty()) return false;
  return m_likesService->isLiked(trackId);
}

QString AppController::currentTrackTitle() const {
  return m_playbackController->currentTrack().title;
}

QString AppController::currentTrackArtist() const {
  const Track track = m_playbackController->currentTrack();
  return track.artists.isEmpty() ? QString() : track.artists.first().name;
}

QString AppController::currentTrackArtistId() const {
  const Track track = m_playbackController->currentTrack();
  return track.artists.isEmpty() ? QString() : track.artists.first().id;
}

QString AppController::currentTrackAlbumTitle() const {
  const Track track = m_playbackController->currentTrack();
  return track.albums.isEmpty() ? QString() : track.albums.first().title;
}

QString AppController::currentTrackAlbumId() const {
  const Track track = m_playbackController->currentTrack();
  return track.albums.isEmpty() ? QString() : track.albums.first().id;
}

void AppController::copyTrack(const QString &title, const QString &artist) {
  if (title.trimmed().isEmpty()) {
    return;
  }

  QString text;

  if (artist.trimmed().isEmpty()) {
    text = title.trimmed();
  } else {
    text = artist.trimmed() + " - " + title.trimmed();
  }

  QGuiApplication::clipboard()->setText(text);
  emit statusChanged(QStringLiteral("Скопировано: %1").arg(text));
}

QString AppController::currentTrackCoverUri() const {
  return m_playbackController->currentTrack().coverUri;
}

qint64 AppController::position() const {
  return m_playerService->position();
}

qint64 AppController::duration() const {
  return m_playerService->duration();
}

PlaybackController::PlaybackState AppController::playbackState() const {
  return m_playbackController->state();
}

int AppController::repeatMode() const {
  return static_cast<int>(m_playbackController->repeatMode());
}

bool AppController::shuffleEnabled() const {
  return m_playbackController->shuffleEnabled();
}

float AppController::volume() const {
  return m_playerService->volume();
}

void AppController::setVolume(float volume) {
  m_playerService->setVolume(volume);
}

void AppController::loadGenres() {
  m_genreController->loadGenres();
}