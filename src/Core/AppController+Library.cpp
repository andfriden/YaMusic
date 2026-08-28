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


    m_libraryController
        ->loadUserPlaylists(
            m_accountUid);
}


// =============================================================
// Select library playlist
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
     * Ask MainLayout to open PlaylistPage.
     */

    emit playlistPageRequested();


    /*
     * Then load the selected playlist.
     */

    m_libraryController
        ->selectLibraryPlaylist(
            index);
}


// =============================================================
// Models
// =============================================================

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


// =============================================================
// State
// =============================================================

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