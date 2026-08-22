#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "../../Models/RecentListeningItem.h"

class RecentListeningModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        TitleRole,
        SubtitleRole,
        CoverUriRole,
        ContextRole,
        ContextItemRole,
        UidRole,
        KindRole
    };

    explicit RecentListeningModel(
        QObject *parent = nullptr);

    int rowCount(
        const QModelIndex &parent =
            QModelIndex()) const override;

    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray>
    roleNames() const override;

    void setItems(
        const QList<RecentListeningItem> &items);

    void clear();

    RecentListeningItem itemAt(
        int index) const;

    int count() const;

private:
    QList<RecentListeningItem> m_items;
};