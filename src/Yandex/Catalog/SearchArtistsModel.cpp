#include "SearchArtistsModel.h"

SearchArtistsModel::SearchArtistsModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}

int SearchArtistsModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_artists.size();
}

QVariant SearchArtistsModel::data(
    const QModelIndex &index,
    int role) const
{
    if (
        !index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_artists.size()
    ) {
        return {};
    }

    const Artist &artist =
        m_artists.at(
            index.row());

    switch (role) {

        case IdRole:
            return artist.id;

        case NameRole:
            return artist.name;

        case CoverUriRole:
            return artist.coverUri;

        default:
            return {};
    }
}

QHash<int, QByteArray>
SearchArtistsModel::roleNames() const
{
    return {
        {IdRole, "artistId"},
        {NameRole, "name"},
        {CoverUriRole, "coverUri"}
    };
}

void SearchArtistsModel::setArtists(
    const QList<Artist> &artists)
{
    beginResetModel();

    m_artists =
        artists;

    endResetModel();
}


void SearchArtistsModel::appendArtists(
    const QList<Artist> &artists)
{
    if (artists.isEmpty()) {
        return;
    }

    beginInsertRows(
        QModelIndex(),
        m_artists.size(),
        m_artists.size() + artists.size() - 1);

    m_artists.append(
        artists);

    endInsertRows();
}


void SearchArtistsModel::clear()
{
    beginResetModel();

    m_artists.clear();

    endResetModel();
}

Artist SearchArtistsModel::artistAt(
    int index) const
{
    if (
        index < 0 ||
        index >= m_artists.size()
    ) {
        return {};
    }

    return m_artists.at(
        index);
}

int SearchArtistsModel::count() const
{
    return m_artists.size();
}