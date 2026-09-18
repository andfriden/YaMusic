#include "AccountService.h"
#include "../../Yandex/Auth/YandexAuth.h"
#include "../../Yandex/YandexClient.h"

AccountService::AccountService(YandexAuth *auth, QObject *parent)
    : QObject(parent), m_auth(auth), m_yandexClient(new YandexClient(this)) {
  connect(m_yandexClient, &YandexClient::accountReceived, this,
          [this](const Account &account) { emit accountReceived(account); });

  connect(m_yandexClient, &YandexClient::requestError, this,
          [this](const QString &message) { emit errorOccurred(message); });
}

void AccountService::loadAccount() {
  if (m_auth == nullptr || !m_auth->isAuthenticated()) {
    emit errorOccurred("Yandex Music token is not set");
    return;
  }

  m_yandexClient->setToken(m_auth->token());
  m_yandexClient->getAccountStatus();
}
