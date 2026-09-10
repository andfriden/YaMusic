#pragma once

#include <QList>
#include <QObject>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QVariantList>

#include "../Models/Playlist.h"
#include "../Yandex/Catalog/GenreModel.h"


class GenreService;
class PlaylistService;


class GenreController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool loading
        READ loading
        NOTIFY loadingChanged
    )

    Q_PROPERTY(
        GenreModel* model
        READ model
        CONSTANT
    )

    Q_PROPERTY(
        bool genreLoading
        READ genreLoading
        NOTIFY genreLoadingChanged
    )

    Q_PROPERTY(
        QVariantList genrePlaylists
        READ genrePlaylists
        NOTIFY genreContentChanged
    )


public:

    explicit GenreController(
        GenreService *genreService,
        PlaylistService *playlistService,
        QObject *parent = nullptr);


    // =============================================================
    // Genres
    // =============================================================

    Q_INVOKABLE void loadGenres();


    // =============================================================
    // Selected genre
    // =============================================================

    Q_INVOKABLE void loadGenre(
        const QString &genreId);


    // =============================================================
    // Tag playlists
    // =============================================================

    Q_INVOKABLE void loadTagPlaylists(
        const QString &tagId);


    // =============================================================
    // Properties
    // =============================================================

    bool loading() const;

    GenreModel *model() const;

    bool genreLoading() const;

    QVariantList genrePlaylists() const;


signals:

    void loadingChanged();

    void genreLoadingChanged();

    void genreContentChanged();

    void statusChanged(
        const QString &status);

    void errorOccurred(
        const QString &message);


private:

    // =============================================================
    // Genre playlist loading
    // =============================================================

    void startGenrePlaylistQueue(
        const QList<QPair<QString, int>> &playlists);


    void loadNextGenrePlaylistBatch();


    void finishGenrePlaylistLoading();


    void startApiFallback();


    // =============================================================
    // CSV
    // =============================================================

    QList<QPair<QString, int>>
        loadPlaylistIdsFromCsv(
            const QString &genreId);


    // =============================================================
    // Helpers
    // =============================================================

    static QStringList splitCsvLine(
        const QString &line);


    static bool isDisplayedGenre(
        const QString &genreId);


    static QString playlistKey(
        const QPair<QString, int> &playlist);


private:

    GenreService *
        m_genreService = nullptr;


    PlaylistService *
        m_playlistService = nullptr;


    GenreModel *
        m_model = nullptr;


    QList<Playlist>
        m_genrePlaylists;


    QString
        m_loadingGenreId;


    // =============================================================
    // Genre loading state
    // =============================================================

    bool
        m_loading = false;


    bool
        m_genreLoading = false;


    bool
        m_waitingForPlaylists = false;


    bool
        m_waitingForTagPlaylistIds = false;


    // =============================================================
    // Playlist queue
    // =============================================================

    QList<QPair<QString, int>>
        m_genrePlaylistQueue;


    int
        m_genrePlaylistQueuePosition = 0;


    bool
        m_usingApiFallback = false;


    bool
        m_csvHadCandidates = false;


    bool
        m_loadedAnyGenrePlaylist = false;


    static constexpr int
        GenrePlaylistBatchSize = 12;
};