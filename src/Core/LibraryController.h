#pragma once

#include "../Playback/PlaybackController.h"
#include <QObject>
#include <QString>
#include <QVariantList>

#include "../Yandex/Catalog/ArtistModel.h"
#include "../Yandex/Catalog/ArtistService.h"
#include "../Yandex/Personal/LibraryPlaylistsModel.h"
#include "../Yandex/Personal/LikedAlbumsModel.h"
#include "../Yandex/Personal/LikedArtistsModel.h"
#include "../Yandex/Personal/LikedTracksModel.h"
#include "../Yandex/Personal/PlaylistModel.h"
#include "../Yandex/Personal/PlaylistService.h"

class LikesService;

class LibraryController : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool loadingLibraryPlaylists READ isLoadingLibraryPlaylists NOTIFY
                 loadingLibraryPlaylistsChanged)

  Q_PROPERTY(LibraryPlaylistsModel *libraryPlaylistsModel READ libraryPlaylistsModel CONSTANT)

  Q_PROPERTY(bool loadingLikedTracks READ isLoadingLikedTracks NOTIFY loadingLikedTracksChanged)

  Q_PROPERTY(LikedTracksModel *likedTracksModel READ likedTracksModel CONSTANT)

  Q_PROPERTY(bool loadingLikedAlbums READ isLoadingLikedAlbums NOTIFY loadingLikedAlbumsChanged)

  Q_PROPERTY(LikedAlbumsModel *likedAlbumsModel READ likedAlbumsModel CONSTANT)

  Q_PROPERTY(bool loadingLikedArtists READ isLoadingLikedArtists NOTIFY loadingLikedArtistsChanged)

  Q_PROPERTY(LikedArtistsModel *likedArtistsModel READ likedArtistsModel CONSTANT)

  Q_PROPERTY(bool loadingPlaylist READ isLoadingPlaylist NOTIFY loadingPlaylistChanged)

  Q_PROPERTY(QString currentPlaylistTitle READ currentPlaylistTitle NOTIFY currentPlaylistChanged)

  Q_PROPERTY(
      QString currentPlaylistCoverUri READ currentPlaylistCoverUri NOTIFY currentPlaylistChanged)

  Q_PROPERTY(
      int currentPlaylistTrackCount READ currentPlaylistTrackCount NOTIFY currentPlaylistChanged)

  Q_PROPERTY(QVariantList similarPlaylists READ similarPlaylists NOTIFY similarPlaylistsChanged)

  Q_PROPERTY(PlaylistModel *playlistModel READ playlistModel CONSTANT)

  Q_PROPERTY(bool loadingArtist READ isLoadingArtist NOTIFY loadingArtistChanged)

  Q_PROPERTY(QString currentArtistName READ currentArtistName NOTIFY currentArtistChanged)

  Q_PROPERTY(QString currentArtistCoverUri READ currentArtistCoverUri NOTIFY currentArtistChanged)

  Q_PROPERTY(QString currentArtistGenres READ currentArtistGenres NOTIFY currentArtistChanged)

  Q_PROPERTY(int currentArtistTrackCount READ currentArtistTrackCount NOTIFY currentArtistChanged)

  Q_PROPERTY(ArtistModel *artistModel READ artistModel CONSTANT)

public:
  explicit LibraryController(PlaylistService *playlistService, ArtistService *artistService,
                             LikesService *likesService, PlaybackController *playbackController,
                             QObject *parent = nullptr);

  void setUserId(const QString &uid);

  void loadUserPlaylists(const QString &uid);

  void selectLibraryPlaylist(int index);

  LibraryPlaylistsModel *libraryPlaylistsModel() const;

  bool isLoadingLibraryPlaylists() const;

  void loadLikedTracks(const QString &uid);

  void selectLikedTrack(int index);

  LikedTracksModel *likedTracksModel() const;

  bool isLoadingLikedTracks() const;

  void loadLikedAlbums(const QString &uid);

  LikedAlbumsModel *likedAlbumsModel() const;

  bool isLoadingLikedAlbums() const;

  void selectLikedAlbum(int index);

  void loadLikedArtists(const QString &uid);

  LikedArtistsModel *likedArtistsModel() const;

  bool isLoadingLikedArtists() const;

  void selectLikedArtist(int index);

  void loadPlaylist(const QString &uid, int kind);

  void selectPlaylistTrack(int index);

  PlaylistModel *playlistModel() const;

  bool isLoadingPlaylist() const;

  QString currentPlaylistTitle() const;

  QString currentPlaylistCoverUri() const;

  int currentPlaylistTrackCount() const;

  int currentPlaylistKind() const;

  QVariantList similarPlaylists() const;

  // Заполняет похожие плейлисты из лендинга,
  // если сервер не вернул родные similar-entities.
  // Вызывать после loadPlaylist().
  void setSimilarPlaylistsFallback(const QVariantList &playlists);

  void loadArtist(const QString &id);

  void selectArtistTrack(int index);

  ArtistModel *artistModel() const;

  bool isLoadingArtist() const;

  QString currentArtistName() const;

  QString currentArtistCoverUri() const;

  QString currentArtistGenres() const;

  int currentArtistTrackCount() const;

  void setTrackLiked(const QString &trackId, bool liked);

  Q_INVOKABLE void createPlaylist(const QString &title);

  Q_INVOKABLE void deleteCurrentPlaylist();

  Q_INVOKABLE void renameCurrentPlaylist(const QString &newTitle);

  Q_INVOKABLE void removeTrackFromPlaylist(int index);

  Q_INVOKABLE void addTrackToPlaylist(int kind, const QString &trackId, const QString &albumId,
                                      int revision = 1);

signals:

  void playlistTracksChanged();

  void statusChanged(const QString &message);

  void albumPageRequested(const QString &albumId);

  void artistPageRequested(const QString &artistId);

  void loadingLibraryPlaylistsChanged();

  void loadingLikedTracksChanged();

  void loadingLikedAlbumsChanged();

  void likedAlbumsChanged();

  void loadingLikedArtistsChanged();

  void likedArtistsChanged();

  void loadingPlaylistChanged();

  void currentPlaylistChanged();

  void similarPlaylistsChanged();

  void loadingArtistChanged();

  void currentArtistChanged();

private:
  PlaylistService *m_playlistService = nullptr;

  ArtistService *m_artistService = nullptr;

  LikesService *m_likesService = nullptr;

  PlaybackController *m_playbackController = nullptr;

  LibraryPlaylistsModel *m_libraryPlaylistsModel = nullptr;

  LikedTracksModel *m_likedTracksModel = nullptr;

  LikedAlbumsModel *m_likedAlbumsModel = nullptr;

  LikedArtistsModel *m_likedArtistsModel = nullptr;

  PlaylistModel *m_playlistModel = nullptr;

  ArtistModel *m_artistModel = nullptr;

  bool m_loadingLibraryPlaylists = false;

  bool m_loadingLikedTracks = false;

  bool m_loadingLikedAlbums = false;

  bool m_loadingLikedArtists = false;

  bool m_loadingPlaylist = false;

  QString m_currentPlaylistTitle;

  QString m_currentPlaylistCoverUri;

  int m_currentPlaylistTrackCount = 0;

  QVariantList m_similarPlaylists;

  bool m_loadingArtist = false;

  QString m_currentArtistName;

  QString m_currentArtistCoverUri;

  QString m_currentArtistGenres;

  int m_currentArtistTrackCount = 0;

  QString m_userId;
};