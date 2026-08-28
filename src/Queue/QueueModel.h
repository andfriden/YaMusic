#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "../Models/Track.h"

class QueueService;

class QueueModel : public QAbstractListModel
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
        DurationMsRole,
        CurrentRole
    };

    explicit QueueModel(
        QueueService *queueService,
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


    Track trackAt(
        int index) const;


    QList<Track> tracks() const;


    int count() const;


    int currentIndex() const;


private:

    void reload();

    void updateCurrentIndex();


private:

    QueueService *m_queueService =
        nullptr;

    QList<Track> m_tracks;

    int m_currentIndex =
        -1;
};