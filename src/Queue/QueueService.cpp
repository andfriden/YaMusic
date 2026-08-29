#include "QueueService.h"

#include <QRandomGenerator>


QueueService::QueueService(
    QObject *parent)
    : QObject(parent)
{
}


// =============================================================
// Queue
// =============================================================

int
QueueService::count() const
{
    return m_tracks.size();
}


int
QueueService::currentIndex() const
{
    return m_currentIndex;
}


Track
QueueService::currentTrack() const
{
    if (
        m_currentIndex < 0 ||
        m_currentIndex >= m_tracks.size()
    ) {
        return {};
    }

    return m_tracks.at(
        m_currentIndex);
}


Track
QueueService::trackAt(
    int index) const
{
    if (
        index < 0 ||
        index >= m_tracks.size()
    ) {
        return {};
    }

    return m_tracks.at(
        index);
}


QList<Track>
QueueService::tracks() const
{
    return m_tracks;
}


// =============================================================
// Source
// =============================================================

QString
QueueService::sourceTitle() const
{
    return m_sourceTitle;
}


QString
QueueService::sourceType() const
{
    return m_sourceType;
}


void
QueueService::setSource(
    const QString &title,
    const QString &type)
{
    const QString trimmedTitle =
        title.trimmed();

    const QString trimmedType =
        type.trimmed();

    if (
        m_sourceTitle ==
            trimmedTitle &&
        m_sourceType ==
            trimmedType
    ) {
        return;
    }

    m_sourceTitle =
        trimmedTitle;

    m_sourceType =
        trimmedType;

    emit queueChanged();
}


void
QueueService::clearSource()
{
    if (
        m_sourceTitle.isEmpty() &&
        m_sourceType.isEmpty()
    ) {
        return;
    }

    m_sourceTitle.clear();

    m_sourceType.clear();

    emit queueChanged();
}


// =============================================================
// Modification
// =============================================================

void
QueueService::addTrack(
    const Track &track)
{
    if (
        track.id.isEmpty()
    ) {
        return;
    }

    m_originalTracks.append(
        track);

    if (
        !m_shuffleEnabled
    ) {
        m_tracks.append(
            track);
    } else {

        m_tracks.append(
            track);

        rebuildShuffledQueue();
    }

    if (
        m_currentIndex < 0
    ) {
        m_currentIndex =
            0;

        emit currentChanged();
    }

    emit queueChanged();
}


void
QueueService::addTracks(
    const QList<Track> &tracks)
{
    QList<Track> validTracks;

    for (
        const Track &track :
        tracks
    ) {

        if (
            !track.id.isEmpty()
        ) {
            validTracks.append(
                track);
        }
    }

    if (
        validTracks.isEmpty()
    ) {
        return;
    }

    m_originalTracks =
        validTracks;

    if (
        !m_shuffleEnabled
    ) {

        m_tracks =
            validTracks;

        if (
            m_currentIndex < 0
        ) {
            m_currentIndex =
                0;
        } else {

            m_currentIndex =
                qBound(
                    0,
                    m_currentIndex,
                    m_tracks.size() - 1);
        }

    } else {

        const QString currentId =
            currentTrack().id;

        m_tracks =
            validTracks;

        rebuildShuffledQueue();

        m_currentIndex =
            0;

        if (
            !currentId.isEmpty()
        ) {

            for (
                int i = 0;
                i < m_tracks.size();
                ++i
            ) {

                if (
                    m_tracks.at(i).id ==
                    currentId
                ) {

                    m_currentIndex =
                        i;

                    break;
                }
            }
        }
    }

    if (
        m_currentIndex < 0 &&
        !m_tracks.isEmpty()
    ) {
        m_currentIndex =
            0;
    }

    emit currentChanged();

    emit queueChanged();
}


void
QueueService::removeTrack(
    int index)
{
    if (
        index < 0 ||
        index >= m_tracks.size()
    ) {
        return;
    }

    const QString trackId =
        m_tracks
            .at(index)
            .id;

    m_tracks.removeAt(
        index);

    for (
        int i = 0;
        i < m_originalTracks.size();
        ++i
    ) {

        if (
            m_originalTracks
                .at(i)
                .id ==
            trackId
        ) {

            m_originalTracks
                .removeAt(i);

            break;
        }
    }

    if (
        m_tracks.isEmpty()
    ) {

        m_currentIndex =
            -1;

        emit currentChanged();

        emit queueChanged();

        return;
    }

    if (
        index < m_currentIndex
    ) {

        --m_currentIndex;

        emit currentChanged();

    } else if (
        index == m_currentIndex &&
        m_currentIndex >=
            m_tracks.size()
    ) {

        m_currentIndex =
            m_tracks.size() - 1;

        emit currentChanged();
    }

    emit queueChanged();
}


