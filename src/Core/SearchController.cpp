#include "SearchController.h"

#include "../Playback/PlaybackController.h"
#include "../Yandex/Catalog/SearchService.h"

#include <QDebug>

SearchController::SearchController(
    SearchService *searchService,
    PlaybackController *playbackController,
    QObject *parent)
    : QObject(parent)
    , m_searchService(searchService)
    , m_playbackController(playbackController)
    , m_model(
          new SearchModel(this))
{
    if (m_searchService == nullptr) {
        return;
    }

    connect(
        m_searchService,
        &SearchService::searchStarted,
        this,
        [this]() {

            if (m_searching) {
                return;
            }

            m_searching =
                true;

            emit searchingChanged();

            emit statusChanged(
                "Поиск...");
        });

    connect(
        m_searchService,
        &SearchService::searchReceived,
        this,
        [this](
            const SearchResults &results) {

            m_searching =
                false;

            emit searchingChanged();

            m_model->setResults(
                results);

            qDebug()
                << "Search results:"
                << results.tracks.size()
                << "/"
                << results.total;

            emit statusChanged(
                QString(
                    "Найдено результатов: %1")
                    .arg(
                        results.total));
        });

    connect(
        m_searchService,
        &SearchService::errorOccurred,
        this,
        [this](
            const QString &message) {

            m_searching =
                false;

            emit searchingChanged();

            m_model->clear();

            qDebug()
                << "Search service error:"
                << message;

            emit statusChanged(
                message);
        });
}

void SearchController::search(
    const QString &query)
{
    const QString trimmedQuery =
        query.trimmed();

    if (trimmedQuery.isEmpty()) {

        m_model->clear();

        if (m_searching) {

            m_searching =
                false;

            emit searchingChanged();
        }

        emit statusChanged(
            "Введите запрос");

        return;
    }

    if (m_searchService == nullptr) {

        emit statusChanged(
            "SearchService недоступен");

        return;
    }

    m_searchService
        ->search(
            trimmedQuery);
}

void SearchController::selectResult(
    int index)
{
    const Track track =
        m_model->trackAt(
            index);

    if (track.id.isEmpty()) {

        emit statusChanged(
            "Некорректный результат поиска");

        return;
    }

    if (m_playbackController == nullptr) {

        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }

    m_playbackController
        ->playTrack(
            track);
}

SearchModel *
SearchController::model() const
{
    return m_model;
}

bool SearchController::isSearching() const
{
    return m_searching;
}