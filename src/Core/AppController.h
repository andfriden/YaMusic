#pragma once

#include <QObject>
#include <QString>

#include "../Yandex/Catalog/SearchModel.h"

class AccountService;
class SearchService;
class YandexAuth;

class AppController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        SearchModel* searchModel
        READ searchModel
        CONSTANT
    )

    Q_PROPERTY(
        bool authenticated
        READ isAuthenticated
        NOTIFY authenticationChanged
    )

public:
    explicit AppController(QObject *parent = nullptr);

    Q_INVOKABLE void testConnection();
    Q_INVOKABLE void testYandexApi();
    Q_INVOKABLE void testSearch(const QString &query);

    Q_INVOKABLE bool setToken(const QString &token);
    Q_INVOKABLE bool clearToken();

    bool isAuthenticated() const;

    SearchModel *searchModel() const;

    signals:
        void statusChanged(const QString &message);
    void authenticationChanged();

private:
    YandexAuth *m_auth = nullptr;
    AccountService *m_accountService = nullptr;
    SearchService *m_searchService = nullptr;
    SearchModel *m_searchModel = nullptr;
};