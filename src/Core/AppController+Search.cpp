#include "AppController.h"


void AppController::search(
    const QString &query)
{
    const QString trimmedQuery =
        query.trimmed();


    if (
        trimmedQuery.isEmpty()
    )
    {
        return;
    }


    if (
        m_searchController == nullptr
    )
    {
        return;
    }


    emit searchPageRequested(
        trimmedQuery);


    m_searchController
        ->search(
            trimmedQuery);
}