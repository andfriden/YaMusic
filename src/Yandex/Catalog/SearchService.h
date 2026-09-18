#pragma once

#include "../YandexServiceBase.h"
#include "SearchResult.h"
#include <QString>

class SearchService : public YandexServiceBase {
  Q_OBJECT

public:
  explicit SearchService(YandexAuth *auth, QObject *parent = nullptr);

  void search(const QString &query, int page = 0);

signals:
  void searchStarted();
  void searchReceived(const SearchResults &results);
  void errorOccurred(const QString &message);
};