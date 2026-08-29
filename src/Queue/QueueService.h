#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "../Models/Track.h"

class QueueService : public QObject
{
    Q_OBJECT

public:

    enum RepeatMode {
        RepeatOff = 0,
        RepeatOne,
        RepeatAll
    };

    Q_ENUM(RepeatMode)


    explicit QueueService(
        QObject *parent = nullptr);


    // =============================================================
    // Queue
    // =============================================================

    int count() const;

    int currentIndex() const;

    Track currentTrack() const;

    Track trackAt(
        int index) const;

    QList<Track> tracks() const;


    // =============================================================
    // Source
    // =============================================================

    QString sourceTitle() const;

    QString sourceType() const;

    void setSource(
        const QString &title,
        const QString &type);

    void clearSource();


    // =============================================================
    // Modification
    // =============================================================

    void addTrack(
        const Track &track);

    void addTracks(
        const QList<Track> &tracks);

    void removeTrack(
        int index);

    void moveTrack(
        int from,
        int to);

    void clear();


    // =============================================================
    // Navigation
    // =============================================================

    bool setCurrentIndex(
        int index);

    bool next();

    bool previous();

    bool hasNext() const;

    bool hasPrevious() const;


    // =============================================================
    // Repeat
    // =============================================================

    RepeatMode repeatMode() const;

    void setRepeatMode(
        RepeatMode mode);

    void cycleRepeatMode();


    // =============================================================
    // Shuffle
    // =============================================================

    bool shuffleEnabled() const;

    void setShuffleEnabled(
        bool enabled);

    void toggleShuffle();


signals:

    void queueChanged();

    void currentChanged();

    void repeatModeChanged();

    void shuffleChanged();


private:

    void rebuildShuffledQueue();


private:

    // =============================================================
    // Tracks
    // =============================================================

    QList<Track> m_tracks;

    QList<Track> m_originalTracks;

    int m_currentIndex = -1;


    // =============================================================
    // Source
    // =============================================================

    QString m_sourceTitle;

    QString m_sourceType;


    // =============================================================
    // Playback options
    // =============================================================

    RepeatMode m_repeatMode =
        RepeatOff;

    bool m_shuffleEnabled =
        false;
};