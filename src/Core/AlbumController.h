#pragma once

#include <QObject>
#include <QString>

#include "../Models/Track.h"
#include "../Yandex/Catalog/AlbumModel.h"
#include "../Yandex/Catalog/ArtistAlbumsModel.h"

class AlbumService;
class ArtistService;
class PlaybackController;

class AlbumController final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool loading
        READ isLoading
        NOTIFY loadingChanged)

    Q_PROPERTY(
        AlbumModel *albumModel
        READ albumModel
        CONSTANT)

    Q_PROPERTY(
        ArtistAlbumsModel *otherAlbumsModel
        READ otherAlbumsModel
        CONSTANT)

    Q_PROPERTY(
        QString albumId
        READ albumId
        NOTIFY albumChanged)

    Q_PROPERTY(
        QString currentAlbumTitle
        READ albumTitle
        NOTIFY albumChanged)

    Q_PROPERTY(
        QString currentAlbumCoverUri
        READ albumCoverUri
        NOTIFY albumChanged)

    Q_PROPERTY(
        int currentAlbumTrackCount
        READ albumTrackCount
        NOTIFY albumChanged)

public:
    explicit AlbumController(
        AlbumService *albumService,
        ArtistService *artistService,
        PlaybackController *playbackController,
        QObject *parent = nullptr);

    void loadAlbum(
        const QString &id);

    Q_INVOKABLE void selectAlbumTrack(
        int index);

    Q_INVOKABLE void playAlbum();

    AlbumModel *
    albumModel() const;

    ArtistAlbumsModel *
    otherAlbumsModel() const;

    bool isLoading() const;

    QString albumId() const;

    QString albumTitle() const;

    QString albumCoverUri() const;

    int albumTrackCount() const;

signals:
    void loadingChanged();

    void albumChanged();

    void statusChanged(
        const QString &message);

    void trackSelected(
        const Track &track);

private:
    void loadOtherAlbumsForCurrentArtist(
        const AlbumDetails &album);

    AlbumService *
        m_albumService =
            nullptr;

    ArtistService *
        m_artistService =
            nullptr;

    PlaybackController *
        m_playbackController =
            nullptr;

    AlbumModel *
        m_albumModel =
            nullptr;

    ArtistAlbumsModel *
        m_otherAlbumsModel =
            nullptr;

    bool m_loading =
        false;

    QString m_albumId;

    QString m_currentArtistId;
};