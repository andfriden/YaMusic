#include "YandexAuth.h"

#include "YandexTokenStorage.h"

#include <QProcessEnvironment>

// Creates the authentication service and loads the stored token.
YandexAuth::YandexAuth(QObject *parent)
    : QObject(parent)
{
    load();
}

// Loads the token from persistent storage.
// Falls back to the environment variable when no stored token exists.
bool YandexAuth::load()
{
    const QString storedToken =
        YandexTokenStorage::load().trimmed();

    if (!storedToken.isEmpty()) {
        m_token = storedToken;
        emit authenticationChanged();
        return true;
    }

    return loadFromEnvironment();
}

// Loads the Yandex Music OAuth token from the environment.
bool YandexAuth::loadFromEnvironment()
{
    const QString token =
        QProcessEnvironment::systemEnvironment()
            .value("YANDEX_MUSIC_TOKEN")
            .trimmed();

    if (token.isEmpty()) {
        m_token.clear();
        emit authenticationChanged();
        return false;
    }

    return setToken(token);
}

// Stores and activates a new Yandex Music OAuth token.
bool YandexAuth::setToken(const QString &token)
{
    const QString trimmedToken = token.trimmed();

    if (trimmedToken.isEmpty()) {
        return false;
    }

    if (!YandexTokenStorage::saveToken(trimmedToken)) {
        return false;
    }

    m_token = trimmedToken;

    emit authenticationChanged();

    return true;
}

// Removes the stored Yandex Music OAuth token.
bool YandexAuth::clearToken()
{
    if (!YandexTokenStorage::clearToken()) {
        return false;
    }

    m_token.clear();

    emit authenticationChanged();

    return true;
}

// Returns true when a Yandex Music OAuth token is available.
bool YandexAuth::isAuthenticated() const
{
    return !m_token.isEmpty();
}

// Returns the current Yandex Music OAuth token.
QString YandexAuth::token() const
{
    return m_token;
}