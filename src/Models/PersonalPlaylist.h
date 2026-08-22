#pragma once

#include <QString>

struct PersonalPlaylist
{
    QString id;
    QString uid;

    int kind = 0;

    QString title;
    QString description;
    QString previewDescription;

    QString coverUri;

    int trackCount = 0;

    QString generatedPlaylistType;
};