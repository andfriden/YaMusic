#pragma once

#include <QHash>
#include <QList>
#include <QVariant>
#include "../../Models/Track.h"
#include "../Catalog/TrackListModelBase.h"

class LikedTracksModel final : public TrackListModelBase
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
        DurationMsRole,
        LikedRole
    };

    Q_ENUM(Roles)

    explicit LikedTracksModel(
        QObject *parent = nullptr);

    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void setTracks(
        const QList<Track> &tracks);

    void clear();

    void setTrackLiked(
        const QString &trackId,
        bool liked);

    void removeTrack(
        const QString &trackId);

private:
};