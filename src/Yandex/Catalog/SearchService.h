#pragma once

#include "../YandexServiceBase.h"
#include "SearchResult.h"
#include <QString>

class SearchService : public YandexServiceBase {
  Q_OBJECT

public:
  explicit SearchService(YandexAuth *auth, QObject *parent = nullptr);

  void search(const QString &query, int page = 0);

  // Подсказки по части запроса: /search/suggest. Ошибки молчаливые —
  // подсказки не обязательны для работы поиска.
  void suggest(const QString &query);

signals:
  void searchStarted();
  void searchReceived(const SearchResults &results);
  void suggestionsReceived(const QStringList &suggestions, const QString &best);
  void errorOccurred(const QString &message);
};