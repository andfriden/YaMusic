#include "YandexAuth.h"

#include <QProcessEnvironment>

// Creates the authentication service and loads the token from the environment.
YandexAuth::YandexAuth(QObject *parent)
    : QObject(parent)
{
    loadFromEnvironment();
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

    m_token = token;
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
