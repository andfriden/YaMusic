#include "LibraryController.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Personal/LikedAlbumsModel.h"
#include "../Yandex/Personal/LikedArtistsModel.h"
#include "../Yandex/Personal/LikesService.h"

LibraryController::LibraryController(PlaylistService *playlistService, ArtistService *artistService,
                                     LikesService *likesService,
                                     PlaybackController *playbackController, QObject *parent)
    : QObject(parent), m_playlistService(playlistService), m_artistService(artistService),
      m_likesService(likesService), m_playbackController(playbackController),
      m_libraryPlaylistsModel(new LibraryPlaylistsModel(this)),
      m_likedTracksModel(new LikedTracksModel(this)),
      m_likedAlbumsModel(new LikedAlbumsModel(this)),
      m_likedArtistsModel(new LikedArtistsModel(this)), m_playlistModel(new PlaylistModel(this)),
      m_artistModel(new ArtistModel(this)) {
  if (m_playlistService != nullptr) {
    connect(m_playlistService, &PlaylistService::playlistReceived, this,
            [this](const Playlist &playlist) {
              // Если мы уже открыли другой плейлист, а этот ответ
              // пришёл для другой цели (например, /change вернул
              // целевой плейлист) — не затираем текущую модель.
              const int currentKind = m_playlistModel->kind();
              if (currentKind > 0 && currentKind != playlist.kind) {
                return;
              }

              m_loadingPlaylist = false;
              emit loadingPlaylistChanged();
              Playlist playlistWithLikes = playlist;

              if (m_likesService != nullptr) {
                for (Track &track : playlistWithLikes.tracks) {
                  track.liked = m_likesService->isLiked(track.id);
                }
              }

              m_playlistModel->setPlaylist(playlistWithLikes);
              m_currentPlaylistTitle = playlistWithLikes.title;
              m_currentPlaylistCoverUri = playlistWithLikes.coverUri;
              m_currentPlaylistTrackCount = playlistWithLikes.trackCount;
              emit currentPlaylistChanged();
            });

    connect(m_playlistService, &PlaylistService::similarPlaylistsReceived, this,
            [this](const QList<Playlist> &playlists) {
              QVariantList result;

              for (const Playlist &playlist : playlists) {
                QVariantMap item;
                item.insert("uid", playlist.uid);
                item.insert("uuid", playlist.uuid);
                item.insert("kind", playlist.kind);
                item.insert("title", playlist.title);
                item.insert("coverUri", playlist.coverUri);
                item.insert("trackCount", playlist.trackCount);
                result.append(item);
              }

              m_similarPlaylists = result;
              emit similarPlaylistsChanged();
            });

    connect(m_playlistService, &PlaylistService::userPlaylistsReceived, this,
            [this](const QList<PersonalPlaylist> &playlists) {
              m_loadingLibraryPlaylists = false;
              emit loadingLibraryPlaylistsChanged();
              m_libraryPlaylistsModel->setPlaylists(playlists);
            });

    connect(m_playlistService, &PlaylistService::errorOccurred, this,
            [this](const QString &message) {
              if (m_loadingPlaylist) {
                m_loadingPlaylist = false;
                emit loadingPlaylistChanged();
              }

              if (m_loadingLibraryPlaylists) {
                m_loadingLibraryPlaylists = false;
                emit loadingLibraryPlaylistsChanged();
              }

              emit statusChanged(QString("Ошибка плейлистов: %1").arg(message));
            });
  }

  if (m_playlistService != nullptr) {
    connect(m_playlistService, &PlaylistService::playlistCreated, this,
            [this](const QString &title) {
              emit statusChanged(QString("Плейлист «%1» создан").arg(title));

              if (!m_userId.isEmpty()) {
                loadUserPlaylists(m_userId);
              }
            });

    connect(m_playlistService, &PlaylistService::playlistDeleted, this, [this](int) {
      ;
      m_playlistModel->clear();
      emit statusChanged("Плейлист удалён");

      if (!m_userId.isEmpty()) {
        loadUserPlaylists(m_userId);
      }
    });

    connect(m_playlistService, &PlaylistService::playlistRenamed, this,
            [this](const QString &newTitle) {
              m_currentPlaylistTitle = newTitle;
              emit currentPlaylistChanged();
              emit statusChanged(QString("Плейлист переименован в «%1»").arg(newTitle));
            });

    connect(m_playlistService, &PlaylistService::tracksAdded, this, [this](int kind, int count) {
      emit statusChanged(QString("Добавлено треков: %1").arg(count));

      // Обновляем список пользовательских плейлистов,
      // чтобы счётчики треков в пикере были актуальными.
      if (!m_userId.isEmpty()) {
        loadUserPlaylists(m_userId);
      }

      // Перезагружаем открытый плейлист только если
      // изменения коснулись именно его — иначе не трогаем
      // текущий экран (например, добавляли в другой плейлист
      // или в персональную подборку).
      if (!m_userId.isEmpty()) {
        const int currentKind = m_playlistModel->kind();
        if (currentKind > 0 && currentKind == kind) {
          loadPlaylist(m_userId, currentKind);
        }
      }
    });

    connect(m_playlistService, &PlaylistService::tracksRemoved, this, [this](int kind, int count) {
      emit playlistTracksChanged();
      emit statusChanged(QString("Удалено треков: %1").arg(count));

      // Обновляем список пользовательских плейлистов,
      // чтобы счётчики треков были актуальными.
      if (!m_userId.isEmpty()) {
        loadUserPlaylists(m_userId);
      }

      if (!m_userId.isEmpty()) {
        const int currentKind = m_playlistModel->kind();
        if (currentKind > 0 && currentKind == kind) {
          loadPlaylist(m_userId, currentKind);
        }
      }
    });
  }

  if (m_likesService != nullptr) {
    connect(m_likesService, &LikesService::tracksReceived, this,
            [this](const QList<Track> &tracks) {
              m_loadingLikedTracks = false;
              emit loadingLikedTracksChanged();
              m_likedTracksModel->setTracks(tracks);
            });

    connect(m_likesService, &LikesService::loadingChanged, this, [this](bool loading) {
      m_loadingLikedTracks = loading;
      emit loadingLikedTracksChanged();
    });

    connect(m_likesService, &LikesService::errorOccurred, this, [this](const QString &message) {
      m_loadingLikedTracks = false;
      emit loadingLikedTracksChanged();
      emit statusChanged(QString("Ошибка лайков: %1").arg(message));
    });

    connect(m_likesService, &LikesService::albumsReceived, this,
            [this](const QList<Album> &albums) {
              m_loadingLikedAlbums = false;
              emit loadingLikedAlbumsChanged();
              m_likedAlbumsModel->setAlbums(albums);
            });

    connect(m_likesService, &LikesService::artistsReceived, this,
            [this](const QList<Artist> &artists) {
              m_loadingLikedArtists = false;
              emit loadingLikedArtistsChanged();
              m_likedArtistsModel->setArtists(artists);
            });

    connect(m_likesService, &LikesService::likeChanged, this,
            [this](const QString &trackId, bool liked) { setTrackLiked(trackId, liked); });

    connect(m_likesService, &LikesService::albumLikeChanged, this, [this](const QString &, bool) {
      ;

      if (!m_userId.isEmpty()) {
        loadLikedAlbums(m_userId);
      }
    });

    connect(m_likesService, &LikesService::artistLikeChanged, this, [this](const QString &, bool) {
      ;

      if (!m_userId.isEmpty()) {
        loadLikedArtists(m_userId);
      }
    });
  }

  if (m_artistService != nullptr) {
    connect(m_artistService, &ArtistService::artistReceived, this,
            [this](const ArtistDetails &artist) {
              m_loadingArtist = false;
              emit loadingArtistChanged();
              m_artistModel->setArtist(artist);
              m_currentArtistName = artist.name;
              m_currentArtistCoverUri = artist.coverUri;
              m_currentArtistGenres = artist.genres.join(", ");
              m_currentArtistTrackCount = artist.tracks.size();
              emit currentArtistChanged();
            });

    connect(m_artistService, &ArtistService::errorOccurred, this, [this](const QString &message) {
      m_loadingArtist = false;
      emit loadingArtistChanged();
      emit statusChanged(QString("Ошибка загрузки исполнителя: %1").arg(message));
    });
  }
}

