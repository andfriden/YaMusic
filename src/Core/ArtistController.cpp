#include "ArtistController.h"

#include "../Playback/PlaybackController.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Catalog/ArtistService.h"

#include <QDebug>

ArtistController::ArtistController(
    ArtistService *artistService,
    PlaybackController *playbackController,
    QObject *parent)
    : QObject(parent)
    , m_artistService(
          artistService)
    , m_playbackController(
          playbackController)
    , m_artistModel(
          new ArtistModel(this))
    , m_albumsModel(
          new ArtistAlbumsModel(this))
    , m_similarArtistsModel(
          new SimilarArtistsModel(this))
{
    if (
        m_artistService == nullptr
    ) {
        qDebug()
            << "ArtistController:"
            << "ArtistService is null";

        return;
    }

    connect(
        m_artistService,
        &ArtistService::artistReceived,
        this,
        [this](
            const ArtistDetails &artist) {

            m_loading = false;

            m_artistId =
                artist.id;

            m_artistName =
                artist.name;

            /*
             * ArtistService may return an empty
             * artist.coverUri.
             *
             * Use the strongest available fallback:
             *
             * 1. artist.coverUri
             * 2. first track cover
             * 3. first popular album cover
             */
            m_artistCoverUri =
                artist.coverUri;

            if (
                m_artistCoverUri.isEmpty()
            ) {
                for (
                    const Track &track :
                    artist.tracks
                ) {
                    if (
                        !track.coverUri.isEmpty()
                    ) {
                        m_artistCoverUri =
                            track.coverUri;

                        break;
                    }
                }
            }

            if (
                m_artistCoverUri.isEmpty()
            ) {
                for (
                    const Album &album :
                    artist.popularAlbums
                ) {
                    if (
                        !album.coverUri.isEmpty()
                    ) {
                        m_artistCoverUri =
                            album.coverUri;

                        break;
                    }
                }
            }

            m_artistDescription =
                artist.description;

            m_artistGenres =
                artist.genres.join(
                    ", ");

            m_newRelease =
                artist.newRelease;

            m_artistModel
                ->setArtist(
                    artist);

            m_albumsModel
                ->setAlbums(
                    artist.popularAlbums);

            m_similarArtistsModel
                ->setArtists(
                    artist.similarArtists);

            qDebug()
                << "ArtistController loaded:"
                << artist.name
                << "| id:"
                << artist.id
                << "| cover:"
                << m_artistCoverUri
                << "| tracks:"
                << artist.tracks.size()
                << "| albums:"
                << artist.popularAlbums.size()
                << "| similar:"
                << artist.similarArtists.size();

            if (
                m_artistCoverUri.isEmpty()
            ) {
                qDebug()
                    << "ArtistController:"
                    << "artist artwork is still empty"
                    << "| artist:"
                    << artist.name;
            }

            emit loadingChanged();

            emit artistChanged();

            emit statusChanged(
                QString(
                    "Исполнитель загружен: %1")
                    .arg(
                        artist.name));
        });

    connect(
        m_artistService,
        &ArtistService::errorOccurred,
        this,
        [this](
            const QString &message) {

            m_loading = false;

            emit loadingChanged();

            qDebug()
                << "ArtistController error:"
                << message;

            emit statusChanged(
                QString(
                    "Ошибка загрузки исполнителя: %1")
                    .arg(
                        message));
        });
}

void ArtistController::loadArtist(
    const QString &id)
{
    const QString artistId =
        id.trimmed();

    if (
        artistId.isEmpty()
    ) {
        emit statusChanged(
            "ID исполнителя не указан");

        return;
    }

    if (
        m_loading
    ) {
        return;
    }

    if (
        m_artistService == nullptr
    ) {
        emit statusChanged(
            "Сервис исполнителя недоступен");

        return;
    }

    m_loading = true;

    emit loadingChanged();

    m_artistId.clear();
    m_artistName.clear();
    m_artistCoverUri.clear();
    m_artistDescription.clear();
    m_artistGenres.clear();

    m_newRelease = {};

    m_artistModel
        ->clear();

    m_albumsModel
        ->clear();

    m_similarArtistsModel
        ->clear();

    emit artistChanged();

    qDebug()
        << "ArtistController loading:"
        << artistId;

    emit statusChanged(
        QString(
            "Загрузка исполнителя: %1")
            .arg(
                artistId));

    m_artistService
        ->loadArtist(
            artistId);
}

