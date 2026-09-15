#include "MediaControlsSMTC.h"

#ifdef Q_OS_WIN

#ifndef YAMUSIC_HAS_CPPWINRT

/*
 * C++/WinRT unavailable (MinGW) — no-op stubs.
 * vtable is emitted here so the linker doesn't fail.
 */

MediaControlsSMTC::MediaControlsSMTC(QObject *parent)
    : SystemMediaControls(parent) {}
MediaControlsSMTC::~MediaControlsSMTC() = default;
void MediaControlsSMTC::platformSetEnabled(bool) {}
void MediaControlsSMTC::platformSetMetadata(const Metadata &) {}
void MediaControlsSMTC::platformSetPlaybackStatus(PlaybackStatus) {}
void MediaControlsSMTC::platformSetPosition(qint64) {}
void MediaControlsSMTC::platformSetDuration(qint64) {}

#else

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.SystemMediaTransportControls.h>
#include <winrt/Windows.Storage.Streams.h>

#include <QDebug>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Media;
using namespace Windows::Media::SystemMediaTransportControls;

/*
 * Windows SMTC через C++/WinRT.
 *
 * Требуется:
 *   - Windows 10 1803+
 *   - флаг компилятора /std:c++20
 *   - link: WindowsApp.lib (или use_winrt)
 */

class MediaControlsSMTC::Impl
{
public:
    Impl()
    {
        m_controls = SystemMediaTransportControls::GetForCurrentView();
    }

    void connectCommands(MediaControlsSMTC *owner)
    {
        if (m_tokenSet) return;

        m_controls.IsPlayEnabled(true);
        m_controls.IsPauseEnabled(true);
        m_controls.IsNextEnabled(true);
        m_controls.IsPreviousEnabled(true);
        m_controls.IsStopEnabled(true);
        m_controls.IsEnabled(true);

        m_buttonToken = m_controls.ButtonPressed(
            [owner](const SystemMediaTransportControls &,
                    const SystemMediaTransportControlsButtonPressedEventArgs &args)
            {
                switch (args.Button())
                {
                case SystemMediaTransportControlsButton::Play:
                    QMetaObject::invokeMethod(owner, [owner]() {
                        Q_EMIT owner->playRequested();
                    });
                    break;
                case SystemMediaTransportControlsButton::Pause:
                    QMetaObject::invokeMethod(owner, [owner]() {
                        Q_EMIT owner->pauseRequested();
                    });
                    break;
                case SystemMediaTransportControlsButton::Next:
                    QMetaObject::invokeMethod(owner, [owner]() {
                        Q_EMIT owner->nextRequested();
                    });
                    break;
                case SystemMediaTransportControlsButton::Previous:
                    QMetaObject::invokeMethod(owner, [owner]() {
                        Q_EMIT owner->previousRequested();
                    });
                    break;
                case SystemMediaTransportControlsButton::Stop:
                    QMetaObject::invokeMethod(owner, [owner]() {
                        Q_EMIT owner->pauseRequested();
                    });
                    break;
                }
            });

        m_positionToken = m_controls.PlaybackPositionChangeRequested(
            [owner](const SystemMediaTransportControls &,
                    const PlaybackPositionChangeRequestedEventArgs &args)
            {
                const auto pos = args.RequestedPlaybackPosition();
                const qint64 ms = pos.duration() / 10000; // 100ns → ms
                QMetaObject::invokeMethod(owner, [owner, ms]() {
                    Q_EMIT owner->seekRequested(ms);
                });
            });

        m_tokenSet = true;
    }

    void disconnectCommands()
    {
        if (!m_tokenSet) return;
        m_controls.ButtonPressed(m_buttonToken);
        m_controls.PlaybackPositionChangeRequested(m_positionToken);
        m_controls.IsEnabled(false);
        m_tokenSet = false;
    }

    void updateInfo(const SystemMediaControls::Metadata &md,
                    SystemMediaControls::PlaybackStatus status,
                    qint64 positionMs,
                    qint64 durationMs)
    {
        auto updater = m_controls.Updater();

        // Тип медиа — музыка
        updater.MediaProperties().MusicProperties().Title(
            winrt::to_hstring(md.title.toStdWString()));
        updater.MediaProperties().MusicProperties().Artist(
            winrt::to_hstring(md.artist.toStdWString()));
        updater.MediaProperties().MusicProperties().AlbumTitle(
            winrt::to_hstring(md.album.toStdWString()));
        updater.MediaProperties().MusicProperties().AlbumArtist(
            winrt::to_hstring(md.artist.toStdWString()));

        updater.MediaProperties().Type(MediaPlaybackType::Music);

        // Длительность
        if (durationMs > 0)
        {
            updater.MediaProperties().Duration(
                std::chrono::milliseconds(durationMs));
        }

        // Статус
        switch (status)
        {
        case SystemMediaControls::PlaybackStatus::Playing:
            updater.Status(MediaPlaybackStatus::Playing);
            break;
        case SystemMediaControls::PlaybackStatus::Paused:
            updater.Status(MediaPlaybackStatus::Paused);
            break;
        case SystemMediaControls::PlaybackStatus::Stopped:
            updater.Status(MediaPlaybackStatus::Stopped);
            break;
        }

        updater.Update();
    }

private:
    SystemMediaTransportControls m_controls = nullptr;
    event_token m_buttonToken;
    event_token m_positionToken;
    bool m_tokenSet = false;
};

// ---- C++ интерфейс ----

MediaControlsSMTC::MediaControlsSMTC(QObject *parent)
    : SystemMediaControls(parent)
    , d(new Impl())
{
}

MediaControlsSMTC::~MediaControlsSMTC()
{
    setEnabled(false);
    delete d;
}

void MediaControlsSMTC::platformSetEnabled(bool enabled)
{
    if (enabled)
        d->connectCommands(this);
    else
        d->disconnectCommands();
}

void MediaControlsSMTC::platformSetMetadata(const Metadata &)
{
    d->updateInfo(m_metadata, m_status, m_positionMs, m_durationMs);
}

void MediaControlsSMTC::platformSetPlaybackStatus(PlaybackStatus)
{
    d->updateInfo(m_metadata, m_status, m_positionMs, m_durationMs);
}

void MediaControlsSMTC::platformSetPosition(qint64)
{
    d->updateInfo(m_metadata, m_status, m_positionMs, m_durationMs);
}

void MediaControlsSMTC::platformSetDuration(qint64)
{
    d->updateInfo(m_metadata, m_status, m_positionMs, m_durationMs);
}

#endif // YAMUSIC_HAS_CPPWINRT

#endif // Q_OS_WIN