void LibraryController::loadUserPlaylists(const QString &uid) {
  const QString userUid = uid.trimmed();

  if (userUid.isEmpty()) {
    emit statusChanged("UID пользователя не указан");
    return;
  }

  if (m_playlistService == nullptr) {
    emit statusChanged("Сервис плейлистов недоступен");
    return;
  }

  m_loadingLibraryPlaylists = true;
  emit loadingLibraryPlaylistsChanged();
  m_libraryPlaylistsModel->clear();
  m_playlistService->loadUserPlaylists(userUid);
}

void LibraryController::selectLibraryPlaylist(int index) {
  if (m_libraryPlaylistsModel == nullptr) {
    emit statusChanged("Модель плейлистов недоступна");
    return;
  }

  const PersonalPlaylist playlist = m_libraryPlaylistsModel->playlistAt(index);

  if (playlist.uid.isEmpty() || playlist.kind <= 0) {
    emit statusChanged("Некорректный плейлист");
    return;
  }

  loadPlaylist(playlist.uid, playlist.kind);
}

LibraryPlaylistsModel *LibraryController::libraryPlaylistsModel() const {
  return m_libraryPlaylistsModel;
}

bool LibraryController::isLoadingLibraryPlaylists() const {
  return m_loadingLibraryPlaylists;
}

