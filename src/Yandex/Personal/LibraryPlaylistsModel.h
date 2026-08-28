#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "../../Models/PersonalPlaylist.h"


class LibraryPlaylistsModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Roles {
        IdRole = Qt::UserRole + 1,
        UidRole,
        KindRole,
        TitleRole,
        DescriptionRole,
        CoverUriRole,
        TrackCountRole
    };

    Q_ENUM(Roles)


    explicit LibraryPlaylistsModel(
        QObject *parent = nullptr);


    int rowCount(
        const QModelIndex &parent =
            QModelIndex()) const override;


    QVariant data(
        const QModelIndex &index,
        int role =
            Qt::DisplayRole
    ) const override;


    QHash<int, QByteArray>
    roleNames() const override;


    void setPlaylists(
        const QList<PersonalPlaylist> &playlists);


    void clear();


    int count() const;


    PersonalPlaylist playlistAt(
        int index) const;


private:

    QList<PersonalPlaylist>
        m_playlists;
};