#include "AppController.h"

#include "../Player/PlayerService.h"
#include "../Yandex/Account/AccountService.h"
#include "../Yandex/Auth/YandexAuth.h"
#include "../Yandex/Catalog/AlbumService.h"
#include "../Yandex/Catalog/ArtistService.h"
#include "../Yandex/Catalog/SearchService.h"
#include "../Yandex/Catalog/TrackService.h"
#include "../Yandex/Personal/MyWaveModel.h"
#include "../Yandex/Personal/PersonalLanding.h"
#include "../Yandex/Personal/PersonalPlaylistsModel.h"
#include "../Yandex/Personal/PlaylistModel.h"
#include "../Yandex/Personal/PlaylistService.h"
#include "../Yandex/Personal/RecentListeningModel.h"
#include "../Yandex/Personal/YandexPersonal.h"

#include <QDebug>
#include <QStringList>

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
    , m_searchModel(
          new SearchModel(
              this))
    , m_myWaveModel(
          new MyWaveModel(
              this))
    , m_personalPlaylistsModel(
          new PersonalPlaylistsModel(
              this))
    , m_playlistModel(
          new PlaylistModel(
              this))
    , m_albumModel(
          new AlbumModel(
              this))
    , m_artistModel(
          new ArtistModel(
              this))
    , m_recentListeningModel(
          new RecentListeningModel(
              this))
    , m_playerService(
          new PlayerService(
              this))
    , m_playbackController(
          new PlaybackController(
              m_trackService,
              m_playerService,
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

            const QString message =
                QString(
                    "Выполнен вход: %1 (uid: %2)")
                    .arg(
                        account.displayName)
                    .arg(
                        account.uid);

            qDebug()
                << message;

            emit statusChanged(
                message);
        });

    connect(
        m_accountService,
        &AccountService::errorOccurred,
        this,
        [this](
            const QString &message) {

            qDebug()
                << "Account service error:"
                << message;

            emit statusChanged(
                message);
        });

    /*
     * Search
     */

    connect(
        m_searchService,
        &SearchService::searchStarted,
        this,
        [this]() {

            if (m_searching) {
                return;
            }

            m_searching =
                true;

            emit searchingChanged();

            emit statusChanged(
                "Поиск...");
        });

    connect(
        m_searchService,
        &SearchService::searchReceived,
        this,
        [this](
            const SearchResults &results) {

            m_searching =
                false;

            emit searchingChanged();

            m_searchModel->setResults(
                results);

            qDebug()
                << "Search results:"
                << results.tracks.size()
                << "/"
                << results.total;

            emit statusChanged(
                QString(
                    "Найдено результатов: %1")
                    .arg(
                        results.total));
        });

    connect(
        m_searchService,
        &SearchService::errorOccurred,
        this,
        [this](
            const QString &message) {

            m_searching =
                false;

            emit searchingChanged();

            m_searchModel->clear();

            qDebug()
                << "Search service error:"
                << message;

            emit statusChanged(
                message);
        });

    /*
     * My Wave
     */

    connect(
        m_yandexPersonal,
        &YandexPersonal::myWaveReceived,
        this,
        [this](
            const QList<Track> &tracks,
            const QString &batchId) {

            const QString trimmedBatchId =
                batchId.trimmed();

            for (const Track &track :
                 tracks) {

                if (
                    !track.id.isEmpty() &&
                    !trimmedBatchId.isEmpty()
                ) {

                    m_myWaveTrackBatches.insert(
                        track.id,
                        trimmedBatchId);
                }
            }

            const bool firstLoad =
                !m_loadingMoreMyWave;

            if (m_loadingMoreMyWave) {

                m_loadingMoreMyWave =
                    false;

                emit loadingMoreMyWaveChanged();
            }

            if (firstLoad) {

                m_loadingMyWave =
                    false;

                emit loadingMyWaveChanged();

                m_myWaveModel->setTracks(
                    tracks);

                qDebug()
                    << "My Wave tracks:"
                    << tracks.size()
                    << "batch:"
                    << trimmedBatchId;

                emit statusChanged(
                    QString(
                        "Моя волна: %1 треков")
                        .arg(
                            m_myWaveModel->count()));

            } else {

                m_myWaveModel->appendTracks(
                    tracks);

                qDebug()
                    << "More My Wave tracks:"
                    << tracks.size()
                    << "batch:"
                    << trimmedBatchId;

                emit statusChanged(
                    QString(
                        "Загружено ещё %1 треков")
                        .arg(
                            tracks.size()));
            }
        });

    connect(
        m_yandexPersonal,
        &YandexPersonal::errorOccurred,
        this,
        [this](
            const QString &message) {

            if (m_loadingMyWave) {

                m_loadingMyWave =
                    false;

                emit loadingMyWaveChanged();
            }

            if (m_loadingMoreMyWave) {

                m_loadingMoreMyWave =
                    false;

                emit loadingMoreMyWaveChanged();
            }

            qDebug()
                << "My Wave error:"
                << message;

            emit statusChanged(
                QString(
                    "Ошибка моей волны: %1")
                    .arg(
                        message));
        });

    connect(
        m_yandexPersonal,
        &YandexPersonal::feedbackSent,
        this,
        [this](
            const QString &event) {

            qDebug()
                << "My Wave feedback sent:"
                << event;
        });

    connect(
        m_yandexPersonal,
        &YandexPersonal::feedbackError,
        this,
        [this](
            const QString &message) {

            qDebug()
                << "My Wave feedback error:"
                << message;
        });

    /*
     * Personal Landing
     */

    connect(
        m_personalLanding,
        &PersonalLanding::personalPlaylistsReceived,
        this,
        [this](
            const QList<PersonalPlaylist> &playlists) {

            m_personalPlaylistsModel
                ->setPlaylists(
                    playlists);

            qDebug()
                << "Персональных плейлистов:"
                << playlists.size();

            for (
                const PersonalPlaylist &playlist :
                playlists) {

                qDebug()
                    << "Плейлист:"
                    << playlist.title
                    << "| uid:"
                    << playlist.uid
                    << "| kind:"
                    << playlist.kind
                    << "| tracks:"
                    << playlist.trackCount;
            }
        });

    connect(
        m_personalLanding,
        &PersonalLanding::recentListeningReceived,
        this,
        [this](
            const QList<RecentListeningItem> &items) {

            m_recentListeningModel
                ->setItems(
                    items);

            qDebug()
                << "Недавно слушали:"
                << items.size();

            for (
                const RecentListeningItem &item :
                items) {

                qDebug()
                    << "Недавно:"
                    << item.title
                    << "| type:"
                    << item.type
                    << "| context:"
                    << item.context
                    << "| contextItem:"
                    << item.contextItem
                    << "| uid:"
                    << item.uid
                    << "| kind:"
                    << item.kind
                    << "| cover:"
                    << item.coverUri;
            }
        });

    connect(
        m_personalLanding,
        &PersonalLanding::loaded,
        this,
        [this](
            const QList<PersonalLandingSection> &sections) {

            m_loadingRecommendations =
                false;

            emit loadingRecommendationsChanged();

            qDebug()
                << "Рекомендации: секций"
                << sections.size();

            for (
                const PersonalLandingSection &section :
                sections) {

                qDebug()
                    << "Секция:"
                    << section.title
                    << "| type:"
                    << section.type
                    << "| items:"
                    << section.items.size();
            }

            emit recommendationsLoaded();

            emit statusChanged(
                QString(
                    "Загружено блоков рекомендаций: %1")
                    .arg(
                        sections.size()));
        });

    connect(
        m_personalLanding,
        &PersonalLanding::errorOccurred,
        this,
        [this](
            const QString &message) {

            m_loadingRecommendations =
                false;

            emit loadingRecommendationsChanged();

            qDebug()
                << "Recommendations error:"
                << message;

            emit statusChanged(
                QString(
                    "Ошибка загрузки рекомендаций: %1")
                    .arg(
                        message));
        });

    /*
     * Playlist
     */

    connect(
        m_playlistService,
        &PlaylistService::playlistReceived,
        this,
        [this](
            const Playlist &playlist) {

            m_loadingPlaylist =
                false;

            emit loadingPlaylistChanged();

            m_playlistModel->setPlaylist(
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

    /*
     * Album
     */

    connect(
        m_albumService,
        &AlbumService::albumReceived,
        this,
        [this](
            const AlbumDetails &album) {

            m_loadingAlbum =
                false;

            emit loadingAlbumChanged();

            m_albumModel->setAlbum(
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

    /*
     * Artist
     */

    connect(
        m_artistService,
        &ArtistService::artistReceived,
        this,
        [this](
            const ArtistDetails &artist) {

            m_loadingArtist =
                false;

            emit loadingArtistChanged();

            m_artistModel->setArtist(
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

    /*
     * PlaybackController
     */

    connect(
        m_playbackController,
        &PlaybackController::currentTrackChanged,
        this,
        [this]() {

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

            emit currentTrackChanged();

            emit statusChanged(
                message);
        });

    connect(
        m_playbackController,
        &PlaybackController::stateChanged,
        this,
        [this]() {

            emit playbackStateChanged();
        });

    connect(
        m_playbackController,
        &PlaybackController::playbackError,
        this,
        [this](
            const QString &message) {

            qDebug()
                << "Playback error:"
                << message;

            emit statusChanged(
                message);
        });

    /*
     * Player
     */

    connect(
        m_playerService,
        &PlayerService::playingChanged,
        this,
        [this]() {

            emit playingChanged();

            if (m_playerService->isPlaying()) {

                emit statusChanged(
                    "Воспроизведение");
            }
        });

    connect(
        m_playerService,
        &PlayerService::playbackStarted,
        this,
        [this]() {

            if (
                m_currentMyWaveTrackId.isEmpty()
            ) {
                return;
            }

            if (m_myWaveTrackStarted) {
                return;
            }

            if (
                batchIdForTrack(
                    m_currentMyWaveTrackId)
                    .isEmpty()
            ) {
                return;
            }

            m_myWaveTrackStarted =
                true;

            sendMyWaveFeedback(
                "trackStarted",
                m_currentMyWaveTrackId);
        });

    connect(
        m_playerService,
        &PlayerService::playbackFinished,
        this,
        [this]() {

            if (
                m_currentMyWaveTrackId.isEmpty() ||
                !m_myWaveTrackStarted
            ) {
                return;
            }

            const qint64 playedSeconds =
                m_playerService
                    ->position() /
                1000;

            sendMyWaveFeedback(
                "trackFinished",
                m_currentMyWaveTrackId,
                playedSeconds);

            m_currentMyWaveTrackId.clear();

            m_myWaveTrackStarted =
                false;
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

            qDebug()
                << "Player error:"
                << message;

            emit statusChanged(
                QString(
                    "Ошибка воспроизведения: %1")
                    .arg(
                        message));
        });
}

void AppController::testConnection()
{
    qDebug()
        << "AppController is working";

    emit statusChanged(
        "Приложение работает");
}

void AppController::testYandexApi()
{
    m_accountService->loadAccount();

    emit statusChanged(
        "Проверка аккаунта Яндекс Музыки...");
}

void AppController::testSearch(
    const QString &query)
{
    m_searchService->search(
        query);
}

void AppController::loadArtist(
    const QString &id)
{
    const QString artistId =
        id.trimmed();

    if (artistId.isEmpty()) {

        emit statusChanged(
            "ID исполнителя не указан");

        return;
    }

    m_loadingArtist =
        true;

    emit loadingArtistChanged();

    m_artistModel->clear();

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

    m_artistService->loadArtist(
        artistId);
}

void AppController::loadMyWave()
{
    if (
        m_loadingMyWave ||
        m_loadingMoreMyWave
    ) {
        return;
    }

    m_loadingMyWave =
        true;

    emit loadingMyWaveChanged();

    m_myWaveTrackBatches.clear();

    emit statusChanged(
        "Загрузка моей волны...");

    m_yandexPersonal->loadMyWave();
}

void AppController::loadMoreMyWave()
{
    if (
        m_loadingMyWave ||
        m_loadingMoreMyWave
    ) {
        return;
    }

    const Track lastTrack =
        m_myWaveModel->lastTrack();

    if (lastTrack.id.isEmpty()) {
        return;
    }

    m_loadingMoreMyWave =
        true;

    emit loadingMoreMyWaveChanged();

    emit statusChanged(
        "Загрузка следующей части моей волны...");

    m_yandexPersonal
        ->loadMoreMyWave(
            lastTrack.id);
}

void AppController::loadRecommendations()
{
    if (m_loadingRecommendations) {
        return;
    }

    m_loadingRecommendations =
        true;

    emit loadingRecommendationsChanged();

    m_personalPlaylistsModel
        ->clear();

    m_recentListeningModel
        ->clear();

    m_albumModel
        ->clear();

    m_artistModel
        ->clear();

    m_currentAlbumTitle.clear();

    m_currentAlbumTrackCount =
        0;

    m_currentAlbumCoverUri.clear();

    m_currentArtistName.clear();

    m_currentArtistCoverUri.clear();

    m_currentArtistGenres.clear();

    m_currentArtistTrackCount =
        0;

    emit currentAlbumChanged();

    emit currentArtistChanged();

    emit statusChanged(
        "Загрузка рекомендаций...");

    m_personalLanding->load();
}

void AppController::selectSearchResult(
    int index)
{
    const Track track =
        m_searchModel
            ->trackAt(index);

    if (track.id.isEmpty()) {

        emit statusChanged(
            "Некорректный результат поиска");

        return;
    }

    if (
        !m_currentMyWaveTrackId.isEmpty() &&
        m_myWaveTrackStarted
    ) {

        const qint64 playedSeconds =
            m_playerService
                ->position() /
            1000;

        sendMyWaveFeedback(
            "skip",
            m_currentMyWaveTrackId,
            playedSeconds);
    }

    m_currentMyWaveTrackId.clear();

    m_myWaveTrackStarted =
        false;

    m_playbackController
        ->playTrack(
            track);
}

void AppController::selectMyWaveTrack(
    int index)
{
    const Track track =
        m_myWaveModel
            ->trackAt(index);

    if (track.id.isEmpty()) {

        emit statusChanged(
            "Некорректный трек моей волны");

        return;
    }

    if (
        !m_currentMyWaveTrackId.isEmpty() &&
        m_myWaveTrackStarted &&
        m_currentMyWaveTrackId != track.id
    ) {

        const qint64 playedSeconds =
            m_playerService
                ->position() /
            1000;

        sendMyWaveFeedback(
            "skip",
            m_currentMyWaveTrackId,
            playedSeconds);
    }

    m_currentMyWaveTrackId =
        track.id;

    m_myWaveTrackStarted =
        false;

    m_playbackController
        ->playTrack(
            track);
}

void AppController::selectPersonalPlaylist(
    int index)
{
    const PersonalPlaylist playlist =
        m_personalPlaylistsModel
            ->playlistAt(
                index);

    if (
        playlist.uid.isEmpty() ||
        playlist.kind <= 0
    ) {

        emit statusChanged(
            "Некорректный плейлист");

        return;
    }

    m_loadingPlaylist =
        true;

    emit loadingPlaylistChanged();

    m_playlistModel->clear();

    m_currentPlaylistTitle.clear();

    m_currentPlaylistTrackCount =
        0;

    emit currentPlaylistChanged();

    qDebug()
        << "Загрузка плейлиста:"
        << playlist.title
        << "| uid:"
        << playlist.uid
        << "| kind:"
        << playlist.kind;

    emit statusChanged(
        QString(
            "Загрузка плейлиста: %1")
            .arg(
                playlist.title));

    m_playlistService
        ->loadPlaylist(
            playlist.uid,
            playlist.kind);
}

void AppController::selectPlaylistTrack(
    int index)
{
    const Track track =
        m_playlistModel
            ->trackAt(
                index);

    if (track.id.isEmpty()) {

        emit statusChanged(
            "Некорректный трек плейлиста");

        return;
    }

    if (
        !m_currentMyWaveTrackId.isEmpty() &&
        m_myWaveTrackStarted
    ) {

        const qint64 playedSeconds =
            m_playerService
                ->position() /
            1000;

        sendMyWaveFeedback(
            "skip",
            m_currentMyWaveTrackId,
            playedSeconds);
    }

    m_currentMyWaveTrackId.clear();

    m_myWaveTrackStarted =
        false;

    m_playbackController
        ->playTrack(
            track);
}

void AppController::selectRecentListening(
    int index)
{
    const RecentListeningItem item =
        m_recentListeningModel
            ->itemAt(
                index);

    if (item.id.isEmpty()) {

        emit statusChanged(
            "Некорректный элемент истории");

        return;
    }

    qDebug()
        << "Выбран элемент истории:"
        << item.title
        << "| type:"
        << item.type
        << "| context:"
        << item.context
        << "| contextItem:"
        << item.contextItem
        << "| uid:"
        << item.uid
        << "| kind:"
        << item.kind;

    /*
     * Playlist
     */

    if (item.context ==
        "playlist") {

        QString uid =
            item.uid;

        int kind =
            item.kind;

        if (
            (uid.isEmpty() ||
             kind <= 0) &&
            item.contextItem.contains(":")
        ) {

            const QStringList parts =
                item.contextItem.split(":");

            if (parts.size() == 2) {

                if (uid.isEmpty()) {
                    uid =
                        parts.at(0);
                }

                if (kind <= 0) {
                    kind =
                        parts.at(1)
                            .toInt();
                }
            }
        }

        if (
            uid.isEmpty() ||
            kind <= 0
        ) {

            emit statusChanged(
                "Не удалось определить плейлист");

            return;
        }

        m_loadingPlaylist =
            true;

        emit loadingPlaylistChanged();

        m_playlistModel->clear();

        m_currentPlaylistTitle.clear();

        m_currentPlaylistTrackCount =
            0;

        emit currentPlaylistChanged();

        qDebug()
            << "Загрузка плейлиста из истории:"
            << item.title
            << "| uid:"
            << uid
            << "| kind:"
            << kind;

        emit statusChanged(
            QString(
                "Загрузка плейлиста: %1")
                .arg(
                    item.title));

        m_playlistService
            ->loadPlaylist(
                uid,
                kind);

        return;
    }

    /*
     * Album
     */

    if (item.context ==
        "album") {

        const QString albumId =
            item.uid.isEmpty()
                ? item.contextItem
                : item.uid;

        if (albumId.isEmpty()) {

            emit statusChanged(
                "Не удалось определить альбом");

            return;
        }

        m_loadingAlbum =
            true;

        emit loadingAlbumChanged();

        m_albumModel->clear();

        m_currentAlbumTitle.clear();

        m_currentAlbumTrackCount =
            0;

        m_currentAlbumCoverUri.clear();

        emit currentAlbumChanged();

        qDebug()
            << "Загрузка альбома из истории:"
            << item.title
            << "| id:"
            << albumId;

        emit statusChanged(
            QString(
                "Загрузка альбома: %1")
                .arg(
                    item.title));

        m_albumService
            ->loadAlbum(
                albumId);

        return;
    }

    /*
     * Artist
     */

    if (item.context ==
        "artist") {

        const QString artistId =
            item.uid.isEmpty()
                ? item.contextItem
                : item.uid;

        if (artistId.isEmpty()) {

            emit statusChanged(
                "Не удалось определить исполнителя");

            return;
        }

        loadArtist(
            artistId);

        return;
    }

    emit statusChanged(
        QString(
            "Выбран: %1")
            .arg(
                item.title));
}

void AppController::selectAlbumTrack(
    int index)
{
    const Track track =
        m_albumModel
            ->trackAt(
                index);

    if (track.id.isEmpty()) {

        emit statusChanged(
            "Некорректный трек альбома");

        return;
    }

    if (
        !m_currentMyWaveTrackId.isEmpty() &&
        m_myWaveTrackStarted
    ) {

        const qint64 playedSeconds =
            m_playerService
                ->position() /
            1000;

        sendMyWaveFeedback(
            "skip",
            m_currentMyWaveTrackId,
            playedSeconds);
    }

    m_currentMyWaveTrackId.clear();

    m_myWaveTrackStarted =
        false;

    m_playbackController
        ->playTrack(
            track);
}

void AppController::selectArtistTrack(
    int index)
{
    const Track track =
        m_artistModel
            ->trackAt(
                index);

    if (track.id.isEmpty()) {

        emit statusChanged(
            "Некорректный трек исполнителя");

        return;
    }

    if (
        !m_currentMyWaveTrackId.isEmpty() &&
        m_myWaveTrackStarted
    ) {

        const qint64 playedSeconds =
            m_playerService
                ->position() /
            1000;

        sendMyWaveFeedback(
            "skip",
            m_currentMyWaveTrackId,
            playedSeconds);
    }

    m_currentMyWaveTrackId.clear();

    m_myWaveTrackStarted =
        false;

    m_playbackController
        ->playTrack(
            track);
}

void AppController::play()
{
    m_playerService->play();
}

void AppController::pause()
{
    m_playerService->pause();
}

void AppController::stop()
{
    m_playerService->stop();
}

void AppController::seek(
    qint64 position)
{
    m_playerService
        ->seek(
            position);
}

void AppController::sendMyWaveFeedback(
    const QString &event,
    const QString &trackId,
    qint64 totalPlayedSeconds)
{
    if (m_yandexPersonal == nullptr) {
        return;
    }

    const QString batchId =
        batchIdForTrack(
            trackId);

    if (batchId.isEmpty()) {

        qDebug()
            << "No batch ID for Wave track:"
            << trackId;

        return;
    }

    m_yandexPersonal
        ->sendMyWaveFeedback(
            event,
            trackId,
            batchId,
            totalPlayedSeconds);
}

QString AppController::batchIdForTrack(
    const QString &trackId) const
{
    return m_myWaveTrackBatches
        .value(
            trackId);
}

SearchModel *
AppController::searchModel() const
{
    return m_searchModel;
}

MyWaveModel *
AppController::myWaveModel() const
{
    return m_myWaveModel;
}

PersonalPlaylistsModel *
AppController::personalPlaylistsModel() const
{
    return m_personalPlaylistsModel;
}

PlaylistModel *
AppController::playlistModel() const
{
    return m_playlistModel;
}

AlbumModel *
AppController::albumModel() const
{
    return m_albumModel;
}

ArtistModel *
AppController::artistModel() const
{
    return m_artistModel;
}

RecentListeningModel *
AppController::recentListeningModel() const
{
    return m_recentListeningModel;
}

bool AppController::isSearching() const
{
    return m_searching;
}

bool AppController::isPlaying() const
{
    if (m_playerService == nullptr) {
        return false;
    }

    return m_playerService
        ->isPlaying();
}

bool AppController::isLoadingMyWave() const
{
    return m_loadingMyWave;
}

bool AppController::isLoadingMoreMyWave() const
{
    return m_loadingMoreMyWave;
}

bool AppController::isLoadingRecommendations() const
{
    return m_loadingRecommendations;
}

bool AppController::isLoadingPlaylist() const
{
    return m_loadingPlaylist;
}

bool AppController::isLoadingAlbum() const
{
    return m_loadingAlbum;
}

bool AppController::isLoadingArtist() const
{
    return m_loadingArtist;
}

QString AppController::currentPlaylistTitle() const
{
    return m_currentPlaylistTitle;
}

int AppController::currentPlaylistTrackCount() const
{
    return m_currentPlaylistTrackCount;
}

QString AppController::currentAlbumTitle() const
{
    return m_currentAlbumTitle;
}

int AppController::currentAlbumTrackCount() const
{
    return m_currentAlbumTrackCount;
}

QString AppController::currentAlbumCoverUri() const
{
    return m_currentAlbumCoverUri;
}

QString AppController::currentArtistName() const
{
    return m_currentArtistName;
}

QString AppController::currentArtistCoverUri() const
{
    return m_currentArtistCoverUri;
}

QString AppController::currentArtistGenres() const
{
    return m_currentArtistGenres;
}

int AppController::currentArtistTrackCount() const
{
    return m_currentArtistTrackCount;
}

QString AppController::currentTrackTitle() const
{
    if (m_playbackController == nullptr) {
        return {};
    }

    return m_playbackController
        ->currentTrack()
        .title;
}

QString AppController::currentTrackArtist() const
{
    if (m_playbackController == nullptr) {
        return {};
    }

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

QString AppController::currentTrackCoverUri() const
{
    if (m_playbackController == nullptr) {
        return {};
    }

    return m_playbackController
        ->currentTrack()
        .coverUri;
}

qint64 AppController::position() const
{
    if (m_playerService == nullptr) {
        return 0;
    }

    return m_playerService
        ->position();
}

qint64 AppController::duration() const
{
    if (m_playerService == nullptr) {
        return 0;
    }

    return m_playerService
        ->duration();
}

PlaybackController::PlaybackState
AppController::playbackState() const
{
    if (m_playbackController == nullptr) {
        return PlaybackController::Idle;
    }

    return m_playbackController
        ->state();
}