void LibraryController::loadLikedTracks(const QString &uid) {
  const QString userUid = uid.trimmed();

  if (userUid.isEmpty()) {
    emit statusChanged("UID пользователя не указан");
    return;
  }

  if (m_likesService == nullptr) {
    emit statusChanged("Сервис лайков недоступен");
    return;
  }

  m_loadingLikedTracks = true;
  emit loadingLikedTracksChanged();
  m_likedTracksModel->clear();
  m_likesService->loadLikedTracks(userUid);
}

void LibraryController::selectLikedTrack(int index) {
  if (m_playbackController == nullptr) {
    emit statusChanged("PlaybackController недоступен");
    return;
  }

  if (m_likedTracksModel == nullptr) {
    emit statusChanged("Модель лайкнутых треков недоступна");
    return;
  }

  const Track track = m_likedTracksModel->trackAt(index);

  if (track.id.isEmpty()) {
    emit statusChanged("Некорректный лайкнутый трек");
    return;
  }

  const QList<Track> tracks = m_likedTracksModel->tracks();
  m_playbackController->playFromSource(tracks, index, "Понравившиеся треки", "likes");
}

LikedTracksModel *LibraryController::likedTracksModel() const {
  return m_likedTracksModel;
}

bool LibraryController::isLoadingLikedTracks() const {
  return m_loadingLikedTracks;
}

void LibraryController::loadLikedAlbums(const QString &uid) {
  const QString userUid = uid.trimmed();

  if (userUid.isEmpty()) {
    emit statusChanged("UID пользователя не указан");
    return;
  }

  if (m_likesService == nullptr) {
    emit statusChanged("Сервис лайков недоступен");
    return;
  }

  m_loadingLikedAlbums = true;
  emit loadingLikedAlbumsChanged();
  m_likedAlbumsModel->clear();
  m_likesService->loadLikedAlbums(userUid);
}

LikedAlbumsModel *LibraryController::likedAlbumsModel() const {
  return m_likedAlbumsModel;
}

bool LibraryController::isLoadingLikedAlbums() const {
  return m_loadingLikedAlbums;
}

void LibraryController::selectLikedAlbum(int index) {
  const Album album = m_likedAlbumsModel->albumAt(index);

  if (album.id.isEmpty()) {
    emit statusChanged("Некорректный альбом");
    return;
  }

  emit albumPageRequested(album.id);
}

