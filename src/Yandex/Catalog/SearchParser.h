#pragma once

#include "SearchResult.h"
#include <QJsonObject>

class SearchParser {
public:
  static SearchResults parse(const QJsonObject &object);
};