void
QueueService::moveTrack(
    int from,
    int to)
{
    if (
        from < 0 ||
        from >= m_tracks.size() ||
        to < 0 ||
        to >= m_tracks.size() ||
        from == to
    ) {
        return;
    }

    const Track track =
        m_tracks.takeAt(
            from);

    m_tracks.insert(
        to,
        track);

    if (
        !m_shuffleEnabled
    ) {
        m_originalTracks =
            m_tracks;
    }

    if (
        m_currentIndex ==
        from
    ) {

        m_currentIndex =
            to;

        emit currentChanged();

    } else if (
        from < m_currentIndex &&
        to >= m_currentIndex
    ) {

        --m_currentIndex;

        emit currentChanged();

    } else if (
        from > m_currentIndex &&
        to <= m_currentIndex
    ) {

        ++m_currentIndex;

        emit currentChanged();
    }

    emit queueChanged();
}


void
QueueService::clear()
{
    const bool hadQueue =
        !m_tracks.isEmpty() ||
        !m_originalTracks.isEmpty() ||
        m_currentIndex != -1;

    m_tracks.clear();

    m_originalTracks.clear();

    m_currentIndex =
        -1;

    if (
        hadQueue
    ) {
        emit currentChanged();

        emit queueChanged();
    }
}


// =============================================================
// Navigation
// =============================================================

bool
QueueService::setCurrentIndex(
    int index)
{
    if (
        index < 0 ||
        index >= m_tracks.size()
    ) {
        return false;
    }

    if (
        m_currentIndex ==
        index
    ) {
        return true;
    }

    m_currentIndex =
        index;

    emit currentChanged();

    return true;
}


bool
QueueService::next()
{
    if (
        !hasNext()
    ) {
        return false;
    }

    ++m_currentIndex;

    emit currentChanged();

    return true;
}


bool
QueueService::previous()
{
    if (
        !hasPrevious()
    ) {
        return false;
    }

    --m_currentIndex;

    emit currentChanged();

    return true;
}


bool
QueueService::hasNext() const
{
    return
        m_currentIndex >= 0 &&
        m_currentIndex + 1 <
            m_tracks.size();
}


bool
QueueService::hasPrevious() const
{
    return
        m_currentIndex > 0 &&
        m_currentIndex <
            m_tracks.size();
}


// =============================================================
// Repeat
// =============================================================

QueueService::RepeatMode
QueueService::repeatMode() const
{
    return m_repeatMode;
}


void
QueueService::setRepeatMode(
    RepeatMode mode)
{
    if (
        m_repeatMode ==
        mode
    ) {
        return;
    }

    m_repeatMode =
        mode;

    emit repeatModeChanged();
}


void
QueueService::cycleRepeatMode()
{
    switch (
        m_repeatMode
    ) {

    case RepeatOff:

        setRepeatMode(
            RepeatAll);

        break;


    case RepeatAll:

        setRepeatMode(
            RepeatOne);

        break;


    case RepeatOne:

        setRepeatMode(
            RepeatOff);

        break;
    }
}


// =============================================================
// Shuffle
// =============================================================

bool
QueueService::shuffleEnabled() const
{
    return m_shuffleEnabled;
}


void
QueueService::setShuffleEnabled(
    bool enabled)
{
    if (
        m_shuffleEnabled ==
        enabled
    ) {
        return;
    }

    const QString currentId =
        currentTrack().id;

    if (
        enabled
    ) {

        m_originalTracks =
            m_tracks;

        m_shuffleEnabled =
            true;

        rebuildShuffledQueue();

    } else {

        m_shuffleEnabled =
            false;

        m_tracks =
            m_originalTracks;
    }

    m_currentIndex =
        -1;

    if (
        !currentId.isEmpty()
    ) {

        for (
            int i = 0;
            i < m_tracks.size();
            ++i
        ) {

            if (
                m_tracks.at(i).id ==
                currentId
            ) {

                m_currentIndex =
                    i;

                break;
            }
        }
    }

    if (
        m_currentIndex < 0 &&
        !m_tracks.isEmpty()
    ) {
        m_currentIndex =
            0;
    }

    emit shuffleChanged();

    emit currentChanged();

    emit queueChanged();
}


void
QueueService::toggleShuffle()
{
    setShuffleEnabled(
        !m_shuffleEnabled);
}


// =============================================================
// Shuffle rebuild
// =============================================================

void
QueueService::rebuildShuffledQueue()
{
    if (
        m_tracks.size() < 2
    ) {
        return;
    }

    const QString currentId =
        currentTrack().id;

    QList<Track> shuffled =
        m_originalTracks;

    for (
        int i = shuffled.size() - 1;
        i > 0;
        --i
    ) {

        const int j =
            QRandomGenerator::global()
                ->bounded(
                    i + 1);

        shuffled.swapItemsAt(
            i,
            j);
    }

    m_tracks =
        shuffled;

    /*
     * При включении Shuffle
     * текущий трек сохраняется
     * первым в очереди.
     */

    if (
        !currentId.isEmpty()
    ) {

        for (
            int i = 0;
            i < m_tracks.size();
            ++i
        ) {

            if (
                m_tracks.at(i).id ==
                currentId
            ) {

                m_tracks.swapItemsAt(
                    0,
                    i);

                break;
            }
        }
    }
}