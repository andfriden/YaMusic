#include "SimilarArtistsModel.h"

SimilarArtistsModel::SimilarArtistsModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}

int SimilarArtistsModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_artists.size();
}

QVariant SimilarArtistsModel::data(
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
SimilarArtistsModel::roleNames() const
{
    return {
        {IdRole, "artistId"},
        {NameRole, "name"},
        {CoverUriRole, "coverUri"}
    };
}

void SimilarArtistsModel::setArtists(
    const QList<Artist> &artists)
{
    beginResetModel();

    m_artists =
        artists;

    endResetModel();
}

void SimilarArtistsModel::clear()
{
    beginResetModel();

    m_artists.clear();

    endResetModel();
}

Artist SimilarArtistsModel::artistAt(
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

QList<Artist>
SimilarArtistsModel::artists() const
{
    return m_artists;
}

int SimilarArtistsModel::count() const
{
    return m_artists.size();
}