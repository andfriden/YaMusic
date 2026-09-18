#include "AppController.h"

void AppController::selectSearchResult(int index) {
  m_searchController->selectResult(index);
}

void AppController::selectSearchArtist(int index) {
  m_searchController->selectArtistResult(index);
}

void AppController::selectSearchAlbum(int index) {
  m_searchController->selectAlbumResult(index);
}

void AppController::selectSearchPlaylist(int index) {
  m_searchController->selectPlaylistResult(index);
}

void AppController::selectMyWaveTrack(int index) {
  m_personalController->selectMyWaveTrack(index);
}

void AppController::selectPersonalPlaylist(const QString &uid, int kind) {
  const QString playlistUid = uid.trimmed();

  if (playlistUid.isEmpty() || kind <= 0) {
    emit statusChanged("Некорректный плейлист");
    return;
  }

  emit playlistPageRequested();
  m_libraryController->loadPlaylist(playlistUid, kind);

  // Для персональных подборок («Собираем для вас»: Премьера,
  // Дежавю, Тайник, Плейлист дня) сервер не отдаёт
  // similar-entities, поэтому подставляем остальные плейлисты
  // из того же раздела лендинга.
  m_libraryController->setSimilarPlaylistsFallback(
      m_personalController->recommendationPlaylistsData(playlistUid, kind));
}

void AppController::selectPlaylistTrack(int index) {
  m_libraryController->selectPlaylistTrack(index);
}

void AppController::selectRecentListening(int index) {
  m_personalController->selectRecentListening(index);
}

void AppController::selectAlbumTrack(int index) {
  m_albumController->selectAlbumTrack(index);
}

void AppController::selectArtistTrack(int index) {
  m_artistController->selectTrack(index);
}

void AppController::selectSimilarArtist(int index) {
  m_artistController->selectSimilarArtist(index);
}

void AppController::selectSimilarTrack(int index) {
  const Track track = m_similarTracksModel->trackAt(index);

  if (track.id.isEmpty()) {
    emit statusChanged("Некорректный трек");
    return;
  }

  const QList<Track> tracks = m_similarTracksModel->tracks();

  if (tracks.isEmpty()) {
    return;
  }

  m_playbackController->playFromSource(tracks, index, "Похожие треки", "similar");
}

void AppController::loadMoreGenreStation() {
  m_genreController->loadMoreGenreStation();
}

void AppController::selectGenreStationTrack(int index) {
  m_genreController->selectStationTrack(index);
}

void AppController::createPlaylist(const QString &title) {
  m_libraryController->createPlaylist(title);
}

void AppController::deleteCurrentPlaylist() {
  m_libraryController->deleteCurrentPlaylist();
}

void AppController::renameCurrentPlaylist(const QString &newTitle) {
  m_libraryController->renameCurrentPlaylist(newTitle);
}

void AppController::removeTrackFromPlaylist(int index) {
  m_libraryController->removeTrackFromPlaylist(index);
}

void AppController::addTrackToPlaylist(int kind, const QString &trackId, const QString &albumId,
                                       int revision) {
  m_libraryController->addTrackToPlaylist(kind, trackId, albumId, revision);
}
