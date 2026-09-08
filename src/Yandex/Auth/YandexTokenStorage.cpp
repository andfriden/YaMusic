#include "YandexTokenStorage.h"

#include <QSettings>


namespace
{
    constexpr const char *OrganizationName =
        "my.appyamusic";

    constexpr const char *ApplicationName =
        "appYaMusic";

    constexpr const char *TokenKey =
        "yandex/token";
}


// =============================================================
// Load
// =============================================================

QString YandexTokenStorage::load()
{
    QSettings settings(
        OrganizationName,
        ApplicationName
    );

    return settings
        .value(TokenKey)
        .toString();
}


// =============================================================
// Save
// =============================================================

bool YandexTokenStorage::saveToken(
    const QString &token)
{
    if (token.isEmpty())
    {
        return false;
    }

    QSettings settings(
        OrganizationName,
        ApplicationName
    );

    settings.setValue(
        TokenKey,
        token
    );

    settings.sync();

    return settings.status() ==
        QSettings::NoError;
}


// =============================================================
// Clear
// =============================================================

bool YandexTokenStorage::clearToken()
{
    QSettings settings(
        OrganizationName,
        ApplicationName
    );

    settings.remove(
        TokenKey
    );

    settings.sync();

    return settings.status() ==
        QSettings::NoError;
}