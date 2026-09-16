#pragma once

#include <QList>
#include <QString>
#include "../../Models/PersonalPlaylist.h"
#include "../../Models/Track.h"

struct SearchResults
{
    QString query;

    int total = 0;
    int page = 0;
    int perPage = 0;

    QList<Track> tracks;
    QList<Artist> artists;
    QList<Album> albums;
    QList<PersonalPlaylist> playlists;
};