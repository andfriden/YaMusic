#include "SearchController.h"
#include "../Playback/PlaybackController.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Catalog/SearchService.h"

SearchController::SearchController(SearchService *searchService,
                                   PlaybackController *playbackController, QObject *parent)
    : QObject(parent), m_searchService(searchService), m_playbackController(playbackController),
      m_model(new SearchModel(this)), m_artistsModel(new SearchArtistsModel(this)),
      m_albumsModel(new SearchAlbumsModel(this)), m_playlistsModel(new SearchPlaylistsModel(this)) {
  if (m_searchService == nullptr) {
    return;
  }

  connect(m_searchService, &SearchService::searchStarted, this, [this]() {
    if (m_searching) {
      return;
    }

    m_searching = true;
    emit searchingChanged();
    emit statusChanged("Поиск...");
  });

  connect(
      m_searchService, &SearchService::searchReceived, this, [this](const SearchResults &results) {
        m_searching = false;
        emit searchingChanged();

        // Пагинация: страница 0 заменяет результаты,
        // последующие — дописывают к текущим.
        if (results.page <= 0) {
          m_model->setResults(results);
          m_artistsModel->setArtists(results.artists);
          m_albumsModel->setAlbums(results.albums);
          m_playlistsModel->setPlaylists(results.playlists);

        } else {
          m_model->appendResults(results.tracks);
          m_artistsModel->appendArtists(results.artists);
          m_albumsModel->appendAlbums(results.albums);
          m_playlistsModel->appendPlaylists(results.playlists);
        }

        m_currentPage = results.page;

        const bool canLoadMore =
            results.total > 0 && (results.total > (results.page + 1) * qMax(1, results.perPage));

        if (canLoadMore != m_canLoadMore) {
          m_canLoadMore = canLoadMore;
          emit canLoadMoreSearchChanged();
        }

        emit statusChanged(QString("Найдено результатов: %1").arg(results.total));
      });

  connect(m_searchService, &SearchService::errorOccurred, this, [this](const QString &message) {
    m_searching = false;
    emit searchingChanged();

    // При ошибке новой страницы очищаем всё: результаты
    // больше не имеют смысла без стабильного состояния.
    if (m_currentPage > 0) {
      m_model->clear();
      m_artistsModel->clear();
      m_albumsModel->clear();
      m_playlistsModel->clear();
      m_currentPage = 0;
      m_currentQuery.clear();
    }

    if (m_canLoadMore) {
      m_canLoadMore = false;
      emit canLoadMoreSearchChanged();
    }

    emit statusChanged(message);
  });
}

void SearchController::search(const QString &query) {
  const QString trimmedQuery = query.trimmed();

  if (trimmedQuery.isEmpty()) {
    m_model->clear();
    m_artistsModel->clear();
    m_albumsModel->clear();
    m_playlistsModel->clear();
    m_currentPage = 0;
    m_currentQuery.clear();

    if (m_canLoadMore) {
      m_canLoadMore = false;
      emit canLoadMoreSearchChanged();
    }

    if (m_searching) {
      m_searching = false;
      emit searchingChanged();
    }

    emit statusChanged("Введите запрос");
    return;
  }

  if (m_searchService == nullptr) {
    emit statusChanged("SearchService недоступен");
    return;
  }

  m_currentQuery = trimmedQuery;
  m_searchService->search(trimmedQuery, 0);
}

void SearchController::loadMoreSearchResults() {
  if (!m_canLoadMore || m_searching || m_searchService == nullptr || m_currentQuery.isEmpty()) {
    return;
  }

  m_searching = true;
  emit searchingChanged();
  m_searchService->search(m_currentQuery, m_currentPage + 1);
}

bool SearchController::canLoadMoreSearch() const {
  return m_canLoadMore;
}

void SearchController::selectResult(int index) {
  if (m_model == nullptr) {
    emit statusChanged("SearchModel недоступен");
    return;
  }

  if (m_playbackController == nullptr) {
    emit statusChanged("PlaybackController недоступен");
    return;
  }

  const Track track = m_model->trackAt(index);

  if (track.id.isEmpty()) {
    emit statusChanged("Некорректный результат поиска");
    return;
  }

  const int count = m_model->rowCount();
  QList<Track> tracks;
  tracks.reserve(count);

  for (int i = 0; i < count; ++i) {
    const Track searchTrack = m_model->trackAt(i);
    if (searchTrack.id.isEmpty()) continue;
    tracks.append(searchTrack);
  }

  if (tracks.isEmpty()) {
    emit statusChanged("Поиск не содержит треков");
    return;
  }

  m_playbackController->playFromSource(tracks, index, "Поиск", "search");
}

void SearchController::selectArtistResult(int index) {
  const Artist artist = m_artistsModel->artistAt(index);

  if (artist.id.isEmpty()) {
    return;
  }

  emit artistClicked(artist.id);
}

void SearchController::selectAlbumResult(int index) {
  const Album album = m_albumsModel->albumAt(index);

  if (album.id.isEmpty()) {
    return;
  }

  emit albumClicked(album.id);
}

void SearchController::selectPlaylistResult(int index) {
  const PersonalPlaylist playlist = m_playlistsModel->playlistAt(index);

  if (playlist.uid.isEmpty() || playlist.kind <= 0) {
    return;
  }

  emit playlistClicked(playlist.uid, playlist.kind);
}

SearchModel *SearchController::model() const {
  return m_model;
}

SearchArtistsModel *SearchController::artistsModel() const {
  return m_artistsModel;
}

SearchAlbumsModel *SearchController::albumsModel() const {
  return m_albumsModel;
}

SearchPlaylistsModel *SearchController::playlistsModel() const {
  return m_playlistsModel;
}

bool SearchController::isSearching() const {
  return m_searching;
}