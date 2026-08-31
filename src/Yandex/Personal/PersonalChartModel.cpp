#include "PersonalChartModel.h"


PersonalChartModel::PersonalChartModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


int PersonalChartModel::rowCount(
    const QModelIndex &parent) const
{
    if (
        parent.isValid()
    )
    {
        return 0;
    }

    return m_items.size();
}


QVariant PersonalChartModel::data(
    const QModelIndex &index,
    int role) const
{
    if (
        !index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_items.size()
    )
    {
        return {};
    }


    const PersonalChartItem &item =
        m_items.at(
            index.row());


    switch (role)
    {
    case IdRole:
        return item.id;

    case PositionRole:
        return item.position;

    case ListenersRole:
        return item.listeners;

    case ProgressRole:
        return item.progress;

    case ShiftRole:
        return item.shift;

    case TitleRole:
        return item.title;

    case ArtistsRole:
        return item.artists;

    case CoverUriRole:
        return item.coverUri;

    case DurationMsRole:
        return item.durationMs;

    default:
        return {};
    }
}


QHash<int, QByteArray>
PersonalChartModel::roleNames() const
{
    return {
        { IdRole,          "trackId" },
        { PositionRole,    "position" },
        { ListenersRole,   "listeners" },
        { ProgressRole,    "progress" },
        { ShiftRole,       "shift" },
        { TitleRole,       "title" },
        { ArtistsRole,     "artists" },
        { CoverUriRole,    "coverUri" },
        { DurationMsRole,  "durationMs" }
    };
}


void PersonalChartModel::setItems(
    const QList<PersonalChartItem> &items)
{
    beginResetModel();

    m_items =
        items;

    endResetModel();
}


void PersonalChartModel::clear()
{
    beginResetModel();

    m_items.clear();

    endResetModel();
}


QVariantMap PersonalChartModel::item(
    int index) const
{
    QVariantMap result;


    if (
        index < 0 ||
        index >= m_items.size()
    )
    {
        return result;
    }


    const PersonalChartItem &item =
        m_items.at(index);


    result["trackId"] =
        item.id;

    result["position"] =
        item.position;

    result["listeners"] =
        item.listeners;

    result["progress"] =
        item.progress;

    result["shift"] =
        item.shift;

    result["title"] =
        item.title;

    result["artists"] =
        item.artists;

    result["coverUri"] =
        item.coverUri;

    result["durationMs"] =
        item.durationMs;


    return result;
}