void LibraryController::loadLikedArtists(const QString &uid) {
  const QString userUid = uid.trimmed();

  if (userUid.isEmpty()) {
    emit statusChanged("UID пользователя не указан");
    return;
  }

  if (m_likesService == nullptr) {
    emit statusChanged("Сервис лайков недоступен");
    return;
  }

  m_loadingLikedArtists = true;
  emit loadingLikedArtistsChanged();
  m_likedArtistsModel->clear();
  m_likesService->loadLikedArtists(userUid);
}

LikedArtistsModel *LibraryController::likedArtistsModel() const {
  return m_likedArtistsModel;
}

bool LibraryController::isLoadingLikedArtists() const {
  return m_loadingLikedArtists;
}

void LibraryController::selectLikedArtist(int index) {
  const Artist artist = m_likedArtistsModel->artistAt(index);

  if (artist.id.isEmpty()) {
    emit statusChanged("Некорректный исполнитель");
    return;
  }

  emit artistPageRequested(artist.id);
}

void LibraryController::loadPlaylist(const QString &uid, int kind) {
  const QString playlistUid = uid.trimmed();

  if (playlistUid.isEmpty() || kind <= 0) {
    emit statusChanged("Некорректный плейлист");
    return;
  }

  if (m_playlistService == nullptr) {
    emit statusChanged("Сервис плейлистов недоступен");
    return;
  }

  m_loadingPlaylist = true;
  emit loadingPlaylistChanged();
  m_playlistModel->clear();
  m_similarPlaylists.clear();
  emit similarPlaylistsChanged();
  m_currentPlaylistTitle.clear();
  m_currentPlaylistCoverUri.clear();
  m_currentPlaylistTrackCount = 0;
  emit currentPlaylistChanged();
  m_playlistService->loadPlaylist(playlistUid, kind);
}

void LibraryController::selectPlaylistTrack(int index) {
  if (m_playbackController == nullptr) {
    emit statusChanged("PlaybackController недоступен");
    return;
  }

  const QList<Track> tracks = m_playlistModel->tracks();

  if (index < 0 || index >= tracks.size()) {
    emit statusChanged("Некорректный индекс трека");
    return;
  }

  const Track track = tracks.at(index);

  if (track.id.isEmpty()) {
    emit statusChanged("Некорректный трек плейлиста");
    return;
  }

  m_playbackController->playFromSource(tracks, index, m_currentPlaylistTitle, "playlist");
}

PlaylistModel *LibraryController::playlistModel() const {
  return m_playlistModel;
}

bool LibraryController::isLoadingPlaylist() const {
  return m_loadingPlaylist;
}

QString LibraryController::currentPlaylistTitle() const {
  return m_currentPlaylistTitle;
}

QString LibraryController::currentPlaylistCoverUri() const {
  return m_currentPlaylistCoverUri;
}

int LibraryController::currentPlaylistTrackCount() const {
  return m_currentPlaylistTrackCount;
}

int LibraryController::currentPlaylistKind() const {
  if (m_playlistModel == nullptr) {
    return 0;
  }
  return m_playlistModel->kind();
}

QVariantList LibraryController::similarPlaylists() const {
  return m_similarPlaylists;
}

void LibraryController::setSimilarPlaylistsFallback(const QVariantList &playlists) {
  m_similarPlaylists = playlists;
  emit similarPlaylistsChanged();
}

void LibraryController::loadArtist(const QString &id) {
  const QString artistId = id.trimmed();

  if (artistId.isEmpty()) {
    emit statusChanged("ID исполнителя не указан");
    return;
  }

  if (m_artistService == nullptr) {
    emit statusChanged("Сервис исполнителя недоступен");
    return;
  }

  m_loadingArtist = true;
  emit loadingArtistChanged();
  m_artistModel->clear();
  m_currentArtistName.clear();
  m_currentArtistCoverUri.clear();
  m_currentArtistGenres.clear();
  m_currentArtistTrackCount = 0;
  emit currentArtistChanged();
  m_artistService->loadArtist(artistId);
}

