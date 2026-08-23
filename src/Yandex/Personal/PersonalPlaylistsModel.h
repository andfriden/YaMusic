#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "../../Models/PersonalPlaylist.h"

class PersonalPlaylistsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        UidRole = Qt::UserRole + 1,
        KindRole,
        TitleRole,
        DescriptionRole,
        CoverUriRole,
        TrackCountRole
    };

    explicit PersonalPlaylistsModel(
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

    void setPlaylists(
        const QList<PersonalPlaylist> &playlists);

    void clear();

    PersonalPlaylist playlistAt(
        int index) const;

    int count() const;

private:
    QList<PersonalPlaylist> m_playlists;
};