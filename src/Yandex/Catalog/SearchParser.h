#pragma once

#include "SearchResult.h"

#include <QJsonObject>

// Converts a Yandex Music search response into application models.
class SearchParser
{
public:
    // Parses the JSON object returned by the Yandex Music search API.
    static SearchResults parse(const QJsonObject &object);
};