void LibraryController::selectArtistTrack(int index) {
  if (m_playbackController == nullptr) {
    emit statusChanged("PlaybackController недоступен");
    return;
  }

  const QList<Track> tracks = m_artistModel->tracks();

  if (index < 0 || index >= tracks.size()) {
    emit statusChanged("Некорректный индекс трека исполнителя");
    return;
  }

  const Track track = tracks.at(index);

  if (track.id.isEmpty()) {
    emit statusChanged("Некорректный трек исполнителя");
    return;
  }

  m_playbackController->playFromSource(tracks, index, m_currentArtistName, "artist");
}

ArtistModel *LibraryController::artistModel() const {
  return m_artistModel;
}

bool LibraryController::isLoadingArtist() const {
  return m_loadingArtist;
}

QString LibraryController::currentArtistName() const {
  return m_currentArtistName;
}

QString LibraryController::currentArtistCoverUri() const {
  return m_currentArtistCoverUri;
}

QString LibraryController::currentArtistGenres() const {
  return m_currentArtistGenres;
}

int LibraryController::currentArtistTrackCount() const {
  return m_currentArtistTrackCount;
}

void LibraryController::setTrackLiked(const QString &trackId, bool liked) {
  const QString id = trackId.trimmed();

  if (id.isEmpty()) {
    return;
  }

  if (m_playlistModel != nullptr) {
    m_playlistModel->setTrackLiked(id, liked);
  }

  if (liked) {
    m_likedTracksModel->setTrackLiked(id, true);
  } else {
    m_likedTracksModel->removeTrack(id);
  }
}

void LibraryController::setUserId(const QString &uid) {
  m_userId = uid.trimmed();
}

void LibraryController::createPlaylist(const QString &title) {
  if (m_playlistService == nullptr || m_userId.isEmpty()) {
    emit statusChanged("Не удалось создать плейлист");
    return;
  }

  m_playlistService->createPlaylist(m_userId, title);
}

void LibraryController::deleteCurrentPlaylist() {
  if (m_playlistService == nullptr || m_userId.isEmpty()) {
    emit statusChanged("Не удалось удалить плейлист");
    return;
  }

  const int kind = m_playlistModel->kind();

  if (kind <= 0) {
    emit statusChanged("Плейлист не загружен");
    return;
  }

  m_playlistService->deletePlaylist(m_userId, kind);
}

void LibraryController::renameCurrentPlaylist(const QString &newTitle) {
  if (m_playlistService == nullptr || m_userId.isEmpty()) {
    emit statusChanged("Не удалось переименовать плейлист");
    return;
  }

  const int kind = m_playlistModel->kind();

  if (kind <= 0) {
    emit statusChanged("Плейлист не загружен");
    return;
  }

  m_playlistService->renamePlaylist(m_userId, kind, newTitle);
}

void LibraryController::removeTrackFromPlaylist(int index) {
  if (m_playlistService == nullptr || m_userId.isEmpty() || m_playlistModel == nullptr) {
    emit statusChanged("Не удалось удалить трек");
    return;
  }

  const int kind = m_playlistModel->kind();

  if (kind <= 0) {
    return;
  }

  const Track track = m_playlistModel->trackAt(index);

  if (track.id.isEmpty()) {
    return;
  }

  m_playlistService->removeTrackFromPlaylist(m_userId, kind, index, m_playlistModel->revision());
}

void LibraryController::addTrackToPlaylist(int kind, const QString &trackId, const QString &albumId,
                                           int revision) {
  if (m_playlistService == nullptr || m_userId.isEmpty()) {
    emit statusChanged("Не удалось добавить трек");
    return;
  }

  if (kind <= 0 || trackId.trimmed().isEmpty() || albumId.trimmed().isEmpty()) {
    return;
  }

  m_playlistService->addTracksToPlaylist(m_userId, kind, {trackId.trimmed()}, {albumId.trimmed()},
                                         revision);
}