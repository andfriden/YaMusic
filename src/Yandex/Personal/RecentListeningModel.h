#pragma once

#include <QHash>
#include <QList>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include "../../Models/Track.h"
#include "../Catalog/TrackListModelBase.h"

class RecentListeningModel final : public TrackListModelBase
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

    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray>
    roleNames() const override;

    void setTracks(
        const QList<Track> &tracks);

    Q_INVOKABLE QVariantMap trackDataAt(
        int index) const;

    Q_INVOKABLE QVariantList randomTrackData(
        int limit) const;

private:
    QVariantMap trackToMap(
        const Track &track,
        int sourceIndex) const;
};