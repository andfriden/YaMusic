#pragma once

#include "../Models/Track.h"
#include "../Yandex/Catalog/SearchAlbumsModel.h"
#include "../Yandex/Catalog/SearchArtistsModel.h"
#include "../Yandex/Catalog/SearchModel.h"
#include "../Yandex/Catalog/SearchPlaylistsModel.h"
#include <QObject>
#include <QString>
#include <QStringList>

class PlaybackController;
class SearchService;

class SearchController : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool searching READ isSearching NOTIFY searchingChanged)

  Q_PROPERTY(bool canLoadMoreSearch READ canLoadMoreSearch NOTIFY canLoadMoreSearchChanged)

  Q_PROPERTY(SearchModel *model READ model CONSTANT)

  Q_PROPERTY(SearchArtistsModel *artistsModel READ artistsModel CONSTANT)

  Q_PROPERTY(SearchAlbumsModel *albumsModel READ albumsModel CONSTANT)

  Q_PROPERTY(SearchPlaylistsModel *playlistsModel READ playlistsModel CONSTANT)

  // Подсказки запроса из /search/suggest.
  Q_PROPERTY(QStringList suggestions READ suggestions NOTIFY suggestionsChanged)

  // Последние запросы: пустое поле показывает их вместо результатов.
  Q_PROPERTY(QStringList history READ history NOTIFY historyChanged)

  // Запрос, результаты которого сейчас показаны.
  Q_PROPERTY(QString query READ query NOTIFY queryChanged)

  // Исправленный запрос, если сервер предложил более точный вариант.
  Q_PROPERTY(QString correction READ correction NOTIFY correctionChanged)

public:
  explicit SearchController(SearchService *searchService, PlaybackController *playbackController,
                            QObject *parent = nullptr);

  Q_INVOKABLE void search(const QString &query);

  // Запрашивает подсказки для недописанного запроса.
  Q_INVOKABLE void requestSuggestions(const QString &query);

  Q_INVOKABLE void clearHistory();

  // Догружает следующую страницу результатов поиска.
  // Безопасен для вызова — игнорируется, если нет
  // активного запроса или загружены все страницы.
  Q_INVOKABLE void loadMoreSearchResults();

  void selectResult(int index);

  void selectArtistResult(int index);

  void selectAlbumResult(int index);

  void selectPlaylistResult(int index);

  SearchModel *model() const;
  SearchArtistsModel *artistsModel() const;
  SearchAlbumsModel *albumsModel() const;
  SearchPlaylistsModel *playlistsModel() const;

  bool isSearching() const;

  bool canLoadMoreSearch() const;

  QStringList suggestions() const;
  QStringList history() const;
  QString query() const;
  QString correction() const;

signals:
  void statusChanged(const QString &message);

  void searchingChanged();

  void canLoadMoreSearchChanged();

  void suggestionsChanged();
  void historyChanged();
  void queryChanged();
  void correctionChanged();

  void artistClicked(const QString &artistId);

  void albumClicked(const QString &albumId);

  void playlistClicked(const QString &uid, int kind);

private:
  // Общий запуск запроса: запоминание в истории и разрешение
  // автоподстановки задаются отдельно, чтобы исправленный запрос
  // не попадал в историю и не зацикливал поиск.
  void runSearch(const QString &query, bool remember, bool allowCorrection);

  void addToHistory(const QString &query);

  void saveHistory() const;

  static QString normalizeQuery(const QString &query);

  SearchService *m_searchService = nullptr;

  PlaybackController *m_playbackController = nullptr;

  SearchModel *m_model = nullptr;
  SearchArtistsModel *m_artistsModel = nullptr;
  SearchAlbumsModel *m_albumsModel = nullptr;
  SearchPlaylistsModel *m_playlistsModel = nullptr;

  bool m_searching = false;
  bool m_canLoadMore = false;
  int m_currentPage = 0;
  QString m_currentQuery;

  QStringList m_suggestions;
  QStringList m_history;
  QString m_query;
  QString m_typedQuery;
  QString m_correction;
  QString m_bestSuggestion;
  bool m_lastResultEmpty = false;
  bool m_allowCorrection = false;
};