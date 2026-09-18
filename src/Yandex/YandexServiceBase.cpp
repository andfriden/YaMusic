#include "YandexServiceBase.h"
#include "Auth/YandexAuth.h"
#include "YandexClient.h"

YandexServiceBase::YandexServiceBase(YandexAuth *auth, QObject *parent)
    : QObject(parent), m_auth(auth), m_yandexClient(new YandexClient(this)) {
  Q_ASSERT(m_auth);
}

// TODO(YM-2242): сообщать об отсутствии авторизации отдельным сигналом, а не только false.
bool YandexServiceBase::ensureAuthenticated() {
  if (!m_auth->isAuthenticated()) {
    return false;
  }

  m_yandexClient->setToken(m_auth->token());
  return true;
}