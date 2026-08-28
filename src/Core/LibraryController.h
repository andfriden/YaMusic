#pragma once

#include <QObject>
#include <QString>

#include "../Playback/PlaybackController.h"

#include "../Yandex/Catalog/ArtistModel.h"
#include "../Yandex/Catalog/ArtistService.h"

#include "../Yandex/Personal/PlaylistModel.h"
#include "../Yandex/Personal/PlaylistService.h"


class LibraryController : public QObject
{
    Q_OBJECT


    // =============================================================
    // Playlist state
    // =============================================================

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


    // =============================================================
    // Artist state
    // =============================================================

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


    // =============================================================
    // Models
    // =============================================================

    Q_PROPERTY(
        PlaylistModel *playlistModel
        READ playlistModel
        CONSTANT)


    Q_PROPERTY(
        ArtistModel *artistModel
        READ artistModel
        CONSTANT)


public:

    explicit LibraryController(
        PlaylistService *playlistService,
        ArtistService *artistService,
        PlaybackController *playbackController,
        QObject *parent = nullptr);


    // =============================================================
    // Playlist
    // =============================================================

    void loadPlaylist(
        const QString &uid,
        int kind);


    void selectPlaylistTrack(
        int index);


    // =============================================================
    // Artist
    // =============================================================

    void loadArtist(
        const QString &id);


    void selectArtistTrack(
        int index);


    // =============================================================
    // Models
    // =============================================================

    PlaylistModel *playlistModel() const;

    ArtistModel *artistModel() const;


    // =============================================================
    // Loading state
    // =============================================================

    bool isLoadingPlaylist() const;

    bool isLoadingArtist() const;


    // =============================================================
    // Current playlist
    // =============================================================

    QString currentPlaylistTitle() const;

    QString currentPlaylistCoverUri() const;

    int currentPlaylistTrackCount() const;


    // =============================================================
    // Current artist
    // =============================================================

    QString currentArtistName() const;

    QString currentArtistCoverUri() const;

    QString currentArtistGenres() const;

    int currentArtistTrackCount() const;


signals:

    void statusChanged(
        const QString &message);


    void loadingPlaylistChanged();

    void loadingArtistChanged();


    void currentPlaylistChanged();

    void currentArtistChanged();


private:

    PlaylistService *
        m_playlistService = nullptr;


    ArtistService *
        m_artistService = nullptr;


    PlaybackController *
        m_playbackController = nullptr;


    PlaylistModel *
        m_playlistModel = nullptr;


    ArtistModel *
        m_artistModel = nullptr;


    // =============================================================
    // Loading state
    // =============================================================

    bool m_loadingPlaylist = false;

    bool m_loadingArtist = false;


    // =============================================================
    // Current playlist
    // =============================================================

    QString m_currentPlaylistTitle;

    QString m_currentPlaylistCoverUri;

    int m_currentPlaylistTrackCount = 0;


    // =============================================================
    // Current artist
    // =============================================================

    QString m_currentArtistName;

    QString m_currentArtistCoverUri;

    QString m_currentArtistGenres;

    int m_currentArtistTrackCount = 0;
};