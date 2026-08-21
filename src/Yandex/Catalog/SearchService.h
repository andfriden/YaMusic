#pragma once

#include <QObject>
#include <QString>

#include "SearchResult.h"

class YandexAuth;
class YandexClient;

class SearchService : public QObject
{
    Q_OBJECT

public:
    explicit SearchService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void search(
        const QString &query);

    signals:
        void searchStarted();

    void searchReceived(
        const SearchResults &results);

    void errorOccurred(
        const QString &message);

private:
    YandexAuth *m_auth = nullptr;
    YandexClient *m_yandexClient = nullptr;
};