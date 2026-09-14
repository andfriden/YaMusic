#pragma once

#include <QList>
#include <QString>

struct Artist
{
    QString id;
    QString name;
    QString coverUri;
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

    bool liked = false;
};


/*
 * Одна строка синхронизированного текста.
 */
struct LyricLine
{
    qint64 timestampMs = 0;
    QString text;
};


/*
 * Текст песни из /tracks/{id}/supplement.
 */
struct TrackSupplementary
{
    QString trackId;

    QString fullText;

    QList<LyricLine> lines;

    bool hasTimedLines() const
    {
        return !lines.isEmpty();
    }
};