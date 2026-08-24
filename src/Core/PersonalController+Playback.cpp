#include "PersonalController.h"

#include "../Player/PlayerService.h"

#include <QDebug>

void PersonalController::connectPlayback()
{
    if (
        m_playerService == nullptr
    ) {
        return;
    }

    /*
     * Track started
     */

    connect(
        m_playerService,
        &PlayerService::playbackStarted,
        this,
        [this]() {

            /*
             * Feedback нужен только
             * для активной My Wave.
             */

            if (
                !m_myWaveQueueActive
            ) {
                return;
            }

            if (
                m_currentMyWaveTrackId
                    .isEmpty()
            ) {
                return;
            }

            if (
                m_myWaveTrackStarted
            ) {
                return;
            }

            const QString batchId =
                batchIdForTrack(
                    m_currentMyWaveTrackId);

            if (
                batchId.isEmpty()
            ) {

                qDebug()
                    << "My Wave track has no batchId:"
                    << m_currentMyWaveTrackId;

                return;
            }

            m_myWaveTrackStarted =
                true;

            sendMyWaveFeedback(
                "trackStarted",
                m_currentMyWaveTrackId);

            qDebug()
                << "My Wave trackStarted:"
                << m_currentMyWaveTrackId;
        });

    /*
     * Track finished
     */

    connect(
        m_playerService,
        &PlayerService::playbackFinished,
        this,
        [this]() {

            /*
             * Сохраняем состояние трека,
             * который только что закончился.
             *
             * Не очищаем его до тех пор,
             * пока не определим следующий шаг.
             */

            const QString finishedTrackId =
                m_currentMyWaveTrackId;

            const bool isMyWavePlayback =
                m_myWaveQueueActive &&
                !finishedTrackId.isEmpty();

            const bool wasStarted =
                m_myWaveTrackStarted;

            const qint64 playedSeconds =
                m_playerService != nullptr
                    ? m_playerService
                        ->position() /
                      1000
                    : 0;

            /*
             * Feedback trackFinished.
             */

            if (
                isMyWavePlayback &&
                wasStarted
            ) {

                sendMyWaveFeedback(
                    "trackFinished",
                    finishedTrackId,
                    playedSeconds);

                qDebug()
                    << "My Wave trackFinished:"
                    << finishedTrackId
                    << "| seconds:"
                    << playedSeconds;
            }

            /*
             * Если это My Wave,
             * передаём управление очереди
             * специальному обработчику.
             *
             * Он либо:
             *
             * 1. запускает следующий трек
             *    текущей партии;
             *
             * 2. запрашивает новую Rotor-партию.
             */

            if (
                isMyWavePlayback
            ) {

                handleMyWavePlaybackFinished();

                /*
                 * Важно:
                 *
                 * handleMyWavePlaybackFinished()
                 * мог уже установить
                 * m_currentMyWaveTrackId
                 * следующего трека.
                 *
                 * Поэтому здесь нельзя
                 * безусловно очищать состояние.
                 */

                if (
                    m_currentMyWaveTrackId ==
                    finishedTrackId
                ) {

                    m_currentMyWaveTrackId
                        .clear();

                    m_myWaveTrackStarted =
                        false;
                }

                return;
            }
        });
}