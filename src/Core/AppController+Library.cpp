#include "AppController.h"

void AppController::loadLibrary() {
  if (m_accountUid.isEmpty()) {
    emit statusChanged("Пользователь ещё не загружен");
    return;
  }

  m_libraryController->loadUserPlaylists(m_accountUid);
  m_libraryController->loadLikedAlbums(m_accountUid);
  m_libraryController->loadLikedArtists(m_accountUid);
}

void AppController::selectLibraryPlaylist(int index) {
  emit playlistPageRequested();
  m_libraryController->selectLibraryPlaylist(index);
}

LibraryPlaylistsModel *AppController::libraryPlaylistsModel() const {
  return m_libraryController->libraryPlaylistsModel();
}

bool AppController::isLoadingLibraryPlaylists() const {
  return m_libraryController->isLoadingLibraryPlaylists();
}

void AppController::loadLikedTracks() {
  if (m_accountUid.isEmpty()) {
    emit statusChanged("Пользователь ещё не загружен");
    return;
  }

  m_libraryController->loadLikedTracks(m_accountUid);
}

void AppController::selectLikedTrack(int index) {
  m_libraryController->selectLikedTrack(index);
}

LikedTracksModel *AppController::likedTracksModel() const {
  return m_libraryController->likedTracksModel();
}

bool AppController::isLoadingLikedTracks() const {
  return m_libraryController->isLoadingLikedTracks();
}

void AppController::loadLikedAlbums() {
  if (m_accountUid.isEmpty()) {
    emit statusChanged("Пользователь ещё не загружен");
    return;
  }

  m_libraryController->loadLikedAlbums(m_accountUid);
}

void AppController::selectLikedAlbum(int index) {
  m_libraryController->selectLikedAlbum(index);
}

LikedAlbumsModel *AppController::likedAlbumsModel() const {
  return m_libraryController->likedAlbumsModel();
}

bool AppController::isLoadingLikedAlbums() const {
  return m_libraryController->isLoadingLikedAlbums();
}

void AppController::loadLikedArtists() {
  if (m_accountUid.isEmpty()) {
    emit statusChanged("Пользователь ещё не загружен");
    return;
  }

  m_libraryController->loadLikedArtists(m_accountUid);
}

void AppController::selectLikedArtist(int index) {
  m_libraryController->selectLikedArtist(index);
}

LikedArtistsModel *AppController::likedArtistsModel() const {
  return m_libraryController->likedArtistsModel();
}

bool AppController::isLoadingLikedArtists() const {
  return m_libraryController->isLoadingLikedArtists();
}