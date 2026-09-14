#pragma once

#include <QObject>
#include <QString>
#include <memory>
#include "../MediaControls/SystemMediaControls.h"
#include "../Models/Track.h"
#include "../Queue/QueueService.h"

class PlayerService;
class TrackService;
class QNetworkAccessManager;
class QNetworkReply;

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

    SystemMediaControls *systemMediaControls() const;

    /*
     * Playback control
     */

    void playTrack(
        const Track &track);

    /*
     * Queue the given tracks (replacing any existing queue),
     * jump to the specified index and immediately start playback.
     * sourceTitle / sourceType are forwarded to QueueService::setSource
     * so the UI can display "Playing from …".
     */

    void playFromSource(
        const QList<Track> &tracks,
        int index,
        const QString &sourceTitle = {},
        const QString &sourceType = {});

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

    /*
     * Emitted when the queue reaches its end and auto-repeat
     * is off / cannot cycle. sourceType tells what kind of
     * content just finished ("playlist", "album", "artist", …).
     * sourceTitle is the human-readable name.
     * The receiver can load a related source and resume playback.
     */

    void playlistExhausted(
        const QString &sourceType,
        const QString &sourceTitle);

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

    void setupSystemMediaControls();

    void fetchCurrentCover();

private:
    TrackService *m_trackService = nullptr;

    PlayerService *m_playerService = nullptr;

    QueueService *m_queueService = nullptr;

    Track m_currentTrack;

    PlaybackState m_state =
        Idle;

    std::unique_ptr<SystemMediaControls>
        m_systemMediaControls;

    QNetworkAccessManager *m_coverNetwork = nullptr;

    QString m_pendingCoverUri;
};