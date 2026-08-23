#pragma once

#include <QObject>
#include <QString>

#include "../Models/Track.h"
#include "../Queue/QueueService.h"

class PlayerService;
class TrackService;

class PlaybackController : public QObject
{
    Q_OBJECT

public:
    enum PlaybackState {
        Idle,
        Loading,
        Playing,
        Paused,
        Stopped,
        Error
    };

    Q_ENUM(PlaybackState)

    explicit PlaybackController(
        TrackService *trackService,
        PlayerService *playerService,
        QueueService *queueService,
        QObject *parent = nullptr);

    Track currentTrack() const;

    PlaybackState state() const;

    QueueService *queueService() const;

    void playTrack(
        const Track &track);

    void playQueue();

    void playCurrent();

    void pause();

    void resume();

    void stop();

    bool next();

    bool previous();

    void setRepeatMode(
        QueueService::RepeatMode mode);

    QueueService::RepeatMode
    repeatMode() const;

    void cycleRepeatMode();

    bool shuffleEnabled() const;

    void setShuffleEnabled(
        bool enabled);

    void toggleShuffle();

    signals:
        void currentTrackChanged();

    void stateChanged();

    void playbackError(
        const QString &message);

    void repeatModeChanged();

    void shuffleChanged();

private:
    void setState(
        PlaybackState state);

    void handlePlaybackFinished();

    void handleStreamUrl(
        const QString &trackId,
        const QString &url);

    bool playQueueCurrentTrack();

private:
    TrackService *m_trackService = nullptr;

    PlayerService *m_playerService = nullptr;

    QueueService *m_queueService = nullptr;

    Track m_currentTrack;

    PlaybackState m_state =
        Idle;
};