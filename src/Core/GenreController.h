#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QVariantList>

#include "../Yandex/Catalog/GenreModel.h"

#include "../Models/Track.h"
#include "../Models/Playlist.h"


class GenreService;
class PlaylistService;
class PlaybackController;


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
        bool subGenreLoading
        READ isSubGenreLoading
        NOTIFY subGenreLoadingChanged)

    Q_PROPERTY(
        QVariantList subGenreTracks
        READ subGenreTracks
        NOTIFY subGenreContentChanged)

    Q_PROPERTY(
        QVariantList subGenrePlaylists
        READ subGenrePlaylists
        NOTIFY subGenreContentChanged)


public:

    explicit GenreController(
        GenreService *genreService,
        PlaylistService *playlistService,
        PlaybackController *playbackController,
        QObject *parent = nullptr);


    Q_INVOKABLE void loadGenres();


    Q_INVOKABLE void loadSubGenre(
        const QString &genreId);


    Q_INVOKABLE void playSubGenreTrack(
        int index);


    GenreModel *
    model() const;


    bool
    isLoading() const;


    bool
    isSubGenreLoading() const;


    QVariantList
    subGenreTracks() const;


    QVariantList
    subGenrePlaylists() const;


signals:

    void loadingChanged();

    void genresChanged();

    void statusChanged(
        const QString &message);


    void subGenreLoadingChanged();

    void subGenreContentChanged();


private:

    GenreService *
        m_genreService = nullptr;


    PlaylistService *
        m_playlistService = nullptr;


    PlaybackController *
        m_playbackController = nullptr;


    GenreModel *
        m_model = nullptr;


    QList<Track>
        m_subGenreTracks;


    QList<Playlist>
        m_subGenrePlaylists;


    QString
        m_loadingSubGenreId;


    bool
        m_loading = false;


    bool
        m_subGenreLoading = false;


    bool
        m_waitingForTagPlaylists = false;


    void clearSubGenreContent();


    void finishSubGenreLoading();
};