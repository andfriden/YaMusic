#pragma once

#include "TrackListModelBase.h"

class SimilarTracksModel : public TrackListModelBase
{
    Q_OBJECT

public:
    enum Roles {
        SourceIndexRole = Qt::UserRole + 1,
        TrackIdRole,
        TitleRole,
        ArtistRole,
        ArtistIdRole,
        CoverUriRole,
        DurationMsRole
    };

    explicit SimilarTracksModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setSimilarTracks(const QList<Track> &tracks);
};