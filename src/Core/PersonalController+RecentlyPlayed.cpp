#include "PersonalController.h"

#include "../Playback/PlaybackController.h"
#include "../Queue/QueueService.h"

#include <QDebug>

void PersonalController::connectRecentlyPlayed()
{
    if (
        m_recentListeningService == nullptr
    ) {
        return;
    }

    connect(
        m_recentListeningService,
        &RecentListeningService::loadingChanged,
        this,
        [](bool loading) {

            qDebug()
                << "Recent listening loading:"
                << loading;
        });

    connect(
        m_recentListeningService,
        &RecentListeningService::tracksReceived,
        this,
        [this](
            const QList<Track> &tracks) {

            m_recentListeningModel
                ->setTracks(
                    tracks);

            qDebug()
                << "Recently listened tracks:"
                << tracks.size();

            emit statusChanged(
                QString(
                    "Недавно слушали: %1 треков")
                .arg(
                    tracks.size()));
        });

    connect(
        m_recentListeningService,
        &RecentListeningService::errorOccurred,
        this,
        [this](
            const QString &message) {

            qDebug()
                << "Recent listening error:"
                << message;

            emit statusChanged(
                QString(
                    "Ошибка истории прослушивания: %1")
                .arg(
                    message));
        });
}

void PersonalController::selectRecentListening(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    const QList<Track> tracks =
        m_recentListeningModel
            ->tracks();

    if (
        tracks.isEmpty()
    ) {

        emit statusChanged(
            "История прослушивания пуста");

        return;
    }

    if (
        index < 0 ||
        index >= tracks.size()
    ) {

        emit statusChanged(
            "Некорректный трек истории");

        return;
    }

    /*
     * Перед началом другого источника
     * отключаем Rotor Wave.
     */

    m_myWaveQueueActive =
        false;

    m_waitingForMoreMyWave =
        false;

    if (
        m_myWaveTrackStarted
    ) {

        stopCurrentMyWaveTrack(
            "skip");
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue != nullptr
    ) {

        queue->clear();

        queue->addTracks(
            tracks);

        queue->setCurrentIndex(
            index);
    }

    const Track track =
        tracks.at(
            index);

    qDebug()
        << "Выбран трек истории:"
        << track.title
        << "| index:"
        << index;

    emit statusChanged(
        QString(
            "Выбран трек: %1")
        .arg(
            track.title));

    m_playbackController
        ->playTrack(
            track);
}