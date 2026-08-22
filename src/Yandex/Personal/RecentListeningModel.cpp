#include "RecentListeningModel.h"

RecentListeningModel::RecentListeningModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}

int RecentListeningModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_items.size();
}

QVariant RecentListeningModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_items.size()) {

        return {};
        }

    const RecentListeningItem &item =
        m_items.at(index.row());

    switch (role) {

        case IdRole:
            return item.id;

        case TypeRole:
            return item.type;

        case TitleRole:
            return item.title;

        case SubtitleRole:
            return item.subtitle;

        case CoverUriRole:
            return item.coverUri;

        case ContextRole:
            return item.context;

        case ContextItemRole:
            return item.contextItem;

        case UidRole:
            return item.uid;

        case KindRole:
            return item.kind;

        default:
            return {};
    }
}

QHash<int, QByteArray>
RecentListeningModel::roleNames() const
{
    return {
        {IdRole, "itemId"},
        {TypeRole, "itemType"},
        {TitleRole, "title"},
        {SubtitleRole, "subtitle"},
        {CoverUriRole, "coverUri"},
        {ContextRole, "context"},
        {ContextItemRole, "contextItem"},
        {UidRole, "uid"},
        {KindRole, "kind"}
    };
}

void RecentListeningModel::setItems(
    const QList<RecentListeningItem> &items)
{
    beginResetModel();

    m_items = items;

    endResetModel();
}

void RecentListeningModel::clear()
{
    beginResetModel();

    m_items.clear();

    endResetModel();
}

RecentListeningItem
RecentListeningModel::itemAt(
    int index) const
{
    if (index < 0 ||
        index >= m_items.size()) {

        return {};
        }

    return m_items.at(index);
}

int RecentListeningModel::count() const
{
    return m_items.size();
}