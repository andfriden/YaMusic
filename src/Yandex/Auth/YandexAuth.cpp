#include "YandexAuth.h"

#include <QProcessEnvironment>

YandexAuth::YandexAuth(QObject *parent)
    : QObject(parent)
{
}

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

bool YandexAuth::isAuthenticated() const
{
    return !m_token.isEmpty();
}

QString YandexAuth::token() const
{
    return m_token;
}
