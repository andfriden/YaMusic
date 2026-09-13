#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include "../Playback/PlaybackController.h"

#include "../Yandex/Catalog/ArtistModel.h"
#include "../Yandex/Catalog/ArtistService.h"
#include "../Yandex/Personal/LibraryPlaylistsModel.h"
#include "../Yandex/Personal/LikedAlbumsModel.h"
#include "../Yandex/Personal/LikedArtistsModel.h"
#include "../Yandex/Personal/LikedTracksModel.h"
#include "../Yandex/Personal/PlaylistModel.h"
#include "../Yandex/Personal/PlaylistService.h"

class LikesService;

class LibraryController : public QObject
{
    Q_OBJECT

    // Library playlists

    Q_PROPERTY(
        bool loadingLibraryPlaylists
        READ isLoadingLibraryPlaylists
        NOTIFY loadingLibraryPlaylistsChanged)

    Q_PROPERTY(
        LibraryPlaylistsModel *libraryPlaylistsModel
        READ libraryPlaylistsModel
        CONSTANT)

    // Liked tracks

    Q_PROPERTY(
        bool loadingLikedTracks
        READ isLoadingLikedTracks
        NOTIFY loadingLikedTracksChanged)

    Q_PROPERTY(
        LikedTracksModel *likedTracksModel
        READ likedTracksModel
        CONSTANT)

    // Liked albums

    Q_PROPERTY(
        bool loadingLikedAlbums
        READ isLoadingLikedAlbums
        NOTIFY loadingLikedAlbumsChanged)

    Q_PROPERTY(
        LikedAlbumsModel *likedAlbumsModel
        READ likedAlbumsModel
        CONSTANT)

    // Liked artists

    Q_PROPERTY(
        bool loadingLikedArtists
        READ isLoadingLikedArtists
        NOTIFY loadingLikedArtistsChanged)

    Q_PROPERTY(
        LikedArtistsModel *likedArtistsModel
        READ likedArtistsModel
        CONSTANT)

    // Playlist state

    Q_PROPERTY(
        bool loadingPlaylist
        READ isLoadingPlaylist
        NOTIFY loadingPlaylistChanged)

    Q_PROPERTY(
        QString currentPlaylistTitle
        READ currentPlaylistTitle
        NOTIFY currentPlaylistChanged)

    Q_PROPERTY(
        QString currentPlaylistCoverUri
        READ currentPlaylistCoverUri
        NOTIFY currentPlaylistChanged)

    Q_PROPERTY(
        int currentPlaylistTrackCount
        READ currentPlaylistTrackCount
        NOTIFY currentPlaylistChanged)

    Q_PROPERTY(
        QVariantList similarPlaylists
        READ similarPlaylists
        NOTIFY similarPlaylistsChanged)

    Q_PROPERTY(
        PlaylistModel *playlistModel
        READ playlistModel
        CONSTANT)

    // Artist state

    Q_PROPERTY(
        bool loadingArtist
        READ isLoadingArtist
        NOTIFY loadingArtistChanged)

    Q_PROPERTY(
        QString currentArtistName
        READ currentArtistName
        NOTIFY currentArtistChanged)

    Q_PROPERTY(
        QString currentArtistCoverUri
        READ currentArtistCoverUri
        NOTIFY currentArtistChanged)

    Q_PROPERTY(
        QString currentArtistGenres
        READ currentArtistGenres
        NOTIFY currentArtistChanged)

    Q_PROPERTY(
        int currentArtistTrackCount
        READ currentArtistTrackCount
        NOTIFY currentArtistChanged)

    Q_PROPERTY(
        ArtistModel *artistModel
        READ artistModel
        CONSTANT)

public:

    explicit LibraryController(
        PlaylistService *playlistService,
        ArtistService *artistService,
        LikesService *likesService,
        PlaybackController *playbackController,
        QObject *parent = nullptr);

