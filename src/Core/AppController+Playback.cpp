#include "AppController.h"

#include "../Player/PlayerService.h"
#include "../Queue/QueueService.h"

#include <QDebug>

// =============================================================
// Playback
// =============================================================

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
    )
    {
        emit statusChanged(
            "Следующего трека нет");
    }
}


void AppController::previous()
{
    if (
        !m_playbackController
            ->previous()
    )
    {
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
        mode < QueueService::RepeatOff ||
        mode > QueueService::RepeatOne
    )
    {
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