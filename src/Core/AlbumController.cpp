#include "AlbumController.h"

#include "../Playback/PlaybackController.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Catalog/AlbumService.h"

#include <QDebug>

AlbumController::AlbumController(
    AlbumService *albumService,
    PlaybackController *playbackController,
    QObject *parent)
    : QObject(parent)
    , m_albumService(albumService)
    , m_playbackController(playbackController)
    , m_albumModel(new AlbumModel(this))
{
    if (
        m_albumService == nullptr
    ) {
        return;
    }

    connect(
        m_albumService,
        &AlbumService::albumReceived,
        this,
        [this](
            const AlbumDetails &album) {

            m_loading = false;

            m_albumId =
                album.album.id;

            m_albumModel
                ->setAlbum(
                    album);

            qDebug()
                << "AlbumController loaded:"
                << album.album.title
                << "| id:"
                << album.album.id
                << "| tracks:"
                << album.tracks.size();

            emit loadingChanged();

            emit albumChanged();

            emit statusChanged(
                QString(
                    "Альбом загружен: %1")
                    .arg(
                        album.album.title));
        });

    connect(
        m_albumService,
        &AlbumService::errorOccurred,
        this,
        [this](
            const QString &message) {

            m_loading = false;

            emit loadingChanged();

            qDebug()
                << "AlbumController error:"
                << message;

            emit statusChanged(
                QString(
                    "Ошибка загрузки альбома: %1")
                    .arg(
                        message));
        });
}

void AlbumController::loadAlbum(
    const QString &id)
{
    const QString albumId =
        id.trimmed();

    if (
        albumId.isEmpty()
    ) {
        emit statusChanged(
            "ID альбома не указан");

        return;
    }

    if (
        m_albumService == nullptr
    ) {
        emit statusChanged(
            "Сервис альбома недоступен");

        return;
    }

    if (
        m_loading
    ) {
        return;
    }

    m_loading = true;

    emit loadingChanged();

    m_albumId.clear();

    m_albumModel
        ->clear();

    emit albumChanged();

    qDebug()
        << "AlbumController loading:"
        << albumId;

    emit statusChanged(
        QString(
            "Загрузка альбома: %1")
            .arg(
                albumId));

    m_albumService
        ->loadAlbum(
            albumId);
}

void AlbumController::selectAlbumTrack(
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
        m_albumModel == nullptr
    ) {
        emit statusChanged(
            "AlbumModel недоступен");

        return;
    }

    const Track track =
        m_albumModel
            ->trackAt(
                index);

    if (
        track.id.isEmpty()
    ) {
        emit statusChanged(
            "Некорректный трек альбома");

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

    const QList<Track> tracks =
        m_albumModel
            ->tracks();

    if (
        tracks.isEmpty()
    ) {
        emit statusChanged(
            "В альбоме нет треков");

        return;
    }

    if (
        index < 0 ||
        index >= tracks.size()
    ) {
        emit statusChanged(
            "Некорректный индекс трека");

        return;
    }

    queue->clear();

    queue->addTracks(
        tracks);

    queue->setCurrentIndex(
        index);

    qDebug()
        << "Album track selected:"
        << track.title
        << "| album:"
        << albumTitle()
        << "| index:"
        << index;

    emit trackSelected(
        track);

    m_playbackController
        ->playTrack(
            track);
}

void AlbumController::playAlbum()
{
    if (
        m_playbackController == nullptr
    ) {
        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }

    if (
        m_albumModel == nullptr
    ) {
        emit statusChanged(
            "AlbumModel недоступен");

        return;
    }

    const QList<Track> tracks =
        m_albumModel
            ->tracks();

    if (
        tracks.isEmpty()
    ) {
        emit statusChanged(
            "В альбоме нет доступных треков");

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
        << "Album playback started:"
        << albumTitle()
        << "| tracks:"
        << tracks.size();

    emit trackSelected(
        track);

    m_playbackController
        ->playTrack(
            track);

    emit statusChanged(
        QString(
            "Воспроизведение альбома: %1")
        .arg(
            albumTitle()));
}

AlbumModel *
AlbumController::albumModel() const
{
    return m_albumModel;
}

bool AlbumController::isLoading() const
{
    return m_loading;
}

QString AlbumController::albumId() const
{
    return m_albumId;
}

QString AlbumController::albumTitle() const
{
    if (
        m_albumModel == nullptr
    ) {
        return {};
    }

    return m_albumModel
        ->title();
}

QString AlbumController::albumCoverUri() const
{
    if (
        m_albumModel == nullptr
    ) {
        return {};
    }

    return m_albumModel
        ->coverUri();
}

int AlbumController::albumTrackCount() const
{
    if (
        m_albumModel == nullptr
    ) {
        return 0;
    }

    return m_albumModel
        ->trackCount();
}