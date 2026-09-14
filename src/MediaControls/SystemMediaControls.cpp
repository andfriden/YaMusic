#include "SystemMediaControls.h"

SystemMediaControls::SystemMediaControls(QObject *parent)
    : QObject(parent)
{
}

SystemMediaControls::~SystemMediaControls() = default;

void SystemMediaControls::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    platformSetEnabled(enabled);
}

bool SystemMediaControls::isEnabled() const
{
    return m_enabled;
}

void SystemMediaControls::setMetadata(const Metadata &metadata)
{
    m_metadata = metadata;
    platformSetMetadata(metadata);
}

void SystemMediaControls::setPlaybackStatus(PlaybackStatus status)
{
    m_status = status;
    platformSetPlaybackStatus(status);
}

void SystemMediaControls::setPosition(qint64 positionMs)
{
    m_positionMs = positionMs;
    platformSetPosition(positionMs);
}

void SystemMediaControls::setDuration(qint64 durationMs)
{
    m_durationMs = durationMs;
    platformSetDuration(durationMs);
}

void SystemMediaControls::setLoopStatus(const QString &loopStatus)
{
    m_loopStatus = loopStatus;
    platformSetLoopStatus(loopStatus);
}

void SystemMediaControls::setShuffle(bool enabled)
{
    m_shuffle = enabled;
    platformSetShuffle(enabled);
}

void SystemMediaControls::platformSetLoopStatus(const QString &)
{
}

void SystemMediaControls::platformSetShuffle(bool)
{
}