#pragma once

#include <QString>

struct Account
{
    qint64 uid = 0;

    QString login;
    QString firstName;
    QString lastName;
    QString displayName;

    int region = 0;
    QString regionCode;

    bool serviceAvailable = false;
    bool hasPlus = false;
};
