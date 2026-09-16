#pragma once

#include <QColor>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include "AlbumController.h"
#include "ArtistController.h"
#include "ChartController.h"
#include "GenreController.h"
#include "LibraryController.h"
#include "PersonalController.h"
#include "SearchController.h"
#include "../Yandex/Catalog/SearchAlbumsModel.h"
#include "../Yandex/Catalog/SearchArtistsModel.h"
#include "../Yandex/Catalog/SearchPlaylistsModel.h"
#include "../Yandex/Catalog/SimilarTracksModel.h"
#include "../Models/Track.h"
#include "../Playback/PlaybackController.h"
#include "../Yandex/Personal/NewPlaylistsService.h"

class AccountService;
class AlbumService;
class ArtistService;
class ChartService;
class GenreService;
class LikesService;
class PersonalLanding;
class PlayerAccentService;
class PlayerService;
class PlaylistService;
class QueueService;
class RecentListeningService;
class SearchService;
class StationService;
class TrackService;
class YandexAuth;
class YandexPersonal;

class AppController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool searching READ isSearching NOTIFY searchingChanged)
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)

    Q_PROPERTY(bool loadingMyWave READ isLoadingMyWave NOTIFY loadingMyWaveChanged)
    Q_PROPERTY(bool loadingMoreMyWave READ isLoadingMoreMyWave NOTIFY loadingMoreMyWaveChanged)
    Q_PROPERTY(bool loadingRecommendations READ isLoadingRecommendations NOTIFY loadingRecommendationsChanged)

    Q_PROPERTY(bool loadingLibraryPlaylists READ isLoadingLibraryPlaylists NOTIFY loadingLibraryPlaylistsChanged)
    Q_PROPERTY(bool loadingLikedTracks READ isLoadingLikedTracks NOTIFY loadingLikedTracksChanged)
    Q_PROPERTY(bool loadingLikedAlbums READ isLoadingLikedAlbums NOTIFY loadingLikedAlbumsChanged)
    Q_PROPERTY(bool loadingLikedArtists READ isLoadingLikedArtists NOTIFY loadingLikedArtistsChanged)
    Q_PROPERTY(bool loadingPlaylist READ isLoadingPlaylist NOTIFY loadingPlaylistChanged)
    Q_PROPERTY(bool loadingAlbum READ isLoadingAlbum NOTIFY loadingAlbumChanged)
    Q_PROPERTY(bool loadingArtist READ isLoadingArtist NOTIFY loadingArtistChanged)

    Q_PROPERTY(SearchModel *searchModel READ searchModel CONSTANT)
    Q_PROPERTY(SearchArtistsModel *searchArtistsModel READ searchArtistsModel CONSTANT)
    Q_PROPERTY(SearchAlbumsModel *searchAlbumsModel READ searchAlbumsModel CONSTANT)
    Q_PROPERTY(SearchPlaylistsModel *searchPlaylistsModel READ searchPlaylistsModel CONSTANT)

    Q_PROPERTY(SimilarTracksModel *similarTracksModel READ similarTracksModel CONSTANT)
    Q_PROPERTY(MyWaveModel *myWaveModel READ myWaveModel CONSTANT)
    Q_PROPERTY(PersonalPlaylistsModel *personalPlaylistsModel READ personalPlaylistsModel CONSTANT)
    Q_PROPERTY(LibraryPlaylistsModel *libraryPlaylistsModel READ libraryPlaylistsModel CONSTANT)
    Q_PROPERTY(LikedTracksModel *likedTracksModel READ likedTracksModel CONSTANT)
    Q_PROPERTY(LikedAlbumsModel *likedAlbumsModel READ likedAlbumsModel CONSTANT)
    Q_PROPERTY(LikedArtistsModel *likedArtistsModel READ likedArtistsModel CONSTANT)
    Q_PROPERTY(PlaylistModel *playlistModel READ playlistModel CONSTANT)
    Q_PROPERTY(RecentListeningModel *recentListeningModel READ recentListeningModel CONSTANT)

    Q_PROPERTY(AlbumController *albumController READ albumController CONSTANT)
    Q_PROPERTY(ArtistController *artistController READ artistController CONSTANT)
    Q_PROPERTY(ChartController *chartController READ chartController CONSTANT)
    Q_PROPERTY(GenreController *genreController READ genreController CONSTANT)

    Q_PROPERTY(QString currentPlaylistTitle READ currentPlaylistTitle NOTIFY currentPlaylistChanged)
    Q_PROPERTY(QString currentPlaylistCoverUri READ currentPlaylistCoverUri NOTIFY currentPlaylistChanged)
    Q_PROPERTY(int currentPlaylistTrackCount READ currentPlaylistTrackCount NOTIFY currentPlaylistChanged)
    Q_PROPERTY(int currentPlaylistKind READ currentPlaylistKind NOTIFY currentPlaylistChanged)

    Q_PROPERTY(
        QVariantList similarPlaylists
        READ similarPlaylists
        NOTIFY similarPlaylistsChanged)

    Q_PROPERTY(QString currentAlbumTitle READ currentAlbumTitle NOTIFY currentAlbumChanged)
    Q_PROPERTY(int currentAlbumTrackCount READ currentAlbumTrackCount NOTIFY currentAlbumChanged)
    Q_PROPERTY(QString currentAlbumCoverUri READ currentAlbumCoverUri NOTIFY currentAlbumChanged)
    Q_PROPERTY(bool currentAlbumLiked READ currentAlbumLiked NOTIFY currentAlbumChanged)

    Q_PROPERTY(QString currentArtistName READ currentArtistName NOTIFY currentArtistChanged)
    Q_PROPERTY(QString currentArtistCoverUri READ currentArtistCoverUri NOTIFY currentArtistChanged)
    Q_PROPERTY(QString currentArtistGenres READ currentArtistGenres NOTIFY currentArtistChanged)
    Q_PROPERTY(int currentArtistTrackCount READ currentArtistTrackCount NOTIFY currentArtistChanged)
    Q_PROPERTY(bool currentArtistLiked READ currentArtistLiked NOTIFY currentArtistChanged)

    Q_PROPERTY(QString currentTrackId READ currentTrackId NOTIFY currentTrackChanged)
    Q_PROPERTY(QString currentTrackTitle READ currentTrackTitle NOTIFY currentTrackChanged)
    Q_PROPERTY(QString currentTrackArtist READ currentTrackArtist NOTIFY currentTrackChanged)
    Q_PROPERTY(QString currentTrackArtistId READ currentTrackArtistId NOTIFY currentTrackChanged)
    Q_PROPERTY(QString currentTrackAlbumTitle READ currentTrackAlbumTitle NOTIFY currentTrackChanged)
    Q_PROPERTY(QString currentTrackAlbumId READ currentTrackAlbumId NOTIFY currentTrackChanged)
    Q_PROPERTY(QString currentTrackCoverUri READ currentTrackCoverUri NOTIFY currentTrackChanged)
    Q_PROPERTY(bool currentTrackLiked READ currentTrackLiked NOTIFY currentTrackChanged)

    /*
     * Lyrics
     */

    Q_PROPERTY(QString lyricsText READ lyricsText NOTIFY lyricsChanged)
    Q_PROPERTY(int lyricsLineCount READ lyricsLineCount NOTIFY lyricsChanged)
    Q_PROPERTY(bool lyricsAvailable READ lyricsAvailable NOTIFY lyricsChanged)

    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(PlaybackController::PlaybackState playbackState READ playbackState NOTIFY playbackStateChanged)
    Q_PROPERTY(int repeatMode READ repeatMode NOTIFY repeatModeChanged)
    Q_PROPERTY(bool shuffleEnabled READ shuffleEnabled NOTIFY shuffleChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)

    Q_PROPERTY(QString playbackSourceTitle READ playbackSourceTitle NOTIFY playbackSourceChanged)
    Q_PROPERTY(QString playbackSourceType READ playbackSourceType NOTIFY playbackSourceChanged)
    Q_PROPERTY(int queueCount READ queueCount NOTIFY queueChanged)
    Q_PROPERTY(int queueCurrentIndex READ queueCurrentIndex NOTIFY queueChanged)

    Q_PROPERTY(QColor playerAccent READ playerAccent NOTIFY playerAccentChanged)

    /*
     * Theme: сохраняется в QSettings.
     */

    Q_PROPERTY(bool darkTheme READ darkTheme WRITE setDarkTheme NOTIFY darkThemeChanged)

