#pragma once

#include "SystemMediaControls.h"

/*
 * Linux: MPRISv2 via D-Bus (org.mpris.MediaPlayer2).
 *
 * Реализует интерфейсы org.mpris.MediaPlayer2 и
 * org.mpris.MediaPlayer2.Player на шине session.
 * Публикует метаданные, LoopStatus, Shuffle и
 * обрабатывает команды из любых MPRIS-клиентов
 * (KDE, GNOME, playerctl, etc.).
 */

class Impl;

class MediaControlsMPRIS : public SystemMediaControls
{
    Q_OBJECT

public:
    explicit MediaControlsMPRIS(QObject *parent = nullptr);
    ~MediaControlsMPRIS() override;

protected:
    void platformSetEnabled(bool enabled) override;
    void platformSetMetadata(const Metadata &metadata) override;
    void platformSetPlaybackStatus(PlaybackStatus status) override;
    void platformSetPosition(qint64 positionMs) override;
    void platformSetDuration(qint64 durationMs) override;
    void platformSetLoopStatus(const QString &loopStatus) override;
    void platformSetShuffle(bool enabled) override;

private:
    friend class Impl;
    Impl *d = nullptr;
};