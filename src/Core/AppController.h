#pragma once

#include <QObject>
#include <QString>

#include "../Yandex/Catalog/SearchModel.h"

class AccountService;
class SearchService;
class YandexAuth;

// Coordinates authentication and Yandex Music application services.
class AppController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        SearchModel* searchModel
        READ searchModel
        CONSTANT
    )

public:
    explicit AppController(QObject *parent = nullptr);

    // Tests that the application controller is available.
    Q_INVOKABLE void testConnection();

    // Tests loading the authenticated Yandex Music account.
    Q_INVOKABLE void testYandexApi();

    // Searches Yandex Music for the specified query.
    Q_INVOKABLE void testSearch(const QString &query);

    // Returns the search model used by QML.
    SearchModel *searchModel() const;

signals:
    // Emitted when the application status changes.
    void statusChanged(const QString &message);

private:
    YandexAuth *m_auth = nullptr;
    AccountService *m_accountService = nullptr;
    SearchService *m_searchService = nullptr;
    SearchModel *m_searchModel = nullptr;
};
