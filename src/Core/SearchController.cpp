#include "SearchController.h"
#include "../Playback/PlaybackController.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Catalog/SearchService.h"
#include <QSettings>

SearchController::SearchController(SearchService *searchService,
                                   PlaybackController *playbackController, QObject *parent)
    : QObject(parent), m_searchService(searchService), m_playbackController(playbackController),
      m_model(new SearchModel(this)), m_artistsModel(new SearchArtistsModel(this)),
      m_albumsModel(new SearchAlbumsModel(this)), m_playlistsModel(new SearchPlaylistsModel(this)) {
  Q_ASSERT(m_searchService != nullptr);
  Q_ASSERT(m_playbackController != nullptr);

  QSettings settings;
  m_history = settings.value(QStringLiteral("search/history")).toStringList();

  while (m_history.size() > 10) {
    m_history.removeLast();
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

        const bool empty = results.total == 0 && results.artists.isEmpty() &&
                           results.albums.isEmpty() && results.playlists.isEmpty();

        if (results.page <= 0) {
          m_lastResultEmpty = empty;

          // Опечатка: по введённому запросу ничего не нашлось, но сервер
          // знает более точный вариант — повторяем поиск с ним.
          if (empty && m_allowCorrection) {
            const QString best = normalizeQuery(m_bestSuggestion);

            if (!best.isEmpty() &&
                best.compare(m_currentQuery, Qt::CaseInsensitive) != 0) {
              m_correction = best;
              emit correctionChanged();
              emit statusChanged(QStringLiteral("Показаны результаты для «%1»").arg(best));
              runSearch(best, false, false);
              return;
            }
          }
        }

        m_allowCorrection = false;

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

        emit statusChanged(QStringLiteral("Найдено результатов: %1").arg(results.total));
      });

  connect(m_searchService, &SearchService::suggestionsReceived, this,
          [this](const QStringList &suggestions, const QString &best) {
            m_bestSuggestion = normalizeQuery(best);

            if (m_suggestions != suggestions) {
              m_suggestions = suggestions;
              emit suggestionsChanged();
            }

            // Результатов по введённому запросу нет, а подсказка пришла
            // позже ответа поиска — исправляем запрос сейчас.
            if (m_lastResultEmpty && m_correction.isEmpty() &&
                !m_bestSuggestion.isEmpty() &&
                m_bestSuggestion.compare(m_currentQuery, Qt::CaseInsensitive) != 0) {
              m_correction = m_bestSuggestion;
              emit correctionChanged();
              emit statusChanged(QStringLiteral("Показаны результаты для «%1»").arg(m_correction));
              runSearch(m_correction, false, false);
            }
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
  const QString normalized = normalizeQuery(query);

  if (normalized.isEmpty()) {
    m_model->clear();
    m_artistsModel->clear();
    m_albumsModel->clear();
    m_playlistsModel->clear();
    m_currentPage = 0;
    m_currentQuery.clear();
    m_typedQuery.clear();
    m_lastResultEmpty = false;
    m_allowCorrection = false;

    if (!m_query.isEmpty()) {
      m_query.clear();
      emit queryChanged();
    }

    if (!m_correction.isEmpty()) {
      m_correction.clear();
      emit correctionChanged();
    }

    if (!m_suggestions.isEmpty()) {
      m_suggestions.clear();
      emit suggestionsChanged();
    }

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

  m_typedQuery = normalized;

  if (!m_correction.isEmpty()) {
    m_correction.clear();
    emit correctionChanged();
  }

  if (!m_suggestions.isEmpty()) {
    m_suggestions.clear();
    emit suggestionsChanged();
  }

  runSearch(normalized, true, true);
}

void SearchController::requestSuggestions(const QString &query) {
  const QString normalized = normalizeQuery(query);

  if (normalized.size() < 2) {
    m_bestSuggestion.clear();

    if (!m_suggestions.isEmpty()) {
      m_suggestions.clear();
      emit suggestionsChanged();
    }

    return;
  }

  // Ответ на предыдущий запрос уже отменён клиентом, поэтому best
  // сбрасываем: он должен относиться именно к текущему вводу.
  m_bestSuggestion.clear();
  m_searchService->suggest(normalized);
}

void SearchController::runSearch(const QString &query, bool remember, bool allowCorrection) {
  m_currentQuery = query;
  m_allowCorrection = allowCorrection;
  m_lastResultEmpty = false;

  if (m_query != query) {
    m_query = query;
    emit queryChanged();
  }

  if (remember) {
    addToHistory(query);
  }

  m_searchService->search(query, 0);
}

void SearchController::addToHistory(const QString &query) {
  const QString normalized = normalizeQuery(query);

  if (normalized.isEmpty()) {
    return;
  }

  for (int i = m_history.size() - 1; i >= 0; --i) {
    if (m_history.at(i).compare(normalized, Qt::CaseInsensitive) == 0) {
      m_history.removeAt(i);
    }
  }

  m_history.prepend(normalized);

  while (m_history.size() > 10) {
    m_history.removeLast();
  }

  saveHistory();
  emit historyChanged();
}

void SearchController::saveHistory() const {
  QSettings settings;
  settings.setValue(QStringLiteral("search/history"), m_history);
}

void SearchController::clearHistory() {
  if (m_history.isEmpty()) {
    return;
  }

  m_history.clear();
  saveHistory();
  emit historyChanged();
}

QString SearchController::normalizeQuery(const QString &query) {
  return query.simplified();
}

void SearchController::loadMoreSearchResults() {
  if (!m_canLoadMore || m_searching || m_currentQuery.isEmpty()) {
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

QStringList SearchController::suggestions() const {
  return m_suggestions;
}

QStringList SearchController::history() const {
  return m_history;
}

QString SearchController::query() const {
  return m_query;
}

QString SearchController::correction() const {
  return m_correction;
}