#include "PersonalController.h"

#include "../Playback/PlaybackController.h"
#include "../Player/PlayerService.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Personal/YandexPersonal.h"

#include <QDebug>

void PersonalController::connectMyWave()
{
    if (
        m_yandexPersonal == nullptr
    ) {
        return;
    }

    connect(
        m_yandexPersonal,
        &YandexPersonal::myWaveReceived,
        this,
        [this](
            const QList<Track> &tracks,
            const QString &batchId) {

            handleMyWaveReceived(
                tracks,
                batchId);
        });

    connect(
        m_yandexPersonal,
        &YandexPersonal::errorOccurred,
        this,
        [this](
            const QString &message) {

            if (
                m_loadingMyWave
            ) {

                m_loadingMyWave =
                    false;

                emit loadingMyWaveChanged();
            }

            if (
                m_loadingMoreMyWave
            ) {

                m_loadingMoreMyWave =
                    false;

                emit loadingMoreMyWaveChanged();
            }

            m_waitingForMoreMyWave =
                false;

            qDebug()
                << "My Wave error:"
                << message;

            emit statusChanged(
                message);
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
}

void PersonalController::loadMyWave()
{
    if (
        m_loadingMyWave ||
        m_loadingMoreMyWave
    ) {
        return;
    }

    if (
        m_yandexPersonal == nullptr
    ) {
        return;
    }

    m_myWaveQueueActive =
        true;

    m_waitingForMoreMyWave =
        false;

    m_currentMyWaveTrackId.clear();

    m_myWaveTrackStarted =
        false;

    m_loadingMyWave =
        true;

    emit loadingMyWaveChanged();

    m_myWaveTrackBatches.clear();

    m_myWaveModel
        ->clear();

    QueueService *queue =
        m_playbackController != nullptr
            ? m_playbackController
                ->queueService()
            : nullptr;

    if (
        queue != nullptr
    ) {
        queue->clear();
    }

    qDebug()
        << "My Wave: loading first batch";

    emit statusChanged(
        "Загрузка моей волны...");

    m_yandexPersonal
        ->loadMyWave();
}

void PersonalController::loadMoreMyWave()
{
    if (
        m_loadingMyWave ||
        m_loadingMoreMyWave
    ) {
        return;
    }

    if (
        m_yandexPersonal == nullptr
    ) {
        return;
    }

    const Track lastTrack =
        m_myWaveModel
            ->lastTrack();

    if (
        lastTrack.id.isEmpty()
    ) {

        emit statusChanged(
            "Нет трека для продолжения My Wave");

        return;
    }

    m_loadingMoreMyWave =
        true;

    emit loadingMoreMyWaveChanged();

    m_waitingForMoreMyWave =
        true;

    qDebug()
        << "My Wave: loading more from"
        << lastTrack.id;

    emit statusChanged(
        "Загрузка следующей части моей волны...");

    m_yandexPersonal
        ->loadMoreMyWave(
            lastTrack.id);
}

void PersonalController::handleMyWaveReceived(
    const QList<Track> &tracks,
    const QString &batchId)
{
    const QString trimmedBatchId =
        batchId.trimmed();

    if (
        tracks.isEmpty()
    ) {

        const bool wasLoadingMore =
            m_loadingMoreMyWave;

        m_loadingMyWave =
            false;

        m_loadingMoreMyWave =
            false;

        m_waitingForMoreMyWave =
            false;

        emit loadingMyWaveChanged();
        emit loadingMoreMyWaveChanged();

        qDebug()
            << "My Wave:"
            << (
                wasLoadingMore
                    ? "empty next batch"
                    : "empty first batch");

        emit statusChanged(
            wasLoadingMore
                ? "Моя волна вернула пустую следующую партию"
                : "Моя волна вернула пустую партию");

        return;
    }

    /*
     * Запоминаем batchId для каждого
     * полученного трека.
     */

    if (
        !trimmedBatchId.isEmpty()
    ) {

        for (
            const Track &track :
            tracks
        ) {

            if (
                track.id.isEmpty()
            ) {
                continue;
            }

            m_myWaveTrackBatches.insert(
                track.id,
                trimmedBatchId);
        }
    }

    const bool isMoreBatch =
        m_loadingMoreMyWave;

    m_loadingMyWave =
        false;

    m_loadingMoreMyWave =
        false;

    emit loadingMyWaveChanged();
    emit loadingMoreMyWaveChanged();

    /*
     * Первая партия.
     */

    if (
        !isMoreBatch
    ) {

        m_myWaveModel
            ->setTracks(
                tracks);

        QueueService *queue =
            m_playbackController != nullptr
                ? m_playbackController
                    ->queueService()
                : nullptr;

        if (
            queue != nullptr
        ) {

            queue->clear();

            queue->addTracks(
                tracks);
        }

        m_myWaveQueueActive =
            true;

        m_waitingForMoreMyWave =
            false;

        qDebug()
            << "My Wave first batch:"
            << tracks.size()
            << "tracks"
            << "| batch:"
            << trimmedBatchId;

        emit statusChanged(
            QString(
                "Моя волна: %1 треков")
                .arg(
                    m_myWaveModel
                        ->count()));

        return;
    }

    /*
     * Следующая партия.
     */

    const int oldCount =
        m_myWaveModel
            ->count();

    m_myWaveModel
        ->appendTracks(
            tracks);

    appendMyWaveTracksToQueue(
        tracks);

    const int appended =
        m_myWaveModel
            ->count() -
        oldCount;

    qDebug()
        << "My Wave next batch:"
        << tracks.size()
        << "received"
        << "|"
        << appended
        << "appended"
        << "| batch:"
        << trimmedBatchId;

    emit statusChanged(
        QString(
            "Моя волна: %1 треков")
            .arg(
                m_myWaveModel
                    ->count()));

    if (
        !m_waitingForMoreMyWave
    ) {
        return;
    }

    m_waitingForMoreMyWave =
        false;

    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue == nullptr
    ) {
        return;
    }

    if (
        !queue->hasNext()
    ) {

        qDebug()
            << "My Wave:"
               "new batch arrived,"
               "but queue has no next track";

        return;
    }

    queue->next();

    const Track nextTrack =
        queue
            ->currentTrack();

    if (
        nextTrack.id.isEmpty()
    ) {
        return;
    }

    m_currentMyWaveTrackId =
        nextTrack.id;

    m_myWaveTrackStarted =
        false;

    qDebug()
        << "My Wave continued:"
        << nextTrack.title;

    m_playbackController
        ->playTrack(
            nextTrack);
}

void PersonalController::selectMyWaveTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    const Track track =
        m_myWaveModel
            ->trackAt(
                index);

    if (
        track.id.isEmpty()
    ) {

        emit statusChanged(
            "Некорректный трек моей волны");

        return;
    }

    if (
        m_myWaveQueueActive &&
        m_myWaveTrackStarted
    ) {

        stopCurrentMyWaveTrack(
            "skip");
    }

    startMyWaveQueue(
        index);

    if (
        !m_myWaveQueueActive
    ) {
        return;
    }

    emit myWaveTrackSelected(
        track);

    emit statusChanged(
        QString(
            "Выбран трек: %1")
            .arg(
                track.title));

    m_playbackController
        ->playTrack(
            track);
}

