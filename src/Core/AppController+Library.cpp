#include "AppController.h"

// Library

void AppController::loadLibrary()
{
    if (m_accountUid.isEmpty())
    {
        emit statusChanged("Пользователь ещё не загружен");
        return;
    }

    m_libraryController->loadUserPlaylists(m_accountUid);
}

void AppController::selectLibraryPlaylist(int index)
{
    emit playlistPageRequested();
    m_libraryController->selectLibraryPlaylist(index);
}

LibraryPlaylistsModel *AppController::libraryPlaylistsModel() const
{
    return m_libraryController->libraryPlaylistsModel();
}

bool AppController::isLoadingLibraryPlaylists() const
{
    return m_libraryController->isLoadingLibraryPlaylists();
}

// Liked tracks

void AppController::loadLikedTracks()
{
    if (m_accountUid.isEmpty())
    {
        emit statusChanged("Пользователь ещё не загружен");
        return;
    }

    m_libraryController->loadLikedTracks(m_accountUid);
}

void AppController::selectLikedTrack(int index)
{
    m_libraryController->selectLikedTrack(index);
}

LikedTracksModel *AppController::likedTracksModel() const
{
    return m_libraryController->likedTracksModel();
}

bool AppController::isLoadingLikedTracks() const
{
    return m_libraryController->isLoadingLikedTracks();
}