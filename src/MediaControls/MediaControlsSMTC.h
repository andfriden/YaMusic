#pragma once

#include "SystemMediaControls.h"

#ifdef YAMUSIC_HAS_CPPWINRT

/*
 * Windows: System Media Transport Controls (SMTC).
 *
 * Использует Windows.Media.MediaExtension через
 * SystemMediaTransportControls из WinRT/C++.
 */

class MediaControlsSMTC : public SystemMediaControls
{
    Q_OBJECT

public:
    explicit MediaControlsSMTC(QObject *parent = nullptr);
    ~MediaControlsSMTC() override;

protected:
    void platformSetEnabled(bool enabled) override;
    void platformSetMetadata(const Metadata &metadata) override;
    void platformSetPlaybackStatus(PlaybackStatus status) override;
    void platformSetPosition(qint64 positionMs) override;
    void platformSetDuration(qint64 durationMs) override;

private:
    class Impl;
    Impl *d = nullptr;
};

#else

/*
 * Windows only — no-op stub used when C++/WinRT is unavailable
 * (e.g. MinGW). Satisfies the interface without including
 * winrt/*.h headers.
 */
class MediaControlsSMTC : public SystemMediaControls
{
    Q_OBJECT

public:
    explicit MediaControlsSMTC(QObject *parent = nullptr)
        : SystemMediaControls(parent) {}

protected:
    void platformSetEnabled(bool) override {}
    void platformSetMetadata(const Metadata &) override {}
    void platformSetPlaybackStatus(PlaybackStatus) override {}
    void platformSetPosition(qint64) override {}
    void platformSetDuration(qint64) override {}
};

#endif