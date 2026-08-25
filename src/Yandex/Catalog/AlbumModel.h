#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QString>
#include <QVariant>

#include "AlbumService.h"

class AlbumModel final : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(
        int count
        READ count
        NOTIFY countChanged)

    Q_PROPERTY(
        QString title
        READ title
        NOTIFY albumChanged)

    Q_PROPERTY(
        QString coverUri
        READ coverUri
        NOTIFY albumChanged)

    Q_PROPERTY(
        int trackCount
        READ trackCount
        NOTIFY albumChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistRole,
        ArtistIdRole,
        CoverUriRole,
        DurationMsRole
    };

    Q_ENUM(Roles)

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

    QList<Track> tracks() const;

    int count() const;

    QString title() const;

    QString coverUri() const;

    int trackCount() const;

    signals:
        void countChanged();
    void albumChanged();

private:
    AlbumDetails m_album;
    QList<Track> m_tracks;
};