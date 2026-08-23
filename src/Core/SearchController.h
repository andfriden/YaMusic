#pragma once

#include <QObject>
#include <QString>

#include "../Models/Track.h"
#include "../Yandex/Catalog/SearchModel.h"

class PlaybackController;
class SearchService;

class SearchController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool searching
        READ isSearching
        NOTIFY searchingChanged)

    Q_PROPERTY(
        SearchModel *model
        READ model
        CONSTANT)

public:
    explicit SearchController(
        SearchService *searchService,
        PlaybackController *playbackController,
        QObject *parent = nullptr);

    void search(
        const QString &query);

    void selectResult(
        int index);

    SearchModel *model() const;

    bool isSearching() const;

    signals:
        void statusChanged(
            const QString &message);

    void searchingChanged();

private:
    SearchService *m_searchService = nullptr;

    PlaybackController *
        m_playbackController = nullptr;

    SearchModel *m_model = nullptr;

    bool m_searching = false;
};