    // Library playlists

    void loadUserPlaylists(
        const QString &uid);

    void selectLibraryPlaylist(
        int index);

    LibraryPlaylistsModel *
    libraryPlaylistsModel() const;

    bool isLoadingLibraryPlaylists() const;

    // Liked tracks

    void loadLikedTracks(
        const QString &uid);

    void selectLikedTrack(
        int index);

    LikedTracksModel *
    likedTracksModel() const;

    bool isLoadingLikedTracks() const;

    // Liked albums

    void loadLikedAlbums(
        const QString &uid);

    LikedAlbumsModel *
    likedAlbumsModel() const;

    bool isLoadingLikedAlbums() const;

    void selectLikedAlbum(
        int index);

    // Liked artists

    void loadLikedArtists(
        const QString &uid);

    LikedArtistsModel *
    likedArtistsModel() const;

    bool isLoadingLikedArtists() const;

    void selectLikedArtist(
        int index);

    // Playlist

    void loadPlaylist(
        const QString &uid,
        int kind);

    void selectPlaylistTrack(
        int index);

    PlaylistModel *
    playlistModel() const;

    bool isLoadingPlaylist() const;

    QString currentPlaylistTitle() const;

    QString currentPlaylistCoverUri() const;

    int currentPlaylistTrackCount() const;

    QVariantList similarPlaylists() const;

    // Artist

    void loadArtist(
        const QString &id);

    void selectArtistTrack(
        int index);

    ArtistModel *
    artistModel() const;

    bool isLoadingArtist() const;

    QString currentArtistName() const;

    QString currentArtistCoverUri() const;

    QString currentArtistGenres() const;

    int currentArtistTrackCount() const;

    // Likes

    void setTrackLiked(
        const QString &trackId,
        bool liked);

signals:

    // Common

    void statusChanged(
        const QString &message);

    void albumPageRequested(
        const QString &albumId);

    void artistPageRequested(
        const QString &artistId);

    // Library playlists

    void loadingLibraryPlaylistsChanged();

    // Liked tracks

    void loadingLikedTracksChanged();

    // Liked albums

    void loadingLikedAlbumsChanged();

    void likedAlbumsChanged();

    // Liked artists

    void loadingLikedArtistsChanged();

    void likedArtistsChanged();

    // Playlist

    void loadingPlaylistChanged();

    void currentPlaylistChanged();

    void similarPlaylistsChanged();

    // Artist

    void loadingArtistChanged();

    void currentArtistChanged();

private:

    // Services

    PlaylistService *
        m_playlistService = nullptr;

    ArtistService *
        m_artistService = nullptr;

    LikesService *
        m_likesService = nullptr;

    PlaybackController *
        m_playbackController = nullptr;

    // Models

    LibraryPlaylistsModel *
        m_libraryPlaylistsModel = nullptr;

    LikedTracksModel *
        m_likedTracksModel = nullptr;

    LikedAlbumsModel *
        m_likedAlbumsModel = nullptr;

    LikedArtistsModel *
        m_likedArtistsModel = nullptr;

    PlaylistModel *
        m_playlistModel = nullptr;

    ArtistModel *
        m_artistModel = nullptr;

    // Library state

    bool m_loadingLibraryPlaylists =
        false;

    bool m_loadingLikedTracks =
        false;

    bool m_loadingLikedAlbums =
        false;

    bool m_loadingLikedArtists =
        false;

    // Playlist state

    bool m_loadingPlaylist =
        false;

    QString m_currentPlaylistTitle;

    QString m_currentPlaylistCoverUri;

    int m_currentPlaylistTrackCount =
        0;

    QVariantList m_similarPlaylists;

    // Artist state

    bool m_loadingArtist =
        false;

    QString m_currentArtistName;

    QString m_currentArtistCoverUri;

    QString m_currentArtistGenres;

    int m_currentArtistTrackCount =
        0;
};