void PersonalController::startMyWaveQueue(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue == nullptr
    ) {
        return;
    }

    const QList<Track> tracks =
        m_myWaveModel
            ->tracks();

    if (
        tracks.isEmpty()
    ) {

        emit statusChanged(
            "Моя волна пока пуста");

        return;
    }

    if (
        index < 0 ||
        index >= tracks.size()
    ) {

        emit statusChanged(
            "Некорректный индекс трека My Wave");

        return;
    }

    queue->clear();

    queue->addTracks(
        tracks);

    if (
        !queue->setCurrentIndex(
            index)
    ) {

        emit statusChanged(
            "Не удалось выбрать трек My Wave");

        return;
    }

    m_myWaveQueueActive =
        true;

    m_waitingForMoreMyWave =
        false;

    m_currentMyWaveTrackId =
        queue
            ->currentTrack()
            .id;

    m_myWaveTrackStarted =
        false;

    qDebug()
        << "My Wave queue:"
        << queue->count()
        << "| current:"
        << queue->currentIndex();
}

void PersonalController::appendMyWaveTracksToQueue(
    const QList<Track> &tracks)
{
    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue == nullptr
    ) {
        return;
    }

    const QList<Track> queuedTracks =
        queue->tracks();

    for (
        const Track &track :
        tracks
    ) {

        if (
            track.id.isEmpty()
        ) {
            continue;
        }

        bool exists =
            false;

        for (
            const Track &queuedTrack :
            queuedTracks
        ) {

            if (
                queuedTrack.id ==
                track.id
            ) {

                exists =
                    true;

                break;
            }
        }

        if (
            exists
        ) {
            continue;
        }

        queue->addTrack(
            track);
    }
}

void PersonalController::handleMyWavePlaybackFinished()
{
    if (
        !m_myWaveQueueActive
    ) {
        return;
    }

    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue == nullptr
    ) {
        return;
    }

    /*
     * Есть следующий локальный трек.
     */

    if (
        queue->hasNext()
    ) {

        queue->next();

        const Track nextTrack =
            queue
                ->currentTrack();

        if (
            nextTrack.id.isEmpty()
        ) {
            return;
        }

        m_currentMyWaveTrackId =
            nextTrack.id;

        m_myWaveTrackStarted =
            false;

        qDebug()
            << "My Wave next:"
            << nextTrack.title;

        m_playbackController
            ->playTrack(
                nextTrack);

        return;
    }

    /*
     * Локальная партия закончилась.
     */

    if (
        m_loadingMoreMyWave
    ) {
        return;
    }

    const Track lastTrack =
        m_myWaveModel
            ->lastTrack();

    if (
        lastTrack.id.isEmpty()
    ) {

        qDebug()
            << "My Wave finished:"
               "no last track";

        m_myWaveQueueActive =
            false;

        return;
    }

    m_waitingForMoreMyWave =
        true;

    qDebug()
        << "My Wave batch finished."
        << "Loading more from:"
        << lastTrack.id;

    loadMoreMyWave();
}

QString PersonalController::batchIdForTrack(
    const QString &trackId) const
{
    return m_myWaveTrackBatches
        .value(
            trackId);
}

void PersonalController::stopCurrentMyWaveTrack(
    const QString &event)
{
    if (
        m_currentMyWaveTrackId
            .isEmpty()
    ) {
        return;
    }

    if (
        !m_myWaveTrackStarted
    ) {

        m_currentMyWaveTrackId
            .clear();

        return;
    }

    const qint64 playedSeconds =
        m_playerService != nullptr
            ? m_playerService
                ->position() /
              1000
            : 0;

    sendMyWaveFeedback(
        event,
        m_currentMyWaveTrackId,
        playedSeconds);

    m_currentMyWaveTrackId
        .clear();

    m_myWaveTrackStarted =
        false;
}

void PersonalController::sendMyWaveFeedback(
    const QString &event,
    const QString &trackId,
    qint64 totalPlayedSeconds)
{
    if (
        m_yandexPersonal == nullptr
    ) {
        return;
    }

    const QString batchId =
        batchIdForTrack(
            trackId);

    if (
        batchId.isEmpty()
    ) {

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