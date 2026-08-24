#include "ArtistAlbumsModel.h"

ArtistAlbumsModel::ArtistAlbumsModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}

int ArtistAlbumsModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_albums.size();
}

QVariant ArtistAlbumsModel::data(
    const QModelIndex &index,
    int role) const
{
    if (
        !index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_albums.size()
    ) {
        return {};
    }

    const Album &album =
        m_albums.at(
            index.row());

    switch (role) {

        case IdRole:
            return album.id;

        case TitleRole:
            return album.title;

        case CoverUriRole:
            return album.coverUri;

        case YearRole:
            return album.year;

        default:
            return {};
    }
}

QHash<int, QByteArray>
ArtistAlbumsModel::roleNames() const
{
    return {
        {IdRole, "albumId"},
        {TitleRole, "title"},
        {CoverUriRole, "coverUri"},
        {YearRole, "year"}
    };
}

void ArtistAlbumsModel::setAlbums(
    const QList<Album> &albums)
{
    beginResetModel();

    m_albums =
        albums;

    endResetModel();
}

void ArtistAlbumsModel::clear()
{
    beginResetModel();

    m_albums.clear();

    endResetModel();
}

Album ArtistAlbumsModel::albumAt(
    int index) const
{
    if (
        index < 0 ||
        index >= m_albums.size()
    ) {
        return {};
    }

    return m_albums.at(
        index);
}

QList<Album>
ArtistAlbumsModel::albums() const
{
    return m_albums;
}

int ArtistAlbumsModel::count() const
{
    return m_albums.size();
}