#pragma once

#include "SystemMediaControls.h"

/*
 * macOS: MPRemoteCommandCenter + MPNowPlayingInfoCenter.
 *
 * Публикует метаданные в Now Playing Info Center и
 * обрабатывает команды из Control Center / Touch Bar / клавиш.
 */

class MediaControlsMacOS : public SystemMediaControls
{
    Q_OBJECT

public:
    explicit MediaControlsMacOS(QObject *parent = nullptr);
    ~MediaControlsMacOS() override;

protected:
    void platformSetEnabled(bool enabled) override;
    void platformSetMetadata(const Metadata &metadata) override;
    void platformSetPlaybackStatus(PlaybackStatus status) override;
    void platformSetPosition(qint64 positionMs) override;
void platformSetDuration(qint64 durationMs) override;

private:
    void updateNowPlayingInfo();

    class Impl;
    Impl *d = nullptr;
};