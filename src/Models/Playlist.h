#pragma once

#include <QList>
#include <QString>

#include "Track.h"

struct Playlist
{
    QString uid;
    int kind = 0;

    QString title;
    QString description;
    QString coverUri;

    int trackCount = 0;

    QList<Track> tracks;
};