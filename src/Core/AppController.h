#pragma once

#include "../Player/PlayerService.h"
#include "../Yandex/Catalog/SearchModel.h"

#include <QObject>
#include <QString>

class YandexAuth;
class AccountService;
class SearchService;
class TrackService;

class AppController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        SearchModel *searchModel
        READ searchModel
        CONSTANT)

    Q_PROPERTY(
        bool searching
        READ isSearching
        NOTIFY searchingChanged)

    Q_PROPERTY(
        bool playing
        READ isPlaying
        NOTIFY playingChanged)

public:
    explicit AppController(
        QObject *parent = nullptr);

    Q_INVOKABLE void testConnection();

    Q_INVOKABLE void testYandexApi();

    Q_INVOKABLE void testSearch(
        const QString &query);

    Q_INVOKABLE void selectSearchResult(
        int index);

    Q_INVOKABLE void play();

    Q_INVOKABLE void pause();

    Q_INVOKABLE void stop();

    SearchModel *searchModel() const;

    bool isSearching() const;

    bool isPlaying() const;

    signals:
        void statusChanged(
            const QString &message);

    void searchingChanged();

    void playingChanged();

private:
    YandexAuth *m_auth = nullptr;

    AccountService *m_accountService = nullptr;

    SearchService *m_searchService = nullptr;

    TrackService *m_trackService = nullptr;

    SearchModel *m_searchModel = nullptr;

    PlayerService *m_playerService = nullptr;

    bool m_searching = false;
};