#include "PlaybackController.h"

#include "../Player/PlayerService.h"
#include "../Yandex/Catalog/TrackService.h"

PlaybackController::PlaybackController(
    TrackService *trackService,
    PlayerService *playerService,
    QObject *parent)
    : QObject(parent)
    , m_trackService(trackService)
    , m_playerService(playerService)
{
    Q_ASSERT(m_trackService != nullptr);
    Q_ASSERT(m_playerService != nullptr);

    connect(
        m_trackService,
        &TrackService::streamUrlReceived,
        this,
        &PlaybackController::handleStreamUrl);

    connect(
        m_trackService,
        &TrackService::errorOccurred,
        this,
        [this](const QString &message) {

            m_state = Error;

            emit stateChanged();

            emit playbackError(
                message);
        });

    connect(
        m_playerService,
        &PlayerService::playbackStarted,
        this,
        [this]() {

            m_state = Playing;

            emit stateChanged();
        });

    connect(
        m_playerService,
        &PlayerService::playbackPaused,
        this,
        [this]() {

            m_state = Paused;

            emit stateChanged();
        });

    connect(
        m_playerService,
        &PlayerService::playbackStopped,
        this,
        [this]() {

            if (m_state != Error) {

                m_state = Idle;

                emit stateChanged();
            }
        });

    connect(
        m_playerService,
        &PlayerService::playbackFinished,
        this,
        [this]() {

            m_state = Idle;

            emit stateChanged();
        });

    connect(
        m_playerService,
        &PlayerService::errorOccurred,
        this,
        [this](const QString &message) {

            m_state = Error;

            emit stateChanged();

            emit playbackError(
                message);
        });
}

void PlaybackController::playTrack(
    const Track &track)
{
    if (track.id.isEmpty()) {

        m_state = Error;

        emit stateChanged();

        emit playbackError(
            "Track ID is empty");

        return;
    }

    m_currentTrack = track;

    emit currentTrackChanged();

    m_state = Loading;

    emit stateChanged();

    m_trackService->loadStreamInfo(
        track.id);
}

Track PlaybackController::currentTrack() const
{
    return m_currentTrack;
}

PlaybackController::PlaybackState
PlaybackController::state() const
{
    return m_state;
}

void PlaybackController::handleStreamUrl(
    const QString &trackId,
    const QString &url)
{
    if (trackId != m_currentTrack.id) {
        return;
    }

    if (url.isEmpty()) {

        m_state = Error;

        emit stateChanged();

        emit playbackError(
            "Stream URL is empty");

        return;
    }

    m_playerService->playUrl(
        url);
}