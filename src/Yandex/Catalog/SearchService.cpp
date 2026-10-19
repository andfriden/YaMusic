#include "SearchService.h"
#include "../Auth/YandexAuth.h"
#include "../YandexClient.h"

SearchService::SearchService(YandexAuth *auth, QObject *parent) : YandexServiceBase(auth, parent) {
  connect(m_yandexClient, &YandexClient::searchReceived, this,
          [this](const SearchResults &results) { emit searchReceived(results); });

  connect(m_yandexClient, &YandexClient::searchSuggestionsReceived, this,
          [this](const QStringList &suggestions, const QString &best) {
            emit suggestionsReceived(suggestions, best);
          });

  connect(m_yandexClient, &YandexClient::requestError, this,
          [this](const QString &message) { emit errorOccurred(message); });
}

void SearchService::search(const QString &query, int page) {
  if (!ensureAuthenticated()) {
    emit errorOccurred("Токен Яндекс Музыки не установлен");
    return;
  }

  const QString trimmedQuery = query.trimmed();
  if (trimmedQuery.isEmpty()) {
    emit errorOccurred("Поисковый запрос пуст");
    return;
  }

  emit searchStarted();
  m_yandexClient->search(trimmedQuery, page);
}

void SearchService::suggest(const QString &query) {
  const QString trimmedQuery = query.trimmed();

  if (trimmedQuery.isEmpty()) {
    emit suggestionsReceived({}, {});
    return;
  }

  // Без токена подсказки не запрашиваем: молча оставляем список пустым.
  if (!ensureAuthenticated()) {
    emit suggestionsReceived({}, {});
    return;
  }

  m_yandexClient->searchSuggest(trimmedQuery);
}