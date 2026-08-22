#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "ArtistService.h"

class ArtistModel : public QAbstractListModel
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

    int rowCount(
        const QModelIndex &parent =
            QModelIndex()) const override;

    QVariant data(
        const QModelIndex &index,
        int role =
            Qt::DisplayRole) const override;

    QHash<int, QByteArray>
    roleNames() const override;

    void setArtist(
        const ArtistDetails &artist);

    void clear();

    Track trackAt(
        int index) const;

    int count() const;

    QString id() const;

    QString name() const;

    QString coverUri() const;

    QString description() const;

    QString genres() const;

private:
    ArtistDetails m_artist;

    QList<Track> m_tracks;
};