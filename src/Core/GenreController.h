#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QObject>
#include <QString>
#include <QVariantList>

#include "../Yandex/Catalog/GenreModel.h"

#include "../Models/Playlist.h"


class GenreService;
class PlaylistService;


class GenreController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool loading
        READ isLoading
        NOTIFY loadingChanged)

    Q_PROPERTY(
        GenreModel *model
        READ model
        CONSTANT)

    Q_PROPERTY(
        bool genreLoading
        READ isGenreLoading
        NOTIFY genreLoadingChanged)

    Q_PROPERTY(
        QVariantList genrePlaylists
        READ genrePlaylists
        NOTIFY genreContentChanged)


public:

    explicit GenreController(
        GenreService *genreService,
        PlaylistService *playlistService,
        QObject *parent = nullptr);


    Q_INVOKABLE void loadGenres();


    Q_INVOKABLE void loadGenre(
        const QString &genreId);


    GenreModel *
    model() const;


    bool
    isLoading() const;


    bool
    isGenreLoading() const;


    QVariantList
    genrePlaylists() const;


signals:

    void loadingChanged();

    void genresChanged();

    void statusChanged(
        const QString &message);


    void genreLoadingChanged();

    void genreContentChanged();


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


    bool
        m_loading = false;


    bool
        m_genreLoading = false;


    bool
        m_waitingForPlaylists = false;


    void clearGenreContent();


    void finishGenreLoading();


    QList<QPair<QString, int>>
    loadPlaylistIdsFromCsv(
        const QString &genreId) const;


    static bool
    isDisplayedGenre(
        const QString &genreId);


    static QList<Genre>
    filterDisplayedGenres(
        const QList<Genre> &genres);
};