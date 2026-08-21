#include "AppController.h"

#include "../Playback/PlaybackController.h"
#include "../Player/PlayerService.h"
#include "../Yandex/Account/AccountService.h"
#include "../Yandex/Auth/YandexAuth.h"
#include "../Yandex/Catalog/SearchService.h"
#include "../Yandex/Catalog/TrackService.h"

#include <QDebug>

AppController::AppController(
    QObject *parent)
    : QObject(parent)
    , m_auth(new YandexAuth(this))
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
    , m_searchModel(
          new SearchModel(this))
    , m_playerService(
          new PlayerService(this))
    , m_playbackController(
          new PlaybackController(
              m_trackService,
              m_playerService,
              this))
{
    connect(
        m_accountService,
        &AccountService::accountReceived,
        this,
        [this](const Account &account) {

            const QString message =
                QString("Logged in as %1 (uid: %2)")
                    .arg(account.displayName)
                    .arg(account.uid);

            qDebug() << message;

            emit statusChanged(
                message);
        });

    connect(
        m_accountService,
        &AccountService::errorOccurred,
        this,
        [this](const QString &message) {

            qDebug()
                << "Account service error:"
                << message;

            emit statusChanged(
                message);
        });

    connect(
        m_searchService,
        &SearchService::searchStarted,
        this,
        [this]() {

            if (m_searching) {
                return;
            }

            m_searching = true;

            emit searchingChanged();

            emit statusChanged(
                "Searching...");
        });

    connect(
        m_searchService,
        &SearchService::searchReceived,
        this,
        [this](const SearchResults &results) {

            m_searching = false;

            emit searchingChanged();

            m_searchModel->setResults(
                results);

            qDebug()
                << "Search results:"
                << results.tracks.size()
                << "/"
                << results.total;

            emit statusChanged(
                QString("Found %1 results")
                    .arg(results.total));
        });

    connect(
        m_searchService,
        &SearchService::errorOccurred,
        this,
        [this](const QString &message) {

            m_searching = false;

            emit searchingChanged();

            qDebug()
                << "Search service error:"
                << message;

            m_searchModel->clear();

            emit statusChanged(
                message);
        });

    connect(
        m_playbackController,
        &PlaybackController::currentTrackChanged,
        this,
        [this]() {

            const Track track =
                m_playbackController->currentTrack();

            if (track.id.isEmpty()) {
                return;
            }

            QString artistName;

            if (!track.artists.isEmpty()) {
                artistName =
                    track.artists.first().name;
            }

            const QString message =
                artistName.isEmpty()
                    ? QString("Selected: %1")
                          .arg(track.title)
                    : QString("Selected: %1 — %2")
                          .arg(track.title)
                          .arg(artistName);

            qDebug() << message;

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
        [this](const QString &message) {

            qDebug()
                << "Playback error:"
                << message;

            emit statusChanged(
                message);
        });

    connect(
        m_playerService,
        &PlayerService::playingChanged,
        this,
        [this]() {

            emit playingChanged();

            if (m_playerService->isPlaying()) {

                emit statusChanged(
                    "Playing");
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
                "Paused");
        });

    connect(
        m_playerService,
        &PlayerService::playbackStopped,
        this,
        [this]() {

            emit statusChanged(
                "Stopped");
        });

    connect(
        m_playerService,
        &PlayerService::errorOccurred,
        this,
        [this](const QString &message) {

            qDebug()
                << "Player error:"
                << message;

            emit statusChanged(
                QString("Player error: %1")
                    .arg(message));
        });
}

void AppController::testConnection()
{
    qDebug()
        << "AppController is working";

    emit statusChanged(
        "AppController is working");
}

void AppController::testYandexApi()
{
    m_accountService->loadAccount();

    emit statusChanged(
        "Requesting Yandex account status...");
}

void AppController::testSearch(
    const QString &query)
{
    m_searchService->search(
        query);
}

void AppController::selectSearchResult(
    int index)
{
    const Track track =
        m_searchModel->trackAt(index);

    if (track.id.isEmpty()) {

        emit statusChanged(
            "Invalid search result");

        return;
    }

    m_playbackController->playTrack(
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
    m_playerService->seek(
        position);
}

SearchModel *AppController::searchModel() const
{
    return m_searchModel;
}

bool AppController::isSearching() const
{
    return m_searching;
}

bool AppController::isPlaying() const
{
    return m_playerService->isPlaying();
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

    return track.artists.first().name;
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

    return m_playerService->position();
}

qint64 AppController::duration() const
{
    if (m_playerService == nullptr) {
        return 0;
    }

    return m_playerService->duration();
}

PlaybackController::PlaybackState
AppController::playbackState() const
{
    if (m_playbackController == nullptr) {
        return PlaybackController::Idle;
    }

    return m_playbackController->state();
}