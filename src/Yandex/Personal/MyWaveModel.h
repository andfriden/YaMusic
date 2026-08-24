#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "../../Models/Track.h"

class MyWaveModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(
        int count
        READ count
        NOTIFY countChanged)

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

    explicit MyWaveModel(
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

    void setTracks(
        const QList<Track> &tracks);

    void appendTracks(
        const QList<Track> &tracks);

    void clear();

    Track trackAt(
        int index) const;

    Track lastTrack() const;

    QList<Track> tracks() const;

    int count() const;

    signals:
        void countChanged();

private:
    QList<Track> m_tracks;
};