#pragma once

#include <QString>

struct Track
{
    QString id;
    QString title;
    QString artist;
    QString album;
    QString artworkUrl;
    int durationMs = 0;
};
