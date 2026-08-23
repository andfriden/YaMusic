#pragma once

#include <QObject>
#include <QString>

#include "../Playback/PlaybackController.h"
#include "../Yandex/Catalog/AlbumModel.h"
#include "../Yandex/Catalog/ArtistModel.h"
#include "../Yandex/Personal/PlaylistModel.h"
#include "../Yandex/Personal/PlaylistService.h"
#include "../Yandex/Catalog/AlbumService.h"
#include "../Yandex/Catalog/ArtistService.h"

#include "../Models/RecentListeningItem.h"

class LibraryController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool loadingPlaylist
        READ isLoadingPlaylist
        NOTIFY loadingPlaylistChanged)

    Q_PROPERTY(
        bool loadingAlbum
        READ isLoadingAlbum
        NOTIFY loadingAlbumChanged)

    Q_PROPERTY(
        bool loadingArtist
        READ isLoadingArtist
        NOTIFY loadingArtistChanged)

    Q_PROPERTY(
        QString currentPlaylistTitle
        READ currentPlaylistTitle
        NOTIFY currentPlaylistChanged)

    Q_PROPERTY(
        int currentPlaylistTrackCount
        READ currentPlaylistTrackCount
        NOTIFY currentPlaylistChanged)

    Q_PROPERTY(
        QString currentAlbumTitle
        READ currentAlbumTitle
        NOTIFY currentAlbumChanged)

    Q_PROPERTY(
        int currentAlbumTrackCount
        READ currentAlbumTrackCount
        NOTIFY currentAlbumChanged)

    Q_PROPERTY(
        QString currentAlbumCoverUri
        READ currentAlbumCoverUri
        NOTIFY currentAlbumChanged)

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
        PlaylistModel *playlistModel
        READ playlistModel
        CONSTANT)

    Q_PROPERTY(
        AlbumModel *albumModel
        READ albumModel
        CONSTANT)

    Q_PROPERTY(
        ArtistModel *artistModel
        READ artistModel
        CONSTANT)

public:
    explicit LibraryController(
        PlaylistService *playlistService,
        AlbumService *albumService,
        ArtistService *artistService,
        PlaybackController *playbackController,
        QObject *parent = nullptr);

    void loadPlaylist(
        const QString &uid,
        int kind);

    void loadAlbum(
        const QString &id);

    void loadArtist(
        const QString &id);

    void selectPlaylistTrack(
        int index);

    void selectAlbumTrack(
        int index);

    void selectArtistTrack(
        int index);

    void selectRecentListening(
        const RecentListeningItem &item);

    PlaylistModel *playlistModel() const;

    AlbumModel *albumModel() const;

    ArtistModel *artistModel() const;

    bool isLoadingPlaylist() const;

    bool isLoadingAlbum() const;

    bool isLoadingArtist() const;

    QString currentPlaylistTitle() const;

    int currentPlaylistTrackCount() const;

    QString currentAlbumTitle() const;

    int currentAlbumTrackCount() const;

    QString currentAlbumCoverUri() const;

    QString currentArtistName() const;

    QString currentArtistCoverUri() const;

    QString currentArtistGenres() const;

    int currentArtistTrackCount() const;

signals:
    void statusChanged(
        const QString &message);

    void loadingPlaylistChanged();

    void loadingAlbumChanged();

    void loadingArtistChanged();

    void currentPlaylistChanged();

    void currentAlbumChanged();

    void currentArtistChanged();

private:
    void loadPlaylistInternal(
        const QString &uid,
        int kind,
        const QString &statusTitle);

    void loadAlbumInternal(
        const QString &id,
        const QString &statusTitle);

private:
    PlaylistService *m_playlistService = nullptr;

    AlbumService *m_albumService = nullptr;

    ArtistService *m_artistService = nullptr;

    PlaybackController *m_playbackController = nullptr;

    PlaylistModel *m_playlistModel = nullptr;

    AlbumModel *m_albumModel = nullptr;

    ArtistModel *m_artistModel = nullptr;

    bool m_loadingPlaylist = false;

    bool m_loadingAlbum = false;

    bool m_loadingArtist = false;

    QString m_currentPlaylistTitle;

    int m_currentPlaylistTrackCount = 0;

    QString m_currentAlbumTitle;

    int m_currentAlbumTrackCount = 0;

    QString m_currentAlbumCoverUri;

    QString m_currentArtistName;

    QString m_currentArtistCoverUri;

    QString m_currentArtistGenres;

    int m_currentArtistTrackCount = 0;
};