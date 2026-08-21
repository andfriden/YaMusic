#include "YandexTokenStorage.h"

#include <QSettings>

QString YandexTokenStorage::load()
{
    QSettings settings;

    return settings.value("yandex/token").toString();
}

bool YandexTokenStorage::saveToken(const QString &token)
{
    if (token.isEmpty()) {
        return false;
    }

    QSettings settings;

    settings.setValue("yandex/token", token);
    settings.sync();

    return settings.status() == QSettings::NoError;
}

bool YandexTokenStorage::clearToken()
{
    QSettings settings;

    settings.remove("yandex/token");
    settings.sync();

    return settings.status() == QSettings::NoError;
}