#pragma once

#include <QObject>
#include <QString>

#include "LibraryController.h"
#include "PersonalController.h"
#include "SearchController.h"

#include "../Playback/PlaybackController.h"

class AccountService;
class AlbumService;
class ArtistService;
class PersonalLanding;
class PlayerService;
class PlaylistService;
class QueueService;
class SearchService;
class TrackService;
class YandexAuth;
class YandexPersonal;

class AppController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool searching
        READ isSearching
        NOTIFY searchingChanged)

    Q_PROPERTY(
        bool playing
        READ isPlaying
        NOTIFY playingChanged)

    Q_PROPERTY(
        bool loadingMyWave
        READ isLoadingMyWave
        NOTIFY loadingMyWaveChanged)

    Q_PROPERTY(
        bool loadingMoreMyWave
        READ isLoadingMoreMyWave
        NOTIFY loadingMoreMyWaveChanged)

    Q_PROPERTY(
        bool loadingRecommendations
        READ isLoadingRecommendations
        NOTIFY loadingRecommendationsChanged)

    Q_PROPERTY(
        bool loadingPlaylist
        READ isLoadingPlaylist
        NOTIFY loadingPlaylistChanged)

    Q_PROPERTY(
        bool loadingAlbum
        READ isLoadingAlbum
        NOTIFY loadingAlbumChanged)

    Q_PROPERTY(
        bool loadingArtist
        READ isLoadingArtist
        NOTIFY loadingArtistChanged)

    Q_PROPERTY(
        SearchModel *searchModel
        READ searchModel
        CONSTANT)

    Q_PROPERTY(
        MyWaveModel *myWaveModel
        READ myWaveModel
        CONSTANT)

    Q_PROPERTY(
        PersonalPlaylistsModel *personalPlaylistsModel
        READ personalPlaylistsModel
        CONSTANT)

    Q_PROPERTY(
        PlaylistModel *playlistModel
        READ playlistModel
        CONSTANT)

    Q_PROPERTY(
        AlbumModel *albumModel
        READ albumModel
        CONSTANT)

    Q_PROPERTY(
        ArtistModel *artistModel
        READ artistModel
        CONSTANT)

    Q_PROPERTY(
        RecentListeningModel *recentListeningModel
        READ recentListeningModel
        CONSTANT)

    Q_PROPERTY(
        QString currentPlaylistTitle
        READ currentPlaylistTitle
        NOTIFY currentPlaylistChanged)

    Q_PROPERTY(
        int currentPlaylistTrackCount
        READ currentPlaylistTrackCount
        NOTIFY currentPlaylistChanged)

    Q_PROPERTY(
        QString currentAlbumTitle
        READ currentAlbumTitle
        NOTIFY currentAlbumChanged)

    Q_PROPERTY(
        int currentAlbumTrackCount
        READ currentAlbumTrackCount
        NOTIFY currentAlbumChanged)

    Q_PROPERTY(
        QString currentAlbumCoverUri
        READ currentAlbumCoverUri
        NOTIFY currentAlbumChanged)

    Q_PROPERTY(
        QString currentArtistName
        READ currentArtistName
        NOTIFY currentArtistChanged)

    Q_PROPERTY(
        QString currentArtistCoverUri
        READ currentArtistCoverUri
        NOTIFY currentArtistChanged)

    Q_PROPERTY(
        QString currentArtistGenres
        READ currentArtistGenres
        NOTIFY currentArtistChanged)

    Q_PROPERTY(
        int currentArtistTrackCount
        READ currentArtistTrackCount
        NOTIFY currentArtistChanged)

    Q_PROPERTY(
        QString currentTrackTitle
        READ currentTrackTitle
        NOTIFY currentTrackChanged)

    Q_PROPERTY(
        QString currentTrackArtist
        READ currentTrackArtist
        NOTIFY currentTrackChanged)

    Q_PROPERTY(
        QString currentTrackCoverUri
        READ currentTrackCoverUri
        NOTIFY currentTrackChanged)

    Q_PROPERTY(
        qint64 position
        READ position
        NOTIFY positionChanged)

    Q_PROPERTY(
        qint64 duration
        READ duration
        NOTIFY durationChanged)

    Q_PROPERTY(
        PlaybackController::PlaybackState playbackState
        READ playbackState
        NOTIFY playbackStateChanged)

    Q_PROPERTY(
        int repeatMode
        READ repeatMode
        NOTIFY repeatModeChanged)

    Q_PROPERTY(
        bool shuffleEnabled
        READ shuffleEnabled
        NOTIFY shuffleChanged)

