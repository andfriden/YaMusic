#include "AppController.h"

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

            emit statusChanged(message);
        });

    connect(
        m_accountService,
        &AccountService::errorOccurred,
        this,
        [this](const QString &message) {

            qDebug()
                << "Account service error:"
                << message;

            emit statusChanged(message);
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

            emit statusChanged(message);
        });

    connect(
        m_trackService,
        &TrackService::streamInfoReceived,
        this,
        [this](
            const QList<TrackStreamInfo> &streams) {

            qDebug()
                << "Stream variants:"
                << streams.size();

            for (const TrackStreamInfo &stream :
                 streams) {

                qDebug()
                    << "codec:"
                    << stream.codec
                    << "bitrate:"
                    << stream.bitrateInKbps
                    << "preview:"
                    << stream.preview
                    << "direct:"
                    << stream.direct
                    << "url:"
                    << stream.downloadInfoUrl;
            }
        });

    connect(
        m_trackService,
        &TrackService::streamUrlReceived,
        this,
        [this](const QString &url) {

            qDebug()
                << "Stream URL:"
                << url;

            emit statusChanged(
                "Stream URL resolved");

            m_playerService->playUrl(url);
        });

    connect(
        m_trackService,
        &TrackService::errorOccurred,
        this,
        [this](const QString &message) {

            qDebug()
                << "Track service error:"
                << message;

            emit statusChanged(message);
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
    connect(
m_trackService,
&TrackService::streamUrlReceived,
this,
[this](const QString &url) {

   if (url.isEmpty()) {

       emit statusChanged(
           "Received empty stream URL");

       return;
   }

   qDebug()
       << "Stream URL received:"
       << url;

   emit statusChanged(
       "Starting playback...");

   m_playerService->playUrl(
       url);
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
    m_searchService->search(query);
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

    const QString message =
        QString("Selected: %1 — %2")
            .arg(track.title)
            .arg(
                track.artists.isEmpty()
                    ? QString()
                    : track.artists.first().name);

    qDebug() << message;

    emit statusChanged(message);

    m_trackService->loadStreamInfo(
        track.id);
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