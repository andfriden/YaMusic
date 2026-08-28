#include "AppController.h"


// =============================================================
// Library
// =============================================================

void AppController::loadLibrary()
{
    if (
        m_libraryController == nullptr
    )
    {
        return;
    }


    if (
        m_accountUid.isEmpty()
    )
    {
        emit statusChanged(
            "Пользователь ещё не загружен");

        return;
    }


    /*
     * Load library playlists.
     */

    m_libraryController
        ->loadUserPlaylists(
            m_accountUid);
}


// =============================================================
// Library playlists
// =============================================================

void AppController::selectLibraryPlaylist(
    int index)
{
    if (
        m_libraryController == nullptr
    )
    {
        return;
    }


    /*
     * Tell MainLayout to open PlaylistPage.
     */

    emit playlistPageRequested();


    /*
     * Load the selected playlist.
     */

    m_libraryController
        ->selectLibraryPlaylist(
            index);
}


LibraryPlaylistsModel *
AppController::libraryPlaylistsModel() const
{
    if (
        m_libraryController == nullptr
    )
    {
        return nullptr;
    }


    return m_libraryController
        ->libraryPlaylistsModel();
}


bool
AppController::isLoadingLibraryPlaylists() const
{
    if (
        m_libraryController == nullptr
    )
    {
        return false;
    }


    return m_libraryController
        ->isLoadingLibraryPlaylists();
}


// =============================================================
// Liked tracks
// =============================================================

void AppController::loadLikedTracks()
{
    if (
        m_libraryController == nullptr
    )
    {
        return;
    }


    if (
        m_accountUid.isEmpty()
    )
    {
        emit statusChanged(
            "Пользователь ещё не загружен");

        return;
    }


    m_libraryController
        ->loadLikedTracks(
            m_accountUid);
}


void AppController::selectLikedTrack(
    int index)
{
    if (
        m_libraryController == nullptr
    )
    {
        return;
    }


    m_libraryController
        ->selectLikedTrack(
            index);
}


LikedTracksModel *
AppController::likedTracksModel() const
{
    if (
        m_libraryController == nullptr
    )
    {
        return nullptr;
    }


    return m_libraryController
        ->likedTracksModel();
}


bool
AppController::isLoadingLikedTracks() const
{
    if (
        m_libraryController == nullptr
    )
    {
        return false;
    }


    return m_libraryController
        ->isLoadingLikedTracks();
}