public:
    explicit AppController(
    YandexAuth *auth,
    AccountService *accountService,
    QObject *parent = nullptr);

    Q_INVOKABLE void testConnection();
    Q_INVOKABLE void testYandexApi();
    Q_INVOKABLE void testSearch(const QString &query);
    Q_INVOKABLE void search(const QString &query);

    Q_INVOKABLE void loadArtist(const QString &id);
    Q_INVOKABLE void loadAlbum(const QString &id);
    Q_INVOKABLE void playAlbum(const QString &id);
    Q_INVOKABLE void loadChart(const QString &chartType);
    Q_INVOKABLE void loadCharts();
    Q_INVOKABLE void loadGenres();

    Q_INVOKABLE void loadMyWave();
    Q_INVOKABLE void loadMoreMyWave();
    Q_INVOKABLE void loadRecommendations();

    Q_INVOKABLE void loadLibrary();
    Q_INVOKABLE void selectLibraryPlaylist(int index);
    Q_INVOKABLE void loadLikedTracks();
    Q_INVOKABLE void selectLikedTrack(int index);

    Q_INVOKABLE void loadLikedAlbums();
    Q_INVOKABLE void selectLikedAlbum(int index);

    Q_INVOKABLE void loadLikedArtists();
    Q_INVOKABLE void selectLikedArtist(int index);

    Q_INVOKABLE void selectSearchResult(int index);
    Q_INVOKABLE void selectSearchArtist(int index);
    Q_INVOKABLE void selectSearchAlbum(int index);
    Q_INVOKABLE void selectSearchPlaylist(int index);
    Q_INVOKABLE void selectSimilarTrack(int index);
    Q_INVOKABLE void loadMoreGenreStation();
    Q_INVOKABLE void selectGenreStationTrack(int index);
    Q_INVOKABLE void createPlaylist(const QString &title);
    Q_INVOKABLE void deleteCurrentPlaylist();
    Q_INVOKABLE void renameCurrentPlaylist(const QString &newTitle);
    Q_INVOKABLE void removeTrackFromPlaylist(int index);
    Q_INVOKABLE void addTrackToPlaylist(int kind, const QString &trackId, const QString &albumId);
    Q_INVOKABLE void selectMyWaveTrack(int index);
    Q_INVOKABLE void selectPersonalPlaylist(const QString &uid, int kind);
    Q_INVOKABLE void selectPlaylistTrack(int index);
    Q_INVOKABLE void selectRecentListening(int index);
    Q_INVOKABLE void selectAlbumTrack(int index);
    Q_INVOKABLE void selectArtistTrack(int index);
    Q_INVOKABLE void selectSimilarArtist(int index);

    Q_INVOKABLE void toggleLike(const QString &trackId,bool liked);
    Q_INVOKABLE void toggleAlbumLike();
    Q_INVOKABLE void toggleArtistLike();

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void cycleRepeat();
    Q_INVOKABLE void setRepeatMode(int mode);
    Q_INVOKABLE void toggleShuffle();
    Q_INVOKABLE void setShuffle(bool enabled);
    Q_INVOKABLE void seek(qint64 position);
    Q_INVOKABLE void setVolume(float volume);

    QString playbackSourceTitle() const;
    QString playbackSourceType() const;
    int queueCount() const;
    int queueCurrentIndex() const;
    Q_INVOKABLE QVariantMap queueTrackData(int index) const;

    SearchModel *searchModel() const;
    SearchArtistsModel *searchArtistsModel() const;
    SearchAlbumsModel *searchAlbumsModel() const;
    SearchPlaylistsModel *searchPlaylistsModel() const;
    SimilarTracksModel *similarTracksModel() const;
    MyWaveModel *myWaveModel() const;
    PersonalPlaylistsModel *personalPlaylistsModel() const;
    LibraryPlaylistsModel *libraryPlaylistsModel() const;
    LikedTracksModel *likedTracksModel() const;
    LikedAlbumsModel *likedAlbumsModel() const;
    LikedArtistsModel *likedArtistsModel() const;
    PlaylistModel *playlistModel() const;
    RecentListeningModel *recentListeningModel() const;

    AlbumController *albumController() const;
    ArtistController *artistController() const;
    ChartController *chartController() const;
    GenreController *genreController() const;

    bool isSearching() const;
    bool isPlaying() const;
    bool isLoadingMyWave() const;
    bool isLoadingMoreMyWave() const;
    bool isLoadingRecommendations() const;
    bool isLoadingLibraryPlaylists() const;
    bool isLoadingLikedTracks() const;

    bool isLoadingLikedAlbums() const;

    bool isLoadingLikedArtists() const;
    bool isLoadingPlaylist() const;
    bool isLoadingAlbum() const;
    bool isLoadingArtist() const;
    bool currentTrackLiked() const;

    QString currentPlaylistTitle() const;
    QString currentPlaylistCoverUri() const;
    int currentPlaylistTrackCount() const;
    int currentPlaylistKind() const;
    QVariantList similarPlaylists() const;

    QString currentAlbumTitle() const;
    int currentAlbumTrackCount() const;
    QString currentAlbumCoverUri() const;
    bool currentAlbumLiked() const;

    QString currentArtistName() const;
    QString currentArtistCoverUri() const;
    QString currentArtistGenres() const;
    int currentArtistTrackCount() const;
    bool currentArtistLiked() const;

    QString currentTrackId() const;
    QString currentTrackTitle() const;
    QString currentTrackArtist() const;
    QString currentTrackArtistId() const;
    QString currentTrackAlbumTitle() const;
    QString currentTrackAlbumId() const;
    QString currentTrackCoverUri() const;

    /*
     * Lyrics
     */

    Q_INVOKABLE void loadLyrics();

    QString lyricsText() const;

    int lyricsLineCount() const;

    bool lyricsAvailable() const;

    qint64 position() const;
    qint64 duration() const;
    PlaybackController::PlaybackState playbackState() const;
    int repeatMode() const;
    bool shuffleEnabled() const;
    float volume() const;

    QColor playerAccent() const;

    /*
     * Theme
     */

    bool darkTheme() const;

    Q_INVOKABLE void setDarkTheme(
        bool dark);

    signals:
        void statusChanged(const QString &message);
    void artistPageRequested(const QString &artistId);
    void albumPageRequested(const QString &albumId);
    void playlistPageRequested();
    void searchPageRequested(const QString &query);

    void searchingChanged();
    void playingChanged();

    void loadingMyWaveChanged();
    void loadingMoreMyWaveChanged();
    void loadingRecommendationsChanged();
    void loadingLibraryPlaylistsChanged();
    void loadingLikedTracksChanged();
    void loadingLikedAlbumsChanged();
    void loadingLikedArtistsChanged();
    void loadingPlaylistChanged();
    void loadingAlbumChanged();
    void loadingArtistChanged();

    void recommendationsLoaded();

    void currentPlaylistChanged();
    void similarPlaylistsChanged();
    void currentAlbumChanged();
    void currentArtistChanged();
    void currentTrackChanged();

    void positionChanged();
    void durationChanged();
    void playbackStateChanged();
    void repeatModeChanged();
    void shuffleChanged();
    void volumeChanged();

    void playbackSourceChanged();
    void queueChanged();
    void playerAccentChanged();

    /*
     * Theme
     */

    void darkThemeChanged();

    /*
     * Lyrics
     */

    void lyricsChanged();

