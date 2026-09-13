#pragma once

#include <QHash>
#include <QList>
#include <QVariant>
#include "../../Models/Track.h"
#include "../Catalog/TrackListModelBase.h"

class MyWaveModel final : public TrackListModelBase
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

    explicit MyWaveModel(
        QObject *parent = nullptr);

    QVariant data(
        const QModelIndex &index,
        int role =
            Qt::DisplayRole) const override;

    QHash<int, QByteArray>
    roleNames() const override;

    void setTracks(
        const QList<Track> &tracks);

    void appendTracks(
        const QList<Track> &tracks);

    Track lastTrack() const;

    signals:
        void countChanged();

private:
};