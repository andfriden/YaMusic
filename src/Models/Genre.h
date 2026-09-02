#pragma once

#include <QList>
#include <QString>


struct Genre
{
    QString id;
    QString title;
    QString fullTitle;
    QString urlPart;
    QString color;

    QString image208;
    QString image300;

    bool showInMenu = false;

    QList<Genre> subGenres;
};