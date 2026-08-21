#pragma once

#include <QList>
#include <QString>

struct Artist
{
    QString id;
    QString name;
};

struct Album
{
    QString id;
    QString title;
    QString coverUri;
    int year = 0;
};

struct Track
{
    QString id;
    QString title;
    QString coverUri;

    int durationMs = 0;

    QList<Artist> artists;
    QList<Album> albums;
};