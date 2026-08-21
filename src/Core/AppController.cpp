#include "AppController.h"

#include "../Yandex/Account/AccountService.h"
#include "../Yandex/Auth/YandexAuth.h"
#include "../Yandex/Catalog/SearchModel.h"
#include "../Yandex/Catalog/SearchService.h"

#include <QDebug>

// Creates the application controller and initializes Yandex services.
AppController::AppController(QObject *parent)
    : QObject(parent)
    , m_auth(new YandexAuth(this))
    , m_accountService(new AccountService(m_auth, this))
    , m_searchService(new SearchService(m_auth, this))
    , m_searchModel(new SearchModel(this))
{
    connect(
        m_accountService,
        &AccountService::accountReceived,
        this,
        [this](const Account &account) {
            const QString message =
                QString("Logged in as %1 (uid: %2)")
                    .arg(account.displayName)
                    .arg(account.uid);

            qDebug() << message;

            emit statusChanged(message);
        });

    connect(
        m_accountService,
        &AccountService::errorOccurred,
        this,
        [this](const QString &message) {
            qDebug()
                << "Account service error:"
                << message;

            emit statusChanged(message);
        });

    connect(
        m_searchService,
        &SearchService::searchReceived,
        this,
        [this](const SearchResults &results) {
            m_searchModel->setResults(results);

            qDebug()
                << "Search results:"
                << results.tracks.size()
                << "/"
                << results.total;

            emit statusChanged(
                QString("Found %1 results")
                    .arg(results.total));
        });

    connect(
        m_searchService,
        &SearchService::errorOccurred,
        this,
        [this](const QString &message) {
            qDebug()
                << "Search service error:"
                << message;

            m_searchModel->clear();

            emit statusChanged(message);
        });
}

// Tests that AppController is functioning.
void AppController::testConnection()
{
    qDebug()
        << "AppController is working";

    emit statusChanged(
        "AppController is working");
}

// Tests the authenticated Yandex Music account.
void AppController::testYandexApi()
{
    m_accountService->loadAccount();

    emit statusChanged(
        "Requesting Yandex account status...");
}

// Searches Yandex Music for the specified query.
void AppController::testSearch(
    const QString &query)
{
    m_searchService->search(query);

    emit statusChanged(
        "Searching...");
}

// Returns the search model used by QML.
SearchModel *AppController::searchModel() const
{
    return m_searchModel;
}
