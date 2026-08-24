#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "../../Models/Track.h"

class SimilarArtistsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        CoverUriRole
    };

    explicit SimilarArtistsModel(
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

    void setArtists(
        const QList<Artist> &artists);

    void clear();

    Artist artistAt(
        int index) const;

    QList<Artist> artists() const;

    int count() const;

private:
    QList<Artist> m_artists;
};