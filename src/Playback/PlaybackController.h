#pragma once

#include <QObject>
#include <QString>

#include "../Yandex/Catalog/SearchResult.h"

class PlayerService;
class TrackService;

class PlaybackController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        PlaybackState state
        READ state
        NOTIFY stateChanged)

public:
    enum PlaybackState {
        Idle,
        Loading,
        Playing,
        Paused,
        Error
    };

    Q_ENUM(PlaybackState)

    explicit PlaybackController(
        TrackService *trackService,
        PlayerService *playerService,
        QObject *parent = nullptr);

    void playTrack(
        const Track &track);

    Track currentTrack() const;

    PlaybackState state() const;

    signals:
        void currentTrackChanged();

    void stateChanged();

    void playbackError(
        const QString &message);

private:
    void handleStreamUrl(
        const QString &trackId,
        const QString &url);

    TrackService *m_trackService = nullptr;

    PlayerService *m_playerService = nullptr;

    Track m_currentTrack;

    PlaybackState m_state = Idle;
};