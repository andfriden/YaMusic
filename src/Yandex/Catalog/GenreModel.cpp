#include "GenreModel.h"


GenreModel::GenreModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


int GenreModel::rowCount(
    const QModelIndex &parent) const
{
    if (
        parent.isValid()
    )
    {
        return 0;
    }

    return m_genres.size();
}


QVariant GenreModel::data(
    const QModelIndex &index,
    int role) const
{
    if (
        !index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_genres.size()
    )
    {
        return {};
    }


    const Genre &genre =
        m_genres.at(
            index.row());


    switch (role)
    {
        case IdRole:
            return genre.id;


        case TitleRole:
            return genre.title;


        case FullTitleRole:
            return genre.fullTitle;


        case UrlPartRole:
            return genre.urlPart;


        case ColorRole:
            return genre.color;


        case Image208Role:
            return genre.image208;


        case Image300Role:
            return genre.image300;


        case ShowInMenuRole:
            return genre.showInMenu;


        case HasSubGenresRole:
            return !genre.subGenres.isEmpty();


        default:
            return {};
    }
}


QHash<int, QByteArray>
GenreModel::roleNames() const
{
    return {
        {
            IdRole,
            "genreId"
        },
        {
            TitleRole,
            "title"
        },
        {
            FullTitleRole,
            "fullTitle"
        },
        {
            UrlPartRole,
            "urlPart"
        },
        {
            ColorRole,
            "color"
        },
        {
            Image208Role,
            "image208"
        },
        {
            Image300Role,
            "image300"
        },
        {
            ShowInMenuRole,
            "showInMenu"
        },
        {
            HasSubGenresRole,
            "hasSubGenres"
        }
    };
}


void GenreModel::setGenres(
    const QList<Genre> &genres)
{
    beginResetModel();

    m_genres =
        genres;

    endResetModel();
}


void GenreModel::clear()
{
    beginResetModel();

    m_genres.clear();

    endResetModel();
}


Genre GenreModel::genreAt(
    int index) const
{
    if (
        index < 0 ||
        index >= m_genres.size()
    )
    {
        return {};
    }

    return m_genres.at(
        index);
}


int GenreModel::count() const
{
    return m_genres.size();
}