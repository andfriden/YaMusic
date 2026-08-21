#pragma once

#include <QList>
#include <QString>

#include "../../Models/Track.h"

struct SearchResults
{
    QString query;

    int total = 0;
    int page = 0;
    int perPage = 0;

    QList<Track> tracks;
};