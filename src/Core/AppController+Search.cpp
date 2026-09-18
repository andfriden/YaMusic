#include "AppController.h"

void AppController::search(const QString &query) {
  const QString trimmedQuery = query.trimmed();

  if (trimmedQuery.isEmpty()) {
    return;
  }

  emit searchPageRequested(trimmedQuery);
  m_searchController->search(trimmedQuery);
}

void AppController::loadMoreSearchResults() {
  m_searchController->loadMoreSearchResults();
}