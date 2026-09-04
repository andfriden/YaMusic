#include "GenreController.h"

#include "../Playback/PlaybackController.h"

#include "../Yandex/Catalog/GenreService.h"
#include "../Yandex/Personal/PlaylistService.h"

#include <QSet>
#include <QVariantMap>


namespace
{

QVariantMap trackToMap(
    const Track &track)
{
    QVariantMap result;


    result.insert(
        "id",
        track.id);


    result.insert(
        "title",
        track.title);


    result.insert(
        "coverUri",
        track.coverUri);


    result.insert(
        "durationMs",
        track.durationMs);


    if (
        !track.artists.isEmpty()
    )
    {
        result.insert(
            "artist",
            track.artists
                .first()
                .name);


        result.insert(
            "artistId",
            track.artists
                .first()
                .id);
    }
    else
    {
        result.insert(
            "artist",
            "");

        result.insert(
            "artistId",
            "");
    }


    if (
        !track.albums.isEmpty()
    )
    {
        result.insert(
            "album",
            track.albums
                .first()
                .title);


        result.insert(
            "albumId",
            track.albums
                .first()
                .id);
    }
    else
    {
        result.insert(
            "album",
            "");

        result.insert(
            "albumId",
            "");
    }


    return result;
}


QVariantMap playlistToMap(
    const Playlist &playlist)
{
    QVariantMap result;


    result.insert(
        "uid",
        playlist.uid);


    result.insert(
        "kind",
        playlist.kind);


    result.insert(
        "title",
        playlist.title);


    result.insert(
        "description",
        playlist.description);


    result.insert(
        "trackCount",
        playlist.trackCount);


    result.insert(
        "coverUri",
        playlist.coverUri);


    return result;
}

}


// =============================================================
// Constructor
// =============================================================

GenreController::GenreController(
    GenreService *genreService,
    PlaylistService *playlistService,
    PlaybackController *playbackController,
    QObject *parent)
    : QObject(parent)
    , m_genreService(genreService)
    , m_playlistService(playlistService)
    , m_playbackController(playbackController)
    , m_model(new GenreModel(this))
{
    if (
        m_genreService != nullptr
    )
    {
        connect(
            m_genreService,
            &GenreService::genresReceived,
            this,
            [this](
                const QList<Genre> &genres)
            {
                m_model->setGenres(
                    genres);

                m_loading =
                    false;

                emit loadingChanged();
                emit genresChanged();
            });


        connect(
            m_genreService,
            &GenreService::tagPlaylistIdsReceived,
            this,
            [this](
                const QString &tagId,
                const QList<QPair<QString, int>>
                    &playlists)
            {
                if (
                    !m_subGenreLoading ||
                    tagId != m_loadingSubGenreId
                )
                {
                    return;
                }


                if (
                    playlists.isEmpty()
                )
                {
                    finishSubGenreLoading();

                    return;
                }


                if (
                    m_playlistService == nullptr
                )
                {
                    finishSubGenreLoading();

                    return;
                }


                m_waitingForTagPlaylists =
                    true;


                m_playlistService
                    ->loadPlaylists(
                        playlists);
            });


        connect(
            m_genreService,
            &GenreService::errorOccurred,
            this,
            [this](
                const QString &message)
            {
                if (m_loading)
                {
                    m_loading = false;

                    emit loadingChanged();


                    emit statusChanged(
                        QString(
                            "Ошибка жанров: %1")
                        .arg(
                            message));
                }


                if (
                    m_subGenreLoading
                )
                {
                    finishSubGenreLoading();


                    emit statusChanged(
                        QString(
                            "Ошибка поджанра: %1")
                        .arg(
                            message));
                }
            });
    }


    if (
        m_playlistService != nullptr
    )
    {
        connect(
            m_playlistService,
            &PlaylistService::playlistsReceived,
            this,
            [this](
                const QList<Playlist> &playlists)
            {
                if (
                    !m_subGenreLoading ||
                    !m_waitingForTagPlaylists
                )
                {
                    return;
                }


                m_waitingForTagPlaylists =
                    false;


                m_subGenrePlaylists =
                    playlists;


                QList<Track>
                    tracks;


                QSet<QString>
                    seenTracks;


                for (
                    const Playlist &playlist :
                    playlists
                )
                {
                    for (
                        const Track &track :
                        playlist.tracks
                    )
                    {
                        if (
                            track.id.isEmpty() ||
                            seenTracks.contains(
                                track.id)
                        )
                        {
                            continue;
                        }


                        seenTracks.insert(
                            track.id);


                        tracks.append(
                            track);


                        if (
                            tracks.size() >= 100
                        )
                        {
                            break;
                        }
                    }


                    if (
                        tracks.size() >= 100
                    )
                    {
                        break;
                    }
                }


                m_subGenreTracks =
                    tracks;


                emit subGenreContentChanged();


                finishSubGenreLoading();
            });


        connect(
            m_playlistService,
            &PlaylistService::errorOccurred,
            this,
            [this](
                const QString &message)
            {
                if (
                    !m_subGenreLoading
                )
                {
                    return;
                }


                finishSubGenreLoading();


                emit statusChanged(
                    QString(
                        "Ошибка контента поджанра: %1")
                    .arg(
                        message));
            });
    }
}


