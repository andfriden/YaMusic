#pragma once

#include <QHash>
#include <QString>
#include <QVariant>
#include "ArtistService.h"
#include "TrackListModelBase.h"

class ArtistModel final : public TrackListModelBase
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

    explicit ArtistModel(
        QObject *parent = nullptr);

    QVariant data(
        const QModelIndex &index,
        int role =
            Qt::DisplayRole) const override;

    QHash<int, QByteArray>
    roleNames() const override;

    void setArtist(
        const ArtistDetails &artist);

    void clear();

    QString id() const;

    QString name() const;

    QString coverUri() const;

    QString description() const;

    QString genres() const;

private:
    ArtistDetails m_artist;
};