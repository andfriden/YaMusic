#include "PlaybackController.h"

#include "../Player/PlayerService.h"
#include "../Yandex/Catalog/TrackService.h"

#include <QDebug>

PlaybackController::PlaybackController(
    TrackService *trackService,
    PlayerService *playerService,
    QueueService *queueService,
    QObject *parent)
    : QObject(parent)
    , m_trackService(trackService)
    , m_playerService(playerService)
    , m_queueService(queueService)
{
    if (m_playerService != nullptr) {

        connect(
            m_playerService,
            &PlayerService::playbackStarted,
            this,
            [this]() {

                setState(
                    Playing);
            });

        connect(
            m_playerService,
            &PlayerService::playbackPaused,
            this,
            [this]() {

                setState(
                    Paused);
            });

        connect(
            m_playerService,
            &PlayerService::playbackStopped,
            this,
            [this]() {

                setState(
                    Stopped);
            });

        connect(
            m_playerService,
            &PlayerService::playbackFinished,
            this,
            &PlaybackController::handlePlaybackFinished);

        connect(
            m_playerService,
            &PlayerService::errorOccurred,
            this,
            [this](
                const QString &message) {

                setState(
                    Error);

                emit playbackError(
                    message);
            });
    }

    if (m_trackService != nullptr) {

        connect(
            m_trackService,
            &TrackService::streamUrlReceived,
            this,
            [this](
                const QString &trackId,
                const QString &url) {

                handleStreamUrl(
                    trackId,
                    url);
            });

        connect(
            m_trackService,
            &TrackService::errorOccurred,
            this,
            [this](
                const QString &message) {

                setState(
                    Error);

                emit playbackError(
                    message);
            });
    }

    if (m_queueService != nullptr) {

        connect(
            m_queueService,
            &QueueService::repeatModeChanged,
            this,
            [this]() {

                emit repeatModeChanged();
            });

        connect(
            m_queueService,
            &QueueService::shuffleChanged,
            this,
            [this]() {

                emit shuffleChanged();
            });
    }
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

QueueService *
PlaybackController::queueService() const
{
    return m_queueService;
}

void PlaybackController::playTrack(
    const Track &track)
{
    if (track.id.isEmpty()) {

        setState(
            Error);

        emit playbackError(
            "Некорректный трек");

        return;
    }

    if (m_queueService != nullptr) {

        const QList<Track> queueTracks =
            m_queueService->tracks();

        for (
            int i = 0;
            i < queueTracks.size();
            ++i
        ) {

            if (
                queueTracks.at(i).id ==
                track.id
            ) {

                m_queueService
                    ->setCurrentIndex(
                        i);

                break;
            }
        }
    }

    m_currentTrack =
        track;

    emit currentTrackChanged();

    setState(
        Loading);

    if (m_trackService == nullptr) {

        setState(
            Error);

        emit playbackError(
            "TrackService недоступен");

        return;
    }

    qDebug()
        << "Loading stream:"
        << track.id
        << "|"
        << track.title;

    m_trackService
        ->loadStreamInfo(
            track.id);
}

void PlaybackController::playQueue()
{
    if (m_queueService == nullptr) {

        setState(
            Error);

        emit playbackError(
            "QueueService недоступен");

        return;
    }

    const Track track =
        m_queueService
            ->currentTrack();

    if (track.id.isEmpty()) {

        setState(
            Error);

        emit playbackError(
            "Очередь пуста");

        return;
    }

    playTrack(
        track);
}

void PlaybackController::playCurrent()
{
    if (m_currentTrack.id.isEmpty()) {

        playQueue();

        return;
    }

    playTrack(
        m_currentTrack);
}

void PlaybackController::pause()
{
    if (m_playerService == nullptr) {
        return;
    }

    m_playerService->pause();
}

void PlaybackController::resume()
{
    if (m_playerService == nullptr) {
        return;
    }

    m_playerService->play();
}

void PlaybackController::stop()
{
    if (m_playerService == nullptr) {
        return;
    }

    m_playerService->stop();
}

bool PlaybackController::next()
{
    if (m_queueService == nullptr) {
        return false;
    }

    if (m_queueService->hasNext()) {

        m_queueService->next();

        return playQueueCurrentTrack();
    }

    if (
        m_queueService->repeatMode() ==
            QueueService::RepeatAll &&
        m_queueService->count() > 0
    ) {

        m_queueService
            ->setCurrentIndex(
                0);

        return playQueueCurrentTrack();
    }

    return false;
}

bool PlaybackController::previous()
{
    if (m_queueService == nullptr) {
        return false;
    }

    if (m_queueService->hasPrevious()) {

        m_queueService->previous();

        return playQueueCurrentTrack();
    }

    if (
        m_queueService->repeatMode() ==
            QueueService::RepeatAll &&
        m_queueService->count() > 0
    ) {

        m_queueService
            ->setCurrentIndex(
                m_queueService->count() - 1);

        return playQueueCurrentTrack();
    }

    return false;
}

void PlaybackController::setRepeatMode(
    QueueService::RepeatMode mode)
{
    if (m_queueService == nullptr) {
        return;
    }

    m_queueService
        ->setRepeatMode(
            mode);
}

QueueService::RepeatMode
PlaybackController::repeatMode() const
{
    if (m_queueService == nullptr) {
        return QueueService::RepeatOff;
    }

    return m_queueService
        ->repeatMode();
}

void PlaybackController::cycleRepeatMode()
{
    if (m_queueService == nullptr) {
        return;
    }

    m_queueService
        ->cycleRepeatMode();
}

bool PlaybackController::shuffleEnabled() const
{
    if (m_queueService == nullptr) {
        return false;
    }

    return m_queueService
        ->shuffleEnabled();
}

void PlaybackController::setShuffleEnabled(
    bool enabled)
{
    if (m_queueService == nullptr) {
        return;
    }

    m_queueService
        ->setShuffleEnabled(
            enabled);
}

void PlaybackController::toggleShuffle()
{
    if (m_queueService == nullptr) {
        return;
    }

    m_queueService
        ->toggleShuffle();
}

void PlaybackController::setState(
    PlaybackState state)
{
    if (m_state == state) {
        return;
    }

    m_state =
        state;

    emit stateChanged();
}

void PlaybackController::handleStreamUrl(
    const QString &trackId,
    const QString &url)
{
    if (
        trackId.isEmpty() ||
        trackId != m_currentTrack.id
    ) {

        qDebug()
            << "Ignoring stale stream URL:"
            << trackId;

        return;
    }

    if (url.trimmed().isEmpty()) {

        setState(
            Error);

        emit playbackError(
            "Stream URL пуст");

        return;
    }

    if (m_playerService == nullptr) {

        setState(
            Error);

        emit playbackError(
            "PlayerService недоступен");

        return;
    }

    qDebug()
        << "PlaybackController stream URL:"
        << trackId;

    m_playerService
        ->playUrl(
            url);
}

bool PlaybackController::playQueueCurrentTrack()
{
    if (m_queueService == nullptr) {
        return false;
    }

    const Track track =
        m_queueService
            ->currentTrack();

    if (track.id.isEmpty()) {
        return false;
    }

    qDebug()
        << "Queue next track:"
        << track.title;

    playTrack(
        track);

    return true;
}

void PlaybackController::handlePlaybackFinished()
{
    if (m_queueService == nullptr) {

        setState(
            Stopped);

        return;
    }

    const QueueService::RepeatMode mode =
        m_queueService
            ->repeatMode();

    if (
        mode ==
        QueueService::RepeatOne
    ) {

        qDebug()
            << "Repeat One:"
            << m_currentTrack.title;

        if (playQueueCurrentTrack()) {
            return;
        }

        setState(
            Stopped);

        return;
    }

    if (m_queueService->hasNext()) {

        m_queueService->next();

        if (playQueueCurrentTrack()) {
            return;
        }
    }

    if (
        mode ==
            QueueService::RepeatAll &&
        m_queueService->count() > 0
    ) {

        qDebug()
            << "Repeat All:"
            << "returning to first track";

        m_queueService
            ->setCurrentIndex(
                0);

        if (playQueueCurrentTrack()) {
            return;
        }
    }

    qDebug()
        << "Queue finished";

    setState(
        Stopped);
}