public:
    explicit AppController(
        QObject *parent = nullptr);

    Q_INVOKABLE void testConnection();

    Q_INVOKABLE void testYandexApi();

    Q_INVOKABLE void testSearch(
        const QString &query);

    Q_INVOKABLE void loadArtist(
        const QString &id);

    Q_INVOKABLE void loadMyWave();

    Q_INVOKABLE void loadMoreMyWave();

    Q_INVOKABLE void loadRecommendations();

    Q_INVOKABLE void selectSearchResult(
        int index);

    Q_INVOKABLE void selectMyWaveTrack(
        int index);

    Q_INVOKABLE void selectPersonalPlaylist(
        int index);

    Q_INVOKABLE void selectPlaylistTrack(
        int index);

    Q_INVOKABLE void selectRecentListening(
        int index);

    Q_INVOKABLE void selectAlbumTrack(
        int index);

    Q_INVOKABLE void selectArtistTrack(
        int index);

    Q_INVOKABLE void play();

    Q_INVOKABLE void pause();

    Q_INVOKABLE void stop();

    Q_INVOKABLE void next();

    Q_INVOKABLE void previous();

    Q_INVOKABLE void cycleRepeat();

    Q_INVOKABLE void setRepeatMode(
        int mode);

    Q_INVOKABLE void toggleShuffle();

    Q_INVOKABLE void setShuffle(
        bool enabled);

    Q_INVOKABLE void seek(
        qint64 position);

    SearchModel *searchModel() const;

    MyWaveModel *myWaveModel() const;

    PersonalPlaylistsModel *
    personalPlaylistsModel() const;

    PlaylistModel *playlistModel() const;

    AlbumModel *albumModel() const;

    ArtistModel *artistModel() const;

    RecentListeningModel *
    recentListeningModel() const;

    bool isSearching() const;

    bool isPlaying() const;

    bool isLoadingMyWave() const;

    bool isLoadingMoreMyWave() const;

    bool isLoadingRecommendations() const;

    bool isLoadingPlaylist() const;

    bool isLoadingAlbum() const;

    bool isLoadingArtist() const;

    QString currentPlaylistTitle() const;

    int currentPlaylistTrackCount() const;

    QString currentAlbumTitle() const;

    int currentAlbumTrackCount() const;

    QString currentAlbumCoverUri() const;

    QString currentArtistName() const;

    QString currentArtistCoverUri() const;

    QString currentArtistGenres() const;

    int currentArtistTrackCount() const;

    QString currentTrackTitle() const;

    QString currentTrackArtist() const;

    QString currentTrackCoverUri() const;

    qint64 position() const;

    qint64 duration() const;

    PlaybackController::PlaybackState
    playbackState() const;

    int repeatMode() const;

    bool shuffleEnabled() const;

signals:
    void statusChanged(
        const QString &message);

    void searchingChanged();

    void playingChanged();

    void loadingMyWaveChanged();

    void loadingMoreMyWaveChanged();

    void loadingRecommendationsChanged();

    void loadingPlaylistChanged();

    void loadingAlbumChanged();

    void loadingArtistChanged();

    void recommendationsLoaded();

    void currentPlaylistChanged();

    void currentAlbumChanged();

    void currentArtistChanged();

    void currentTrackChanged();

    void positionChanged();

    void durationChanged();

    void playbackStateChanged();

    void repeatModeChanged();

    void shuffleChanged();

private:
    YandexAuth *m_auth = nullptr;

    AccountService *m_accountService = nullptr;

    SearchService *m_searchService = nullptr;

    TrackService *m_trackService = nullptr;

    YandexPersonal *m_yandexPersonal = nullptr;

    PersonalLanding *m_personalLanding = nullptr;

    PlaylistService *m_playlistService = nullptr;

    AlbumService *m_albumService = nullptr;

    ArtistService *m_artistService = nullptr;

    PlayerService *m_playerService = nullptr;

    QueueService *m_queueService = nullptr;

    PlaybackController *
        m_playbackController = nullptr;

    LibraryController *
        m_libraryController = nullptr;

    PersonalController *
        m_personalController = nullptr;

    SearchController *
        m_searchController = nullptr;
};