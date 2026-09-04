#include "GenreModel.h"

#include <QVariantList>
#include <QVariantMap>


GenreModel::GenreModel(
    QObject *parent)
    : QAbstractListModel(parent)
{
}


int GenreModel::rowCount(
    const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_genres.size();
}


QVariant GenreModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid())
    {
        return {};
    }

    const int row =
        index.row();

    if (
        row < 0 ||
        row >= m_genres.size()
    )
    {
        return {};
    }

    const Genre &genre =
        m_genres.at(row);


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


        case SubGenresRole:
        {
            QVariantList result;

            result.reserve(
                genre.subGenres.size()
            );


            for (
                const Genre &subGenre :
                genre.subGenres
            )
            {
                QVariantMap item;

                item.insert(
                    "id",
                    subGenre.id
                );

                item.insert(
                    "title",
                    subGenre.title
                );

                item.insert(
                    "fullTitle",
                    subGenre.fullTitle
                );

                item.insert(
                    "urlPart",
                    subGenre.urlPart
                );

                item.insert(
                    "color",
                    subGenre.color
                );

                item.insert(
                    "image208",
                    subGenre.image208
                );

                item.insert(
                    "image300",
                    subGenre.image300
                );

                item.insert(
                    "showInMenu",
                    subGenre.showInMenu
                );

                result.append(
                    item
                );
            }

            return result;
        }


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
        },
        {
            SubGenresRole,
            "subGenres"
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

    return m_genres.at(index);
}


int GenreModel::count() const
{
    return m_genres.size();
}