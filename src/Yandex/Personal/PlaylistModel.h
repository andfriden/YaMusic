#pragma once

#include <QHash>
#include <QString>
#include <QVariant>
#include "../../Models/Playlist.h"
#include "../Catalog/TrackListModelBase.h"

class PlaylistModel final : public TrackListModelBase
{
    Q_OBJECT

public:

    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistRole,
        ArtistIdRole,
        AlbumRole,
        AlbumIdRole,
        CoverUriRole,
        DurationMsRole,
        LikedRole
    };

    explicit PlaylistModel(
        QObject *parent = nullptr);

    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray>
    roleNames() const override;

    void setPlaylist(
        const Playlist &playlist);

    void clear();

    QString title() const;

    int trackCount() const;

    void setTrackLiked(
        const QString &trackId,
        bool liked);

private:

    Playlist m_playlist;
};