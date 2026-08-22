#pragma once

#include <QString>

struct RecentListeningItem
{
    QString id;
    QString type;

    QString title;
    QString subtitle;

    QString coverUri;

    QString context;
    QString contextItem;

    QString uid;
    int kind = 0;
};