#pragma once

#include <QObject>
#include <QString>
#include "../Models/Track.h"
#include "../Yandex/Catalog/SearchArtistsModel.h"
#include "../Yandex/Catalog/SearchAlbumsModel.h"
#include "../Yandex/Catalog/SearchPlaylistsModel.h"
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
        bool canLoadMoreSearch
        READ canLoadMoreSearch
        NOTIFY canLoadMoreSearchChanged)

    Q_PROPERTY(
        SearchModel *model
        READ model
        CONSTANT)

    Q_PROPERTY(
        SearchArtistsModel *artistsModel
        READ artistsModel
        CONSTANT)

    Q_PROPERTY(
        SearchAlbumsModel *albumsModel
        READ albumsModel
        CONSTANT)

    Q_PROPERTY(
        SearchPlaylistsModel *playlistsModel
        READ playlistsModel
        CONSTANT)

public:
    explicit SearchController(
        SearchService *searchService,
        PlaybackController *playbackController,
        QObject *parent = nullptr);

    void search(
        const QString &query);

    /*
     * Догружает следующую страницу результатов поиска.
     * Безопасен для вызова — игнорируется, если нет
     * активного запроса или загружены все страницы.
     */
    Q_INVOKABLE void loadMoreSearchResults();

    void selectResult(
        int index);

    void selectArtistResult(
        int index);

    void selectAlbumResult(
        int index);

    void selectPlaylistResult(
        int index);

    SearchModel *model() const;
    SearchArtistsModel *artistsModel() const;
    SearchAlbumsModel *albumsModel() const;
    SearchPlaylistsModel *playlistsModel() const;

    bool isSearching() const;

    bool canLoadMoreSearch() const;

    signals:
        void statusChanged(
            const QString &message);

    void searchingChanged();

    void canLoadMoreSearchChanged();

    void artistClicked(
        const QString &artistId);

    void albumClicked(
        const QString &albumId);

    void playlistClicked(
        const QString &uid,
        int kind);

private:
    SearchService *m_searchService = nullptr;

    PlaybackController *
        m_playbackController = nullptr;

    SearchModel *m_model = nullptr;
    SearchArtistsModel *m_artistsModel = nullptr;
    SearchAlbumsModel *m_albumsModel = nullptr;
    SearchPlaylistsModel *m_playlistsModel = nullptr;

    bool m_searching = false;
    bool m_canLoadMore = false;
    int m_currentPage = 0;
    QString m_currentQuery;
};