#pragma once

#include <QObject>
#include <QString>

#include "../Models/Track.h"

#include "../Yandex/Catalog/ArtistAlbumsModel.h"
#include "../Yandex/Catalog/ArtistModel.h"
#include "../Yandex/Catalog/SimilarArtistsModel.h"

class ArtistService;
class PlaybackController;

class ArtistController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool loading
        READ isLoading
        NOTIFY loadingChanged)

    Q_PROPERTY(
        ArtistModel *artistModel
        READ artistModel
        CONSTANT)

    Q_PROPERTY(
        ArtistAlbumsModel *albumsModel
        READ albumsModel
        CONSTANT)

    Q_PROPERTY(
        SimilarArtistsModel *similarArtistsModel
        READ similarArtistsModel
        CONSTANT)

    Q_PROPERTY(
        QString artistId
        READ artistId
        NOTIFY artistChanged)

    Q_PROPERTY(
        QString artistName
        READ artistName
        NOTIFY artistChanged)

    Q_PROPERTY(
        QString artistCoverUri
        READ artistCoverUri
        NOTIFY artistChanged)

    Q_PROPERTY(
        QString artistDescription
        READ artistDescription
        NOTIFY artistChanged)

    Q_PROPERTY(
        QString artistGenres
        READ artistGenres
        NOTIFY artistChanged)

    Q_PROPERTY(
        QString newReleaseId
        READ newReleaseId
        NOTIFY artistChanged)

    Q_PROPERTY(
        QString newReleaseTitle
        READ newReleaseTitle
        NOTIFY artistChanged)

    Q_PROPERTY(
        QString newReleaseCoverUri
        READ newReleaseCoverUri
        NOTIFY artistChanged)

    Q_PROPERTY(
        int newReleaseYear
        READ newReleaseYear
        NOTIFY artistChanged)

public:
    explicit ArtistController(
        ArtistService *artistService,
        PlaybackController *playbackController,
        QObject *parent = nullptr);

    void loadArtist(
        const QString &id);

    Q_INVOKABLE void selectTrack(
        int index);

    Q_INVOKABLE void selectSimilarArtist(
        int index);

    Q_INVOKABLE void playArtist();

    ArtistModel *artistModel() const;

    ArtistAlbumsModel *albumsModel() const;

    SimilarArtistsModel *similarArtistsModel() const;

    bool isLoading() const;

    QString artistId() const;

    QString artistName() const;

    QString artistCoverUri() const;

    QString artistDescription() const;

    QString artistGenres() const;

    QString newReleaseId() const;

    QString newReleaseTitle() const;

    QString newReleaseCoverUri() const;

    int newReleaseYear() const;

signals:
    void statusChanged(
        const QString &message);

    void loadingChanged();

    void artistChanged();

    void trackSelected(
        const Track &track);

    void similarArtistSelected(
        const QString &artistId);

private:
    ArtistService *m_artistService =
        nullptr;

    PlaybackController *m_playbackController =
        nullptr;

    ArtistModel *m_artistModel =
        nullptr;

    ArtistAlbumsModel *m_albumsModel =
        nullptr;

    SimilarArtistsModel *m_similarArtistsModel =
        nullptr;

    bool m_loading =
        false;

    QString m_artistId;
    QString m_artistName;
    QString m_artistCoverUri;
    QString m_artistDescription;
    QString m_artistGenres;

    Album m_newRelease;
};