// =============================================================
// Genres
// =============================================================

void GenreController::loadGenres()
{
    if (
        m_genreService == nullptr
    )
    {
        return;
    }


    if (
        m_loading
    )
    {
        return;
    }


    m_loading =
        true;


    emit loadingChanged();


    m_model->clear();


    m_genreService
        ->loadGenres();
}


// =============================================================
// Subgenre
// =============================================================

void GenreController::loadSubGenre(
    const QString &genreId)
{
    const QString id =
        genreId.trimmed();


    if (
        id.isEmpty() ||
        m_genreService == nullptr ||
        m_playlistService == nullptr
    )
    {
        return;
    }


    if (
        m_subGenreLoading
    )
    {
        return;
    }


    m_loadingSubGenreId =
        id;


    m_subGenreLoading =
        true;


    m_waitingForTagPlaylists =
        false;


    clearSubGenreContent();


    emit subGenreLoadingChanged();


    m_genreService
        ->loadTagPlaylistIds(
            id);
}


// =============================================================
// Play subgenre track
// =============================================================

void GenreController::playSubGenreTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    )
    {
        return;
    }


    if (
        index < 0 ||
        index >= m_subGenreTracks.size()
    )
    {
        return;
    }


    const Track &track =
        m_subGenreTracks.at(
            index);


    if (
        track.id.isEmpty()
    )
    {
        return;
    }


    m_playbackController
        ->playTrack(
            track);
}


// =============================================================
// Model
// =============================================================

GenreModel *
GenreController::model() const
{
    return m_model;
}


// =============================================================
// Loading
// =============================================================

bool
GenreController::isLoading() const
{
    return m_loading;
}


bool
GenreController::isSubGenreLoading() const
{
    return m_subGenreLoading;
}


// =============================================================
// Subgenre data
// =============================================================

QVariantList
GenreController::subGenreTracks() const
{
    QVariantList result;


    result.reserve(
        m_subGenreTracks.size());


    for (
        const Track &track :
        m_subGenreTracks
    )
    {
        result.append(
            trackToMap(
                track));
    }


    return result;
}


QVariantList
GenreController::subGenrePlaylists() const
{
    QVariantList result;


    result.reserve(
        m_subGenrePlaylists.size());


    for (
        const Playlist &playlist :
        m_subGenrePlaylists
    )
    {
        result.append(
            playlistToMap(
                playlist));
    }


    return result;
}


// =============================================================
// Clear
// =============================================================

void GenreController::clearSubGenreContent()
{
    m_subGenreTracks.clear();

    m_subGenrePlaylists.clear();


    emit subGenreContentChanged();
}


// =============================================================
// Finish loading
// =============================================================

void GenreController::finishSubGenreLoading()
{
    if (
        !m_subGenreLoading
    )
    {
        return;
    }


    m_subGenreLoading =
        false;


    m_waitingForTagPlaylists =
        false;


    emit subGenreLoadingChanged();
}