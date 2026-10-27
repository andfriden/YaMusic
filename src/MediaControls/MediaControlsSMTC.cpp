#include "MediaControlsSMTC.h"

#ifdef Q_OS_WIN

#ifndef YAMUSIC_HAS_CPPWINRT

// C++/WinRT недоступен (MinGW) — заглушки без логики.
// vtable нужен, чтобы линкер не падал.
MediaControlsSMTC::MediaControlsSMTC(QObject *parent) : SystemMediaControls(parent) {}
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
#include <winrt/Windows.Media.SystemMediaTransportControls.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Storage.Streams.h>

// GetForWindow для Win32-приложений: GetForCurrentView() рассчитан на UWP
// и в обычном desktop-окне не работает.
#include <systemmediatransportcontrolsinterop.h>

#include <QDebug>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Media;
using namespace Windows::Media::SystemMediaTransportControls;

class MediaControlsSMTC::Impl {
public:
  explicit Impl(MediaControlsSMTC *owner) : m_owner(owner) {}

  // SMTC создаётся для конкретного окна приложения. Окно появляется
  // после конструктора PlaybackController, поэтому создание ленивое:
  // первая попытка может не найти HWND, следующие — найдут.
  bool ensureControls() {
    if (m_controls) return true;

    const HWND hwnd = findAppWindow();
    if (hwnd == nullptr) return false;

    try {
      auto interop =
          winrt::get_activation_factory<SystemMediaTransportControls,
                                        ISystemMediaTransportControlsInterop>();

      SystemMediaTransportControls controls{nullptr};
      winrt::check_hresult(interop->GetForWindow(
          hwnd, winrt::guid_of<SystemMediaTransportControls>(),
          winrt::put_abi(controls)));

      m_controls = controls;
    } catch (const winrt::hresult_error &error) {
      qWarning("SMTC: cannot create controls (0x%08X)",
               static_cast<unsigned>(error.code()));
      return false;
    }

    return m_controls != nullptr;
  }

  // Идемпотентно: подключает обработчики, когда окно уже создано.
  // На старте окна может ещё не быть, поэтому вызывается не только из
  // platformSetEnabled, но и при каждом обновлении метаданных/статуса.
  void connectCommands() {
    if (m_tokenSet || !ensureControls()) return;

    MediaControlsSMTC *owner = m_owner;
    m_controls.IsPlayEnabled(true);
    m_controls.IsPauseEnabled(true);
    m_controls.IsNextEnabled(true);
    m_controls.IsPreviousEnabled(true);
    m_controls.IsStopEnabled(true);
    m_controls.IsEnabled(true);

    m_buttonToken = m_controls.ButtonPressed(
        [owner](const SystemMediaTransportControls &,
                const SystemMediaTransportControlsButtonPressedEventArgs &args) {
          switch (args.Button()) {
          case SystemMediaTransportControlsButton::Play:
            QMetaObject::invokeMethod(owner, [owner]() { Q_EMIT owner->playRequested(); });
            break;
          case SystemMediaTransportControlsButton::Pause:
            QMetaObject::invokeMethod(owner, [owner]() { Q_EMIT owner->pauseRequested(); });
            break;
          case SystemMediaTransportControlsButton::Next:
            QMetaObject::invokeMethod(owner, [owner]() { Q_EMIT owner->nextRequested(); });
            break;
          case SystemMediaTransportControlsButton::Previous:
            QMetaObject::invokeMethod(owner, [owner]() { Q_EMIT owner->previousRequested(); });
            break;
          case SystemMediaTransportControlsButton::Stop:
            QMetaObject::invokeMethod(owner, [owner]() { Q_EMIT owner->pauseRequested(); });
            break;
          }
        });

    m_positionToken = m_controls.PlaybackPositionChangeRequested(
        [owner](const SystemMediaTransportControls &,
                const PlaybackPositionChangeRequestedEventArgs &args) {
          const auto pos = args.RequestedPlaybackPosition();
          const qint64 ms = pos.duration() / 10000; // 100нс → мс
          QMetaObject::invokeMethod(owner, [owner, ms]() { Q_EMIT owner->seekRequested(ms); });
        });
    m_tokenSet = true;
  }

  void disconnectCommands() {
    if (!m_tokenSet || !m_controls) return;
    m_controls.ButtonPressed(m_buttonToken);
    m_controls.PlaybackPositionChangeRequested(m_positionToken);
    m_controls.IsEnabled(false);
    m_tokenSet = false;
  }

  void updateInfo(const SystemMediaControls::Metadata &md,
                  SystemMediaControls::PlaybackStatus status, qint64 positionMs,
                  qint64 durationMs) {
    if (!ensureControls()) return;

    // Подключаем кнопки, как только окно появилось (на старте его могло
    // не быть в platformSetEnabled). Повторные вызовы безопасны.
    connectCommands();

    auto updater = m_controls.Updater();

    updater.MediaProperties().MusicProperties().Title(winrt::to_hstring(md.title.toStdWString()));
    updater.MediaProperties().MusicProperties().Artist(winrt::to_hstring(md.artist.toStdWString()));
    updater.MediaProperties().MusicProperties().AlbumTitle(
        winrt::to_hstring(md.album.toStdWString()));
    updater.MediaProperties().MusicProperties().AlbumArtist(
        winrt::to_hstring(md.artist.toStdWString()));
    updater.MediaProperties().Type(MediaPlaybackType::Music);

    if (durationMs > 0) {
      updater.MediaProperties().Duration(std::chrono::milliseconds(durationMs));
    }

    switch (status) {
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
  // Окно приложения — первое видимое top-level окно потока GUI.
  static HWND findAppWindow() {
    HWND window = nullptr;

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND candidate, LPARAM param) -> BOOL {
          if (GetWindow(candidate, GW_OWNER) == nullptr &&
              IsWindowVisible(candidate)) {
            *reinterpret_cast<HWND *>(param) = candidate;
            return FALSE;
          }
          return TRUE;
        },
        reinterpret_cast<LPARAM>(&window));

    return window;
  }

  SystemMediaTransportControls m_controls = nullptr;
  event_token m_buttonToken;
  event_token m_positionToken;
  bool m_tokenSet = false;
  MediaControlsSMTC *m_owner = nullptr;
};

MediaControlsSMTC::MediaControlsSMTC(QObject *parent)
    : SystemMediaControls(parent), d(new Impl(this)) {}

MediaControlsSMTC::~MediaControlsSMTC() {
  setEnabled(false);
  delete d;
}

void MediaControlsSMTC::platformSetEnabled(bool enabled) {
  if (enabled)
    d->connectCommands();
  else
    d->disconnectCommands();
}

void MediaControlsSMTC::platformSetMetadata(const Metadata &) {
  d->updateInfo(m_metadata, m_status, m_positionMs, m_durationMs);
}

void MediaControlsSMTC::platformSetPlaybackStatus(PlaybackStatus) {
  d->updateInfo(m_metadata, m_status, m_positionMs, m_durationMs);
}

void MediaControlsSMTC::platformSetPosition(qint64) {
  d->updateInfo(m_metadata, m_status, m_positionMs, m_durationMs);
}

void MediaControlsSMTC::platformSetDuration(qint64) {
  d->updateInfo(m_metadata, m_status, m_positionMs, m_durationMs);
}

#endif // YAMUSIC_HAS_CPPWINRT

#endif // Q_OS_WIN