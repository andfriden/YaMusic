#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QString>
#include <QVariant>

#include "../../Models/Genre.h"


class GenreModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        FullTitleRole,
        UrlPartRole,
        ColorRole,
        Image208Role,
        Image300Role,
        ShowInMenuRole,
        HasSubGenresRole
    };

    Q_ENUM(Roles)


    explicit GenreModel(
        QObject *parent = nullptr);


    int rowCount(
        const QModelIndex &parent =
            QModelIndex()) const override;


    QVariant data(
        const QModelIndex &index,
        int role = Qt::DisplayRole) const override;


    QHash<int, QByteArray>
    roleNames() const override;


    void setGenres(
        const QList<Genre> &genres);


    void clear();


    Genre genreAt(
        int index) const;


    int count() const;


private:

    QList<Genre> m_genres;
};