void ArtistController::selectTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }

    if (
        m_artistModel == nullptr
    ) {
        emit statusChanged(
            "ArtistModel недоступен");

        return;
    }

    const Track track =
        m_artistModel
            ->trackAt(
                index);

    if (
        track.id.isEmpty()
    ) {
        emit statusChanged(
            "Некорректный трек исполнителя");

        return;
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue == nullptr
    ) {
        emit statusChanged(
            "Очередь воспроизведения недоступна");

        return;
    }

    queue->clear();

    queue->addTracks(
        m_artistModel
            ->tracks());

    queue->setCurrentIndex(
        index);

    qDebug()
        << "Artist track selected:"
        << track.title
        << "| index:"
        << index;

    emit trackSelected(
        track);

    m_playbackController
        ->playTrack(
            track);
}

void ArtistController::selectSimilarArtist(
    int index)
{
    if (
        m_similarArtistsModel == nullptr
    ) {
        return;
    }

    const Artist artist =
        m_similarArtistsModel
            ->artistAt(
                index);

    if (
        artist.id.isEmpty()
    ) {
        emit statusChanged(
            "Некорректный похожий исполнитель");

        return;
    }

    qDebug()
        << "Similar artist selected:"
        << artist.name
        << "| id:"
        << artist.id;

    emit similarArtistSelected(
        artist.id);
}

void ArtistController::playArtist()
{
    if (
        m_playbackController == nullptr
    ) {
        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }

    if (
        m_artistModel == nullptr
    ) {
        emit statusChanged(
            "ArtistModel недоступен");

        return;
    }

    const QList<Track> tracks =
        m_artistModel
            ->tracks();

    if (
        tracks.isEmpty()
    ) {
        emit statusChanged(
            "У исполнителя нет доступных треков");

        return;
    }

    QueueService *queue =
        m_playbackController
            ->queueService();

    if (
        queue == nullptr
    ) {
        emit statusChanged(
            "Очередь воспроизведения недоступна");

        return;
    }

    queue->clear();

    queue->addTracks(
        tracks);

    queue->setCurrentIndex(
        0);

    const Track &track =
        tracks.first();

    qDebug()
        << "Artist playback started:"
        << m_artistName
        << "| tracks:"
        << tracks.size();

    emit trackSelected(
        track);

    m_playbackController
        ->playTrack(
            track);

    emit statusChanged(
        QString(
            "Воспроизведение исполнителя: %1")
        .arg(
            m_artistName));
}

ArtistModel *
ArtistController::artistModel() const
{
    return m_artistModel;
}

ArtistAlbumsModel *
ArtistController::albumsModel() const
{
    return m_albumsModel;
}

SimilarArtistsModel *
ArtistController::similarArtistsModel() const
{
    return m_similarArtistsModel;
}

bool ArtistController::isLoading() const
{
    return m_loading;
}

QString ArtistController::artistId() const
{
    return m_artistId;
}

QString ArtistController::artistName() const
{
    return m_artistName;
}

QString ArtistController::artistCoverUri() const
{
    return m_artistCoverUri;
}

QString ArtistController::artistDescription() const
{
    return m_artistDescription;
}

QString ArtistController::artistGenres() const
{
    return m_artistGenres;
}

QString ArtistController::newReleaseId() const
{
    return m_newRelease.id;
}

QString ArtistController::newReleaseTitle() const
{
    return m_newRelease.title;
}

QString ArtistController::newReleaseCoverUri() const
{
    return m_newRelease.coverUri;
}

int ArtistController::newReleaseYear() const
{
    return m_newRelease.year;
}