#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QVariant>

#include "../../Models/Track.h"


class LikedTracksModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Roles
    {
        TrackIdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistRole,
        ArtistIdRole,
        AlbumRole,
        AlbumIdRole,
        CoverUriRole,
        DurationMsRole
    };

    Q_ENUM(Roles)


    explicit LikedTracksModel(
        QObject *parent = nullptr);


    int rowCount(
        const QModelIndex &parent =
            QModelIndex()) const override;


    QVariant data(
        const QModelIndex &index,
        int role =
            Qt::DisplayRole) const override;


    QHash<int, QByteArray>
    roleNames() const override;


    // =============================================================
    // Data
    // =============================================================

    void setTracks(
        const QList<Track> &tracks);


    void clear();


    QList<Track> tracks() const;


    Track trackAt(
        int index) const;


    int count() const;


private:

    QList<Track> m_tracks;
};