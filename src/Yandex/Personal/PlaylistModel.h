#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "../../Models/Playlist.h"

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistRole,
        ArtistIdRole,
        AlbumRole,
        CoverUriRole,
        DurationMsRole
    };

    explicit PlaylistModel(
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

    void setPlaylist(
        const Playlist &playlist);

    void clear();

    Track trackAt(
        int index) const;

    QList<Track> tracks() const;

    int count() const;

    QString title() const;

    int trackCount() const;

private:
    Playlist m_playlist;

    QList<Track> m_tracks;
};