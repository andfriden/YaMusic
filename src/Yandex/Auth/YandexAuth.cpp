#include "YandexAuth.h"
#include "YandexTokenStorage.h"

#include <QProcessEnvironment>

YandexAuth::YandexAuth(QObject *parent) : QObject(parent) {
  load();
}

bool YandexAuth::load() {
  const QString storedToken = YandexTokenStorage::load().trimmed();

  if (!storedToken.isEmpty()) {
    m_token = storedToken;
    emit authenticationChanged();
    return true;
  }
  return loadFromEnvironment();
}

bool YandexAuth::loadFromEnvironment() {
  const QString token =
      QProcessEnvironment::systemEnvironment().value("YANDEX_MUSIC_TOKEN").trimmed();

  if (token.isEmpty()) {
    m_token.clear();
    emit authenticationChanged();
    return false;
  }
  return setToken(token);
}

bool YandexAuth::setToken(const QString &token) {
  const QString trimmedToken = token.trimmed();

  if (trimmedToken.isEmpty())
    return false;
  if (!YandexTokenStorage::saveToken(trimmedToken))
    return false;

  m_token = trimmedToken;
  emit authenticationChanged();
  return true;
}

bool YandexAuth::clearToken() {
  if (!YandexTokenStorage::clearToken())
    return false;

  m_token.clear();
  emit authenticationChanged();
  return true;
}

bool YandexAuth::isAuthenticated() const {
  return !m_token.isEmpty();
}

QString YandexAuth::token() const {
  return m_token;
}