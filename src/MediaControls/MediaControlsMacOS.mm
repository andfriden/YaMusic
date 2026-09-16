#include "MediaControlsMacOS.h"

#if __has_feature(objc_arc)
#error This file must be compiled with Automatic Reference Counting (ARC) disabled.
#endif

#import <Cocoa/Cocoa.h>
#import <MediaPlayer/MediaPlayer.h>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QImage>
#include <QFile>

/*
 * Private ObjC implementation behind a PIMPL.
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
                    qint64 durationMs,
                    const QImage &artworkImage)
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

if (!artworkImage.isNull())
        {
            CGImageRef cgImage = artworkImage.toCGImage();
            if (cgImage)
            {
                NSImage *nsImage = [[NSImage alloc] initWithCGImage:cgImage
                                                               size:NSZeroSize];
                CGImageRelease(cgImage);
                if (nsImage)
                {
                    MPMediaItemArtwork *artwork =
                        [[MPMediaItemArtwork alloc] initWithBoundsSize:nsImage.size
                                                        requestHandler:^NSImage *(CGSize) {
                                                            return nsImage;
                                                        }];
                    info[MPMediaItemPropertyArtwork] = artwork;
                    [artwork release];
                    [nsImage release];
                }
            }
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
    , m_network(new QNetworkAccessManager(this))
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
    /*
     * Only clear artwork and restart the fetch when the
     * track actually changes (not when fetchCurrentCover
     * re-publishes metadata with a cached file:// URL).
     */

    if (m_metadata.trackId != m_lastArtworkTrackId)
    {
        m_artworkImage = {};
        m_lastArtworkTrackId = m_metadata.trackId;
        updateNowPlayingInfo();
        startArtworkFetch(m_metadata.coverUrl);
    }
    else
    {
        /*
         * Same track — coverUrl may have changed from a
         * remote URL to a cached file:// path. Re-fetch
         * but keep the existing artwork visible.
         */
        startArtworkFetch(m_metadata.coverUrl);
    }
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
        m_durationMs,
        m_artworkImage);
}

QString MediaControlsMacOS::fullCoverUrl(const QString &coverUri)
{
    if (coverUri.isEmpty())
        return {};

    /*
     * Local file path — pass through unchanged.
     */
    if (coverUri.startsWith(QStringLiteral("file://")))
        return coverUri;

    QString url = coverUri;
    url.replace(QStringLiteral("%%"), QStringLiteral("200x200"));
    url.replace(QStringLiteral("%25%25"), QStringLiteral("200x200"));

    if (url.startsWith(QStringLiteral("http://")) ||
        url.startsWith(QStringLiteral("https://")))
    {
        return url;
    }

    return QStringLiteral("https://") + url;
}

void MediaControlsMacOS::startArtworkFetch(const QString &coverUri)
{
    const QString url = fullCoverUrl(coverUri);
    if (url.isEmpty())
    {
        m_pendingArtworkUri.clear();
        m_artworkImage = {};
        updateNowPlayingInfo();
        return;
    }

    m_pendingArtworkUri = coverUri;

    /*
     * Local file (cached cover) – load directly.
     * QNetworkAccessManager does not handle file:// reliably.
     */

    if (url.startsWith(QStringLiteral("file://")))
    {
        QImage image(QUrl(url).toLocalFile());
        if (!image.isNull())
        {
            m_artworkImage = std::move(image);
        }
        updateNowPlayingInfo();
        return;
    }

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QStringLiteral("YaMusic/1.0 (Qt)"));

    QNetworkReply *reply = m_network->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        handleArtworkReply(reply, m_pendingArtworkUri);
    });
}

void MediaControlsMacOS::handleArtworkReply(QNetworkReply *reply,
                                            const QString &requestedUri)
{
    // Если уже переключили трек — игнорируем ответ
    if (requestedUri != m_metadata.coverUrl)
        return;

    if (reply->error() != QNetworkReply::NoError)
    {
        m_artworkImage = {};
        updateNowPlayingInfo();
        return;
    }

    const QByteArray data = reply->readAll();
    QImage image;
    if (!image.loadFromData(data))
    {
        m_artworkImage = {};
        updateNowPlayingInfo();
        return;
    }

    m_artworkImage = std::move(image);
    updateNowPlayingInfo();
}