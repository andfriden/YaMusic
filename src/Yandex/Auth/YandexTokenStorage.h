#pragma once

#include <QString>

class YandexTokenStorage
{
public:
    static QString load();
    static bool saveToken(const QString &token);
    static bool clearToken();
};