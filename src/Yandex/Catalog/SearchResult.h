#pragma once

#include <QList>
#include <QString>

// Represents an artist returned by Yandex Music.
struct SearchArtist
{
    QString id;
    QString name;
};

// Represents an album returned by Yandex Music.
struct SearchAlbum
{
    QString id;
    QString title;
    QString coverUri;
    int year = 0;
};

// Represents a track returned by Yandex Music search.
struct SearchTrack
{
    QString id;
    QString title;
    QString coverUri;
    QString albumTitle;
    int durationMs = 0;
    QList<SearchArtist> artists;
    QList<SearchAlbum> albums;
};

// Contains the track results returned by a search request.
struct SearchResults
{
    QString query;
    int total = 0;
    int page = 0;
    int perPage = 0;
    QList<SearchTrack> tracks;
};
