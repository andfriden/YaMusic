#include "MediaControlsMacOS.h"

#if __has_feature(objc_arc)
#error This file must be compiled with Automatic Reference Counting (ARC) disabled.
#endif

#import <Cocoa/Cocoa.h>
#import <MediaPlayer/MediaPlayer.h>

/*
 * Private ObjC implementation behind a PIMPL.
 * Keeps the C++ header clean of ObjC types.
 */

class MediaControlsMacOS::Impl
{
public:
    Impl()
    {
        center = [MPRemoteCommandCenter sharedCommandCenter];
    }

    ~Impl()
    {
        disconnectCommands();
    }

    void connectCommands(MediaControlsMacOS *owner)
    {
        if (connected)
            return;

        MPRemoteCommandCenter *c = center;

        toggleTarget = [c.togglePlayPauseCommand
            addTargetWithHandler:^(MPRemoteCommandEvent *) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    Q_EMIT owner->togglePlayPauseRequested();
                });
                return MPRemoteCommandHandlerStatusSuccess;
            }];

        playTarget = [c.playCommand
            addTargetWithHandler:^(MPRemoteCommandEvent *) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    Q_EMIT owner->playRequested();
                });
                return MPRemoteCommandHandlerStatusSuccess;
            }];

        pauseTarget = [c.pauseCommand
            addTargetWithHandler:^(MPRemoteCommandEvent *) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    Q_EMIT owner->pauseRequested();
                });
                return MPRemoteCommandHandlerStatusSuccess;
            }];

        nextTarget = [c.nextTrackCommand
            addTargetWithHandler:^(MPRemoteCommandEvent *) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    Q_EMIT owner->nextRequested();
                });
                return MPRemoteCommandHandlerStatusSuccess;
            }];

        previousTarget = [c.previousTrackCommand
            addTargetWithHandler:^(MPRemoteCommandEvent *) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    Q_EMIT owner->previousRequested();
                });
                return MPRemoteCommandHandlerStatusSuccess;
            }];

        seekTarget = [c.changePlaybackPositionCommand
            addTargetWithHandler:^(MPRemoteCommandEvent *event) {
                MPChangePlaybackPositionCommandEvent *pe =
                    static_cast<MPChangePlaybackPositionCommandEvent *>(event);
                dispatch_async(dispatch_get_main_queue(), ^{
                    qint64 ms = static_cast<qint64>(pe.positionTime * 1000.0);
                    Q_EMIT owner->seekRequested(ms);
                });
                return MPRemoteCommandHandlerStatusSuccess;
            }];

        connected = true;
    }

    void disconnectCommands()
    {
        if (!connected)
            return;

        MPRemoteCommandCenter *c = center;

        [c.togglePlayPauseCommand removeTarget:toggleTarget];
        [c.playCommand removeTarget:playTarget];
        [c.pauseCommand removeTarget:pauseTarget];
        [c.nextTrackCommand removeTarget:nextTarget];
        [c.previousTrackCommand removeTarget:previousTarget];
        [c.changePlaybackPositionCommand removeTarget:seekTarget];

        toggleTarget = nil;
        playTarget = nil;
        pauseTarget = nil;
        nextTarget = nil;
        previousTarget = nil;
        seekTarget = nil;

        connected = false;
    }

    void updateInfo(const SystemMediaControls::Metadata &md,
                    SystemMediaControls::PlaybackStatus status,
                    qint64 positionMs,
                    qint64 durationMs)
    {
        NSMutableDictionary *info =
            [[NSMutableDictionary alloc] init];

        if (!md.title.isEmpty())
        {
            info[MPMediaItemPropertyTitle] =
                [NSString stringWithUTF8String:md.title.toUtf8().constData()];
        }

        if (!md.artist.isEmpty())
        {
            info[MPMediaItemPropertyArtist] =
                [NSString stringWithUTF8String:md.artist.toUtf8().constData()];
        }

        if (!md.album.isEmpty())
        {
            info[MPMediaItemPropertyAlbumTitle] =
                [NSString stringWithUTF8String:md.album.toUtf8().constData()];
        }

        if (durationMs > 0)
        {
            info[MPMediaItemPropertyPlaybackDuration] =
                @(static_cast<double>(durationMs) / 1000.0);
        }

        if (positionMs >= 0)
        {
            info[MPNowPlayingInfoPropertyElapsedPlaybackTime] =
                @(static_cast<double>(positionMs) / 1000.0);
        }

        switch (status)
        {
        case SystemMediaControls::PlaybackStatus::Playing:
            info[MPNowPlayingInfoPropertyPlaybackRate] = @(1.0);
            break;
        case SystemMediaControls::PlaybackStatus::Paused:
        case SystemMediaControls::PlaybackStatus::Stopped:
            info[MPNowPlayingInfoPropertyPlaybackRate] = @(0.0);
            break;
        }

        [[MPNowPlayingInfoCenter defaultCenter] setNowPlayingInfo:info];
        [info release];
    }

private:
    MPRemoteCommandCenter *center = nil;
    id toggleTarget = nil;
    id playTarget = nil;
    id pauseTarget = nil;
    id nextTarget = nil;
    id previousTarget = nil;
    id seekTarget = nil;
    bool connected = false;
};

// C++ interface

MediaControlsMacOS::MediaControlsMacOS(QObject *parent)
    : SystemMediaControls(parent)
    , d(new Impl())
{
}

MediaControlsMacOS::~MediaControlsMacOS()
{
    setEnabled(false);
    delete d;
}

void MediaControlsMacOS::platformSetEnabled(bool enabled)
{
    if (enabled)
        d->connectCommands(this);
    else
        d->disconnectCommands();

    if (!enabled)
    {
        [[MPNowPlayingInfoCenter defaultCenter]
            setNowPlayingInfo:nil];
    }
}

void MediaControlsMacOS::platformSetMetadata(const Metadata &)
{
    updateNowPlayingInfo();
}

void MediaControlsMacOS::platformSetPlaybackStatus(PlaybackStatus)
{
    updateNowPlayingInfo();
}

void MediaControlsMacOS::platformSetPosition(qint64)
{
    updateNowPlayingInfo();
}

void MediaControlsMacOS::platformSetDuration(qint64)
{
    updateNowPlayingInfo();
}

void MediaControlsMacOS::updateNowPlayingInfo()
{
    d->updateInfo(
        m_metadata,
        m_status,
        m_positionMs,
        m_durationMs);
}