#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "AlbumService.h"

class AlbumModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistRole,
        ArtistIdRole,
        CoverUriRole,
        DurationMsRole
    };

    explicit AlbumModel(
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

    void setAlbum(
        const AlbumDetails &album);

    void clear();

    Track trackAt(
        int index) const;

    int count() const;

    QString title() const;

    QString coverUri() const;

    int trackCount() const;

private:
    AlbumDetails m_album;

    QList<Track> m_tracks;
};