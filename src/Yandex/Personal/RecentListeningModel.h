#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "../../Models/Track.h"

class RecentListeningModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistRole,
        ArtistIdRole,
        AlbumRole,
        AlbumIdRole,
        CoverUriRole,
        DurationMsRole
    };

    explicit RecentListeningModel(
        QObject *parent = nullptr);

    int rowCount(
        const QModelIndex &parent =
            QModelIndex()) const override;

    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray>
    roleNames() const override;

    void setTracks(
        const QList<Track> &tracks);

    void clear();

    Track trackAt(
        int index) const;

    int count() const;

private:
    QList<Track> m_tracks;
};