private:
    void connectAccount();
    void connectSearch();
    void connectLibrary();
    void connectAlbum();
    void connectPersonal();
    void connectArtist();
    void connectChart();
    void connectGenre();
    void connectPlayback();
    void connectPlayer();

    YandexAuth *m_auth;
    AccountService *m_accountService;
    SearchService *m_searchService;
    TrackService *m_trackService;
    YandexPersonal *m_yandexPersonal;
    PersonalLanding *m_personalLanding;
    RecentListeningService *m_recentListeningService;
    PlaylistService *m_playlistService;
    NewPlaylistsService *m_newPlaylistsService;
    LikesService *m_likesService;
    AlbumService *m_albumService;
    ArtistService *m_artistService;
    ChartService *m_chartService;
    GenreService *m_genreService;
    StationService *m_stationService;
    PlayerService *m_playerService;
    QueueService *m_queueService;
    PlayerAccentService *m_playerAccentService;

    PlaybackController *m_playbackController;
    LibraryController *m_libraryController;
    PersonalController *m_personalController;
    SearchController *m_searchController;
    SimilarTracksModel *m_similarTracksModel;
    AlbumController *m_albumController;
    ArtistController *m_artistController;
    ChartController *m_chartController;
    GenreController *m_genreController;

    QString m_accountUid;
    bool m_playAlbumAfterLoad = false;

    /*
     * Theme
     */

    bool m_darkTheme = false;

    /*
     * Lyrics
     */

    TrackSupplementary m_supplementary;
};