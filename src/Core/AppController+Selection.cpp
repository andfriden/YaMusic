#include "AppController.h"

// Search
void AppController::selectSearchResult(int index)
{
    if (m_searchController == nullptr) {
        return;
    }

    m_searchController->selectResult(index);
}

void AppController::selectSearchArtist(int index)
{
    if (m_searchController == nullptr) {
        return;
    }

    m_searchController->selectArtistResult(index);
}

void AppController::selectSearchAlbum(int index)
{
    if (m_searchController == nullptr) {
        return;
    }

    m_searchController->selectAlbumResult(index);
}

void AppController::selectSearchPlaylist(int index)
{
    if (m_searchController == nullptr) {
        return;
    }

    m_searchController->selectPlaylistResult(index);
}

// My Wave
void AppController::selectMyWaveTrack(int index)
{
    if (m_personalController == nullptr) {
        return;
    }

    m_personalController->selectMyWaveTrack(index);
}

// Personal / catalog playlist
void AppController::selectPersonalPlaylist(
    const QString &uid,
    int kind)
{
    const QString playlistUid = uid.trimmed();

    if (playlistUid.isEmpty() || kind <= 0) {
        emit statusChanged("Некорректный плейлист");
        return;
    }

    if (m_libraryController == nullptr) {
        return;
    }

    emit playlistPageRequested();

    m_libraryController->loadPlaylist(
        playlistUid,
        kind);

    // Для персональных подборок («Собираем для вас»: Премьера,
    // Дежавю, Тайник, Плейлист дня) сервер не отдаёт
    // similar-entities, поэтому подставляем остальные плейлисты
    // из того же раздела лендинга.
    m_libraryController->setSimilarPlaylistsFallback(
        m_personalController->recommendationPlaylistsData(
            playlistUid,
            kind));
}

// Playlist track
void AppController::selectPlaylistTrack(int index)
{
    if (m_libraryController == nullptr) {
        return;
    }

    m_libraryController->selectPlaylistTrack(index);
}

// Recent listening
void AppController::selectRecentListening(int index)
{
    if (m_personalController == nullptr) {
        return;
    }

    m_personalController->selectRecentListening(index);
}

// Album track
void AppController::selectAlbumTrack(int index)
{
    if (m_albumController == nullptr) {
        return;
    }

    m_albumController->selectAlbumTrack(index);
}

// Artist track
void AppController::selectArtistTrack(int index)
{
    if (m_artistController == nullptr) {
        return;
    }

    m_artistController->selectTrack(index);
}

// Similar artist
void AppController::selectSimilarArtist(int index)
{
    if (m_artistController == nullptr) {
        return;
    }

    m_artistController->selectSimilarArtist(index);
}

// Similar track
void AppController::selectSimilarTrack(int index)
{
    if (m_similarTracksModel == nullptr) {
        return;
    }

    const Track track =
        m_similarTracksModel->trackAt(index);

    if (track.id.isEmpty()) {
        emit statusChanged(
            "Некорректный трек");
        return;
    }

    if (m_playbackController == nullptr) {
        return;
    }

    // Collect all similar tracks and start playing from selected
    const QList<Track> tracks =
        m_similarTracksModel->tracks();

    if (tracks.isEmpty()) {
        return;
    }

    m_playbackController->playFromSource(
        tracks,
        index,
        "Похожие треки",
        "similar");
}
