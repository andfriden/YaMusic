#include "SearchController.h"
#include "../Playback/PlaybackController.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Catalog/SearchService.h"

SearchController::SearchController(
    SearchService *searchService,
    PlaybackController *playbackController,
    QObject *parent)
    : QObject(parent)
    , m_searchService(
          searchService)
    , m_playbackController(
          playbackController)
    , m_model(
          new SearchModel(this))
    , m_artistsModel(
          new SearchArtistsModel(this))
    , m_albumsModel(
          new SearchAlbumsModel(this))
    , m_playlistsModel(
          new SearchPlaylistsModel(this))
{
    if (
        m_searchService == nullptr
    ) {
        return;
    }

    connect(
        m_searchService,
        &SearchService::searchStarted,
        this,
        [this]()
        {
            if (
                m_searching
            ) {
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
            const SearchResults &results)
        {
            m_searching =
                false;

            emit searchingChanged();

            m_model
                ->setResults(
                    results);

            m_artistsModel
                ->setArtists(
                    results.artists);

            m_albumsModel
                ->setAlbums(
                    results.albums);

            m_playlistsModel
                ->setPlaylists(
                    results.playlists);

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
            const QString &message)
        {
            m_searching =
                false;

            emit searchingChanged();

            m_model
                ->clear();

            m_artistsModel
                ->clear();

            m_albumsModel
                ->clear();

            m_playlistsModel
                ->clear();

            emit statusChanged(
                message);
        });
}

void SearchController::search(
    const QString &query)
{
    const QString trimmedQuery =
        query.trimmed();

    if (
        trimmedQuery.isEmpty()
    ) {
        m_model
            ->clear();

        m_artistsModel
            ->clear();

        m_albumsModel
            ->clear();

        m_playlistsModel
            ->clear();

        if (
            m_searching
        ) {
            m_searching =
                false;

            emit searchingChanged();
        }

        emit statusChanged(
            "Введите запрос");

        return;
    }

    if (
        m_searchService == nullptr
    ) {
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
    if (
        m_model == nullptr
    ) {
        emit statusChanged(
            "SearchModel недоступен");

        return;
    }

    if (
        m_playbackController == nullptr
    ) {
        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }

    const Track track =
        m_model
            ->trackAt(
                index);

    if (
        track.id.isEmpty()
    ) {
        emit statusChanged(
            "Некорректный результат поиска");

        return;
    }

    const int count =
        m_model
            ->rowCount();

    QList<Track> tracks;

    tracks.reserve(
        count);

    for (
        int i = 0;
        i < count;
        ++i
    ) {

        const Track searchTrack =
            m_model
                ->trackAt(
                    i);

        if (
            searchTrack.id.isEmpty()
        ) {
            continue;
        }

        tracks.append(
            searchTrack);
    }

    if (
        tracks.isEmpty()
    ) {
        emit statusChanged(
            "Поиск не содержит треков");

        return;
    }

    m_playbackController
        ->playFromSource(
            tracks,
            index,
            "Поиск",
            "search");
}

void SearchController::selectArtistResult(
    int index)
{
    if (
        m_artistsModel == nullptr
    ) {
        return;
    }

    const Artist artist =
        m_artistsModel
            ->artistAt(
                index);

    if (
        artist.id.isEmpty()
    ) {
        return;
    }

    emit artistClicked(
        artist.id);
}

void SearchController::selectAlbumResult(
    int index)
{
    if (
        m_albumsModel == nullptr
    ) {
        return;
    }

    const Album album =
        m_albumsModel
            ->albumAt(
                index);

    if (
        album.id.isEmpty()
    ) {
        return;
    }

    emit albumClicked(
        album.id);
}

void SearchController::selectPlaylistResult(
    int index)
{
    if (
        m_playlistsModel == nullptr
    ) {
        return;
    }

    const PersonalPlaylist playlist =
        m_playlistsModel
            ->playlistAt(
                index);

    if (
        playlist.uid.isEmpty() ||
        playlist.kind <= 0
    ) {
        return;
    }

    emit playlistClicked(
        playlist.uid,
        playlist.kind);
}

SearchModel *
SearchController::model() const
{
    return m_model;
}

SearchArtistsModel *
SearchController::artistsModel() const
{
    return m_artistsModel;
}

SearchAlbumsModel *
SearchController::albumsModel() const
{
    return m_albumsModel;
}

SearchPlaylistsModel *
SearchController::playlistsModel() const
{
    return m_playlistsModel;
}

bool
SearchController::isSearching() const
{
    return m_searching;
}