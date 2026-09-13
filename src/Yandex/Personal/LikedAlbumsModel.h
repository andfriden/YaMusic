#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QVariant>

#include "../../Models/Track.h"

class LikedAlbumsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        AlbumIdRole = Qt::UserRole + 1,
        TitleRole,
        CoverUriRole,
        YearRole
    };

    Q_ENUM(Roles)

    explicit LikedAlbumsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setAlbums(const QList<Album> &albums);
    void clear();

    Album albumAt(int index) const;
    int count() const;

signals:
    void countChanged();

private:
    QList<Album> m_albums;
};