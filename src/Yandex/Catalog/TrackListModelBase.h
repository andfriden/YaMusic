#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QByteArray>
#include <QVariant>
#include "../../Models/Track.h"

class TrackListModelBase : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(
        int count
        READ count
        NOTIFY countChanged)

public:

    using QAbstractListModel::QAbstractListModel;

    int rowCount(
        const QModelIndex &parent =
            QModelIndex()) const override;

    /*
     * Access a single track by index.
     * Returns a default-constructed Track if out of range.
     */

    Track trackAt(
        int index) const;

    /*
     * All tracks (copy).
     */

    QList<Track> tracks() const;

    /*
     * Convenience property.
     */

    int count() const;

    /*
     * Remove all tracks and signal the UI.
     */

    void clear();

    /*
     * Replace all tracks with a new list.
     * Must be called from derived-class methods
     * that handle beginResetModel / endResetModel.
     */

    void setTracks(
        const QList<Track> &tracks);

    /*
     * Find a track by id and set its `liked` flag.
     * Emits dataChanged on the matching row.
     * Returns true if the track was found and changed.
     */

    bool setTrackLiked(
        const QString &trackId,
        bool liked,
        int likedRole);

    /*
     * Remove the first track with matching id.
     * Emits rowsRemoved. Returns true if removed.
     */

    bool removeTrackById(
        const QString &trackId);

signals:

    void countChanged();

protected:

    QList<Track> m_tracks;
};