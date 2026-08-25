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
    qDebug()
        << "==================================================";

    qDebug()
        << "AlbumController CREATED"
        << "| service:"
        << m_albumService
        << "| playback:"
        << m_playbackController
        << "| model:"
        << m_albumModel;

    qDebug()
        << "==================================================";

    if (
        m_albumService == nullptr
    ) {
        qDebug()
            << "AlbumController:"
            << "AlbumService is null";

        return;
    }

    connect(
        m_albumService,
        &AlbumService::albumReceived,
        this,
        [this](
            const AlbumDetails &album) {

            qDebug()
                << "##################################################";

            qDebug()
                << "AlbumController::albumReceived";

            qDebug()
                << "SERVICE:"
                << "id ="
                << album.album.id
                << "| title ="
                << album.album.title
                << "| tracks ="
                << album.tracks.size();

            m_loading =
                false;

            m_albumId =
                album.album.id;

            qDebug()
                << "MODEL BEFORE setAlbum:"
                << "count ="
                << m_albumModel->count()
                << "| tracks ="
                << m_albumModel->tracks().size();

            m_albumModel->setAlbum(
                album);

            qDebug()
                << "MODEL AFTER setAlbum:"
                << "count ="
                << m_albumModel->count()
                << "| tracks ="
                << m_albumModel->tracks().size();

            const QList<Track> modelTracks =
                m_albumModel->tracks();

            for (
                int i = 0;
                i < modelTracks.size();
                ++i
            ) {
                qDebug()
                    << "AlbumController MODEL TRACK"
                    << i
                    << "| id:"
                    << modelTracks.at(i).id
                    << "| title:"
                    << modelTracks.at(i).title;
            }

            emit loadingChanged();

            emit albumChanged();

            emit statusChanged(
                QString(
                    "Альбом загружен: %1")
                    .arg(
                        album.album.title));

            qDebug()
                << "AlbumController SIGNALS EMITTED"
                << "| loading:"
                << m_loading
                << "| model count:"
                << m_albumModel->count();

            qDebug()
                << "##################################################";
        });

    connect(
        m_albumService,
        &AlbumService::errorOccurred,
        this,
        [this](
            const QString &message) {

            m_loading =
                false;

            qDebug()
                << "AlbumController ERROR:"
                << message;

            emit loadingChanged();

            emit statusChanged(
                QString(
                    "Ошибка загрузки альбома: %1")
                    .arg(message));
        });
}

void AlbumController::loadAlbum(
    const QString &id)
{
    const QString albumId =
        id.trimmed();

    qDebug()
        << "==================================================";

    qDebug()
        << "AlbumController::loadAlbum"
        << "| requested id:"
        << albumId;

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
        qDebug()
            << "AlbumController:"
            << "already loading";

        return;
    }

    m_loading =
        true;

    emit loadingChanged();

    m_albumId.clear();

    qDebug()
        << "CLEARING MODEL BEFORE LOAD"
        << "| count:"
        << m_albumModel->count();

    m_albumModel->clear();

    qDebug()
        << "MODEL CLEARED"
        << "| count:"
        << m_albumModel->count();

    emit albumChanged();

    emit statusChanged(
        QString(
            "Загрузка альбома: %1")
            .arg(albumId));

    qDebug()
        << "CALL AlbumService::loadAlbum:"
        << albumId;

    m_albumService->loadAlbum(
        albumId);

    qDebug()
        << "==================================================";
}

void AlbumController::selectAlbumTrack(
    int index)
{
    qDebug()
        << "AlbumController::selectAlbumTrack"
        << "| index:"
        << index
        << "| model count:"
        << m_albumModel->count();

    if (
        m_playbackController == nullptr
    ) {
        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }

    const Track track =
        m_albumModel->trackAt(index);

    if (
        track.id.isEmpty()
    ) {
        emit statusChanged(
            "Некорректный трек альбома");

        return;
    }

    QueueService *queue =
        m_playbackController->queueService();

    if (
        queue == nullptr
    ) {
        emit statusChanged(
            "Очередь воспроизведения недоступна");

        return;
    }

    const QList<Track> tracks =
        m_albumModel->tracks();

    if (tracks.isEmpty()) {
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
        << "| index:"
        << index
        << "| queue tracks:"
        << tracks.size();

    emit trackSelected(
        track);

    m_playbackController->playTrack(
        track);
}

void AlbumController::playAlbum()
{
    qDebug()
        << "AlbumController::playAlbum"
        << "| model count:"
        << m_albumModel->count();

    if (
        m_playbackController == nullptr
    ) {
        emit statusChanged(
            "PlaybackController недоступен");

        return;
    }

    const QList<Track> tracks =
        m_albumModel->tracks();

    if (tracks.isEmpty()) {
        emit statusChanged(
            "В альбоме нет доступных треков");

        return;
    }

    QueueService *queue =
        m_playbackController->queueService();

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

    queue->setCurrentIndex(0);

    const Track &track =
        tracks.first();

    qDebug()
        << "Album playback started:"
        << albumTitle()
        << "| tracks:"
        << tracks.size();

    emit trackSelected(
        track);

    m_playbackController->playTrack(
        track);

    emit statusChanged(
        QString(
            "Воспроизведение альбома: %1")
            .arg(albumTitle()));
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

    return m_albumModel->title();
}

QString AlbumController::albumCoverUri() const
{
    if (
        m_albumModel == nullptr
    ) {
        return {};
    }

    return m_albumModel->coverUri();
}

int AlbumController::albumTrackCount() const
{
    if (
        m_albumModel == nullptr
    ) {
        return 0;
    }

    return m_albumModel->count();
}