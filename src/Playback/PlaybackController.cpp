#include "PlaybackController.h"

#include "../MediaControls/MediaControlsFactory.h"
#include "../Player/PlayerService.h"
#include "../Yandex/Catalog/TrackService.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>


static QString coverCacheDir()
{
    return QStandardPaths::writableLocation(
        QStandardPaths::CacheLocation)
        + QStringLiteral("/covers");
}


static QString coverFilePath(
    const QString &trackId)
{
    return coverCacheDir()
        + QStringLiteral("/")
        + trackId
        + QStringLiteral(".jpg");
}


PlaybackController::PlaybackController(
    TrackService *trackService,
    PlayerService *playerService,
    QueueService *queueService,
    QObject *parent)
    : QObject(parent)
    , m_trackService(trackService)
    , m_playerService(playerService)
    , m_queueService(queueService)
    , m_coverNetwork(new QNetworkAccessManager(this))
    , m_streamNetwork(new QNetworkAccessManager(this))
    , m_streamCacheDir(QStandardPaths::writableLocation(
          QStandardPaths::CacheLocation)
          + QStringLiteral("/stream_cache"))
{
    setupSystemMediaControls();

    if (m_playerService != nullptr) {

        connect(
            m_playerService,
            &PlayerService::playbackStarted,
            this,
            [this]()
            {
                /*
                 * playbackStarted means that the new media
                 * source has actually started playing.
                 *
                 * This is the correct moment to restore the
                 * position saved before a broken network stream.
                 */

                if (
                    m_recoveringPlayback
                    && m_recoveryPositionPending
                    && m_currentTrack.id ==
                        m_recoveryTrackId
                ) {

                    const qint64 recoveryPosition =
                        m_recoveryPosition;

                    m_recoveringPlayback =
                        false;

                    m_recoveryPositionPending =
                        false;

                    m_recoveryPosition =
                        0;

                    m_recoveryTrackId.clear();

                    if (recoveryPosition > 0) {

                        m_playerService
                            ->seek(
                                recoveryPosition);
                    }

                } else {

                    m_recoveringPlayback =
                        false;

                    m_recoveryPositionPending =
                        false;

                    m_recoveryPosition =
                        0;

                    m_recoveryTrackId.clear();
                }

                setState(
                    Playing);
            });

        connect(
            m_playerService,
            &PlayerService::playbackPaused,
            this,
            [this]()
            {
                setState(
                    Paused);
            });

        connect(
            m_playerService,
            &PlayerService::playbackStopped,
            this,
            [this]()
            {
                /*
                 * Do not change the controller state while
                 * recovering a broken network stream.
                 *
                 * QMediaPlayer can temporarily enter
                 * StoppedState while the old network source
                 * is being replaced.
                 */

                if (!m_recoveringPlayback) {

                    setState(
                        Stopped);
                }
            });

        connect(
            m_playerService,
            &PlayerService::playbackFinished,
            this,
            &PlaybackController::handlePlaybackFinished);

        connect(
            m_playerService,
            &PlayerService::errorOccurred,
            this,
            [this](
                const QString &message)
            {
                /*
                 * There is nothing to recover without the
                 * required services or a current track.
                 */

                if (
                    m_trackService == nullptr
                    || m_playerService == nullptr
                    || m_currentTrack.id.isEmpty()
                ) {

                    setState(
                        Error);

                    emit playbackError(
                        message);

                    return;
                }

                /*
                 * FFmpeg may report the same network error
                 * several times while the old socket is being
                 * closed.
                 *
                 * Only the first error starts recovery.
                 */

                if (m_recoveringPlayback) {
                    return;
                }

                /*
                 * Remember the exact playback position before
                 * replacing the expired/broken stream URL.
                 */

                m_recoveryPosition =
                    m_playerService->position();

                m_recoveryTrackId =
                    m_currentTrack.id;

                m_recoveryPositionPending =
                    true;

                m_recoveringPlayback =
                    true;

                setState(
                    Loading);

                /*
                 * Request a fresh temporary Yandex stream URL.
                 */

                m_trackService
                    ->loadStreamInfo(
                        m_currentTrack.id);
            });
    }

    if (m_trackService != nullptr) {

        connect(
            m_trackService,
            &TrackService::streamUrlReceived,
            this,
            [this](
                const QString &trackId,
                const QString &url)
            {
                /*
                 * Ignore responses for tracks which are no
                 * longer current.
                 */

                if (
                    trackId.isEmpty()
                    || trackId != m_currentTrack.id
                ) {
                    return;
                }

                if (url.trimmed().isEmpty()) {

                    const bool recovering =
                        m_recoveringPlayback;

                    m_recoveringPlayback =
                        false;

                    m_recoveryPositionPending =
                        false;

                    m_recoveryPosition =
                        0;

                    m_recoveryTrackId.clear();

                    setState(
                        Error);

                    emit playbackError(
                        recovering
                            ? QStringLiteral(
                                "Не удалось обновить Stream URL")
                            : QStringLiteral(
                                "Stream URL пуст"));

                    return;
                }

                if (m_playerService == nullptr) {

                    m_recoveringPlayback =
                        false;

                    m_recoveryPositionPending =
                        false;

                    m_recoveryPosition =
                        0;

                    m_recoveryTrackId.clear();

                    setState(
                        Error);

                    emit playbackError(
                        QStringLiteral(
                            "PlayerService недоступен"));

                    return;
                }

                /*
                 * Normal playback.
                 */

                if (!m_recoveringPlayback) {

                    handleStreamUrl(
                        trackId,
                        url);

                    return;
                }

                /*
                 * Recovery playback.
                 *
                 * Do NOT seek here.
                 *
                 * QMediaPlayer has only received the new source
                 * at this point. The source may still be loading,
                 * so the duration/position may not be available.
                 *
                 * We keep the recovery state active until
                 * playbackStarted.
                 */

                downloadAndPlayStream(
                    trackId,
                    url);
            });

        connect(
            m_trackService,
            &TrackService::errorOccurred,
            this,
            [this](
                const QString &message)
            {
                const bool recovering =
                    m_recoveringPlayback;

                m_recoveringPlayback =
                    false;

                m_recoveryPositionPending =
                    false;

                m_recoveryPosition =
                    0;

                m_recoveryTrackId.clear();

                setState(
                    Error);

                emit playbackError(
                    recovering
                        ? QStringLiteral(
                            "Не удалось восстановить воспроизведение: ")
                            + message
                        : message);
            });
    }

    if (m_queueService != nullptr) {

        connect(
            m_queueService,
            &QueueService::repeatModeChanged,
            this,
            [this]()
            {
                emit repeatModeChanged();
            });

        connect(
            m_queueService,
            &QueueService::shuffleChanged,
            this,
            [this]()
            {
                emit shuffleChanged();
            });
    }
}


Track PlaybackController::currentTrack() const
{
    return m_currentTrack;
}


PlaybackController::PlaybackState
PlaybackController::state() const
{
    return m_state;
}


QueueService *
PlaybackController::queueService() const
{
    return m_queueService;
}


SystemMediaControls *
PlaybackController::systemMediaControls() const
{
    return m_systemMediaControls.get();
}


/*
 * Playback
 */

void PlaybackController::playTrack(
    const Track &track)
{
    if (track.id.isEmpty()) {

        setState(
            Error);

        emit playbackError(
            "Некорректный трек");

        return;
    }

    /*
     * A new explicit track selection always cancels
     * any previous recovery operation.
     */

    m_recoveringPlayback =
        false;

    m_recoveryPositionPending =
        false;

    m_recoveryPosition =
        0;

    m_recoveryTrackId.clear();

    /*
     * Cancel any in-progress stream download for the
     * previous track.
     */

    cancelStreamDownload();

    if (m_queueService != nullptr) {

        const QList<Track> queueTracks =
            m_queueService->tracks();

        for (
            int i = 0;
            i < queueTracks.size();
            ++i
        ) {

            if (
                queueTracks.at(i).id ==
                track.id
            ) {

                m_queueService
                    ->setCurrentIndex(
                        i);

                break;
            }
        }
    }

    m_currentTrack =
        track;

    emit currentTrackChanged();

    setState(
        Loading);

    if (m_trackService == nullptr) {

        setState(
            Error);

        emit playbackError(
            "TrackService недоступен");

        return;
    }

    m_trackService
        ->loadStreamInfo(
            track.id);
}


void PlaybackController::playFromSource(
    const QList<Track> &tracks,
    int index,
    const QString &sourceTitle,
    const QString &sourceType)
{
    if (m_queueService == nullptr) {

        setState(
            Error);

        emit playbackError(
            "QueueService недоступен");

        return;
    }

    if (
        index < 0 ||
        index >= tracks.size()
    ) {

        setState(
            Error);

        emit playbackError(
            "Некорректный индекс трека");

        return;
    }

    const Track track =
        tracks.at(index);

    if (track.id.isEmpty()) {

        setState(
            Error);

        emit playbackError(
            "Некорректный трек");

        return;
    }

    m_queueService->clear();

    m_queueService->addTracks(
        tracks);

    m_queueService->setCurrentIndex(
        index);

    if (!sourceTitle.isEmpty()) {

        m_queueService->setSource(
            sourceTitle,
            sourceType);
    }

    playTrack(
        track);
}


void PlaybackController::playQueue()
{
    if (m_queueService == nullptr) {

        setState(
            Error);

        emit playbackError(
            "QueueService недоступен");

        return;
    }

    const Track track =
        m_queueService
            ->currentTrack();

    if (track.id.isEmpty()) {

        setState(
            Error);

        emit playbackError(
            "Очередь пуста");

        return;
    }

    playTrack(
        track);
}


void PlaybackController::playCurrent()
{
    if (m_currentTrack.id.isEmpty()) {

        playQueue();

        return;
    }

    playTrack(
        m_currentTrack);
}


void PlaybackController::pause()
{
    if (m_playerService == nullptr) {
        return;
    }

    m_playerService->pause();
}


void PlaybackController::resume()
{
    if (m_playerService == nullptr) {
        return;
    }

    m_playerService->play();
}


void PlaybackController::stop()
{
    /*
     * Explicit stop cancels any pending recovery.
     */

    m_recoveringPlayback =
        false;

    m_recoveryPositionPending =
        false;

    m_recoveryPosition =
        0;

    m_recoveryTrackId.clear();

    cancelStreamDownload();

    if (m_playerService == nullptr) {
        return;
    }

    m_playerService->stop();
}


bool PlaybackController::next()
{
    if (m_queueService == nullptr) {
        return false;
    }

    if (m_queueService->hasNext()) {

        m_queueService->next();

        return playQueueCurrentTrack();
    }

    if (
        m_queueService->repeatMode() ==
            QueueService::RepeatAll
        && m_queueService->count() > 0
    ) {

        m_queueService
            ->setCurrentIndex(
                0);

        return playQueueCurrentTrack();
    }

    return false;
}


bool PlaybackController::previous()
{
    if (m_queueService == nullptr) {
        return false;
    }

    if (m_queueService->hasPrevious()) {

        m_queueService->previous();

        return playQueueCurrentTrack();
    }

    if (
        m_queueService->repeatMode() ==
            QueueService::RepeatAll
        && m_queueService->count() > 0
    ) {

        m_queueService
            ->setCurrentIndex(
                m_queueService->count() - 1);

        return playQueueCurrentTrack();
    }

    return false;
}


void PlaybackController::setRepeatMode(
    QueueService::RepeatMode mode)
{
    if (m_queueService == nullptr) {
        return;
    }

    m_queueService
        ->setRepeatMode(
            mode);
}


QueueService::RepeatMode
PlaybackController::repeatMode() const
{
    if (m_queueService == nullptr) {
        return QueueService::RepeatOff;
    }

    return m_queueService
        ->repeatMode();
}


void PlaybackController::cycleRepeatMode()
{
    if (m_queueService == nullptr) {
        return;
    }

    m_queueService
        ->cycleRepeatMode();
}


bool PlaybackController::shuffleEnabled() const
{
    if (m_queueService == nullptr) {
        return false;
    }

    return m_queueService
        ->shuffleEnabled();
}


void PlaybackController::setShuffleEnabled(
    bool enabled)
{
    if (m_queueService == nullptr) {
        return;
    }

    m_queueService
        ->setShuffleEnabled(
            enabled);
}


void PlaybackController::toggleShuffle()
{
    if (m_queueService == nullptr) {
        return;
    }

    m_queueService
        ->toggleShuffle();
}


/*
 * State
 */

void PlaybackController::setState(
    PlaybackState state)
{
    if (m_state == state) {
        return;
    }

    m_state =
        state;

    emit stateChanged();
}


/*
 * Stream
 */

void PlaybackController::handleStreamUrl(
    const QString &trackId,
    const QString &url)
{
    if (
        trackId.isEmpty()
        || trackId != m_currentTrack.id
    ) {
        return;
    }

    if (url.trimmed().isEmpty()) {

        setState(
            Error);

        emit playbackError(
            "Stream URL пуст");

        return;
    }

    if (m_playerService == nullptr) {

        setState(
            Error);

        emit playbackError(
            "PlayerService недоступен");

        return;
    }

    downloadAndPlayStream(
        trackId,
        url);
}

/*
 * Stream download proxy
 *
 * On macOS, Qt Multimedia uses FFmpeg as the media backend.
 * FFmpeg on macOS relies on Apple SecureTransport for TLS,
 * which has known issues with streaming HTTPS audio and
 * frequently fails with errSSLClosedGraceful (-9806).
 *
 * Qt's own QNetworkAccessManager uses a different SSL stack
 * (its bundled TLS or SecureTransport via the right APIs).
 *
 * Workaround: download the audio through QNetworkAccessManager
 * into a local cache file, then play the local file.
 */

static QString streamCachePath(
    const QString &trackId)
{
    return QStandardPaths::writableLocation(
               QStandardPaths::CacheLocation)
        + QStringLiteral("/stream_cache/")
        + trackId
        + QStringLiteral(".mp3");
}

void PlaybackController::downloadAndPlayStream(
    const QString &trackId,
    const QString &streamUrl)
{
    if (trackId.isEmpty() || streamUrl.isEmpty())
    {
        setState(Error);
        emit playbackError("Stream download: empty args");
        return;
    }

    /*
     * Clean up any previous download for this track.
     */
    if (m_streamDownloadReply)
    {
        m_streamDownloadReply->disconnect();
        m_streamDownloadReply->abort();
        m_streamDownloadReply->deleteLater();
        m_streamDownloadReply = nullptr;
    }

    const QString dest = streamCachePath(trackId);
    QDir().mkpath(QFileInfo(dest).absolutePath());

    /*
     * Delete old cached file.
     */
    QFile::remove(dest);

    QFile *cacheFile = new QFile(dest, this);
    if (!cacheFile->open(QIODevice::WriteOnly))
    {
        delete cacheFile;
        setState(Error);
        emit playbackError("Cannot open stream cache file");
        return;
    }

    m_pendingStreamTrackId = trackId;

    QNetworkRequest request{QUrl(streamUrl)};
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QStringLiteral("YaMusic/1.0 (Qt)"));
    request.setAttribute(
        QNetworkRequest::RedirectPolicyAttribute,
        QNetworkRequest::NoLessSafeRedirectPolicy);

    m_streamDownloadReply = m_streamNetwork->get(request);

    connect(m_streamDownloadReply, &QNetworkReply::readyRead,
            this, [this, cacheFile]()
    {
        cacheFile->write(m_streamDownloadReply->readAll());
    });

    connect(m_streamDownloadReply, &QNetworkReply::finished,
            this, [this, cacheFile, trackId, dest]()
    {
        QPointer<QNetworkReply> reply = m_streamDownloadReply;
        m_streamDownloadReply = nullptr;

        cacheFile->close();

        if (reply && reply->error() != QNetworkReply::NoError)
        {
            cacheFile->remove();
            emit playbackError("Stream download failed: "
                               + reply->errorString());
            return;
        }

        if (trackId != m_currentTrack.id)
        {
            cacheFile->remove();
            return;
        }

        if (m_playerService)
        {
            m_playerService->playUrl(
                QUrl::fromLocalFile(dest).toString());
        }
    });

    connect(m_streamDownloadReply, &QNetworkReply::downloadProgress,
            this, [this, trackId](qint64 received, qint64 total)
    {
        if (received > 0 && total > 0)
        {
            const int pct = static_cast<int>(received * 100 / total);
            Q_UNUSED(pct)
        }
    });
}


void PlaybackController::cancelStreamDownload()
{
    if (m_streamDownloadReply)
    {
        m_streamDownloadReply->disconnect();
        m_streamDownloadReply->abort();
        m_streamDownloadReply->deleteLater();
        m_streamDownloadReply = nullptr;
    }

    if (!m_pendingStreamTrackId.isEmpty())
    {
        QFile::remove(streamCachePath(m_pendingStreamTrackId));
        m_pendingStreamTrackId.clear();
    }
}


bool PlaybackController::playQueueCurrentTrack()
{
    if (m_queueService == nullptr) {
        return false;
    }

    const Track track =
        m_queueService
            ->currentTrack();

    if (track.id.isEmpty()) {
        return false;
    }

    playTrack(
        track);

    return true;
}


/*
 * End of track
 */

void PlaybackController::handlePlaybackFinished()
{
    if (m_queueService == nullptr) {

        setState(
            Stopped);

        return;
    }

    const QueueService::RepeatMode mode =
        m_queueService->repeatMode();

    /*
     * RepeatOne: переигрываем текущий трек.
     */

    if (mode == QueueService::RepeatOne) {

        if (playQueueCurrentTrack()) {
            return;
        }

        setState(
            Stopped);

        return;
    }

    /*
     * Есть следующий трек в очереди — играем его.
     */

    if (m_queueService->hasNext()) {

        m_queueService->next();

        if (playQueueCurrentTrack()) {
            return;
        }
    }

    /*
     * RepeatAll: зацикливаемся на начало.
     */

    if (
        mode == QueueService::RepeatAll
        && m_queueService->count() > 0
    ) {

        m_queueService
            ->setCurrentIndex(
                0);

        if (playQueueCurrentTrack()) {
            return;
        }
    }

    /*
     * Очередь кончилась.
     */

    const QString sourceType =
        m_queueService->sourceType();

    const QString sourceTitle =
        m_queueService->sourceTitle();

    setState(
        Stopped);

    emit playlistExhausted(
        sourceType,
        sourceTitle);
}


/*
 * System media metadata
 */

static QString
normalizeCoverUri(
    const QString &uri)
{
    if (uri.isEmpty()) {
        return {};
    }

    QString result =
        uri;

    result.replace(
        QStringLiteral("%%"),
        QStringLiteral("1000x1000"));

    if (
        !result.contains(
            QStringLiteral("://"))
    ) {
        result.prepend(
            QStringLiteral("https://"));
    }

    return result;
}


static SystemMediaControls::Metadata
makeMediaMetadata(
    const Track &track)
{
    SystemMediaControls::Metadata md;

    md.title =
        track.title;

    md.artist =
        track.artists.isEmpty()
            ? QString()
            : track.artists.first().name;

    md.album =
        track.albums.isEmpty()
            ? QString()
            : track.albums.first().title;

    md.durationMs =
        track.durationMs;

    md.trackId =
        track.id;

    /*
     * Use local cached cover when available.
     * file:// is required by MPRIS clients.
     */

    const QString cached =
        coverFilePath(
            track.id);

    if (QFile::exists(cached)) {

        md.coverUrl =
            QUrl::fromLocalFile(cached).toString();

    } else {

        md.coverUrl =
            normalizeCoverUri(
                track.coverUri);
    }

    return md;
}


static QString
mprisLoopStatus(
    QueueService::RepeatMode mode)
{
    switch (mode)
    {
    case QueueService::RepeatOff:
        return QStringLiteral("None");

    case QueueService::RepeatOne:
        return QStringLiteral("Track");

    case QueueService::RepeatAll:
        return QStringLiteral("Playlist");
    }

    return QStringLiteral("None");
}


void PlaybackController::setupSystemMediaControls()
{
    m_systemMediaControls =
        MediaControlsFactory::create(
            this);

    if (
        m_systemMediaControls == nullptr
    ) {
        return;
    }

    /*
     * Управление из системы → PlaybackController.
     */

    connect(
        m_systemMediaControls.get(),
        &SystemMediaControls::playRequested,
        this,
        &PlaybackController::resume);

    connect(
        m_systemMediaControls.get(),
        &SystemMediaControls::pauseRequested,
        this,
        &PlaybackController::pause);

    connect(
        m_systemMediaControls.get(),
        &SystemMediaControls::togglePlayPauseRequested,
        this,
        [this]()
        {
            if (state() == Playing) {

                pause();

            } else {

                resume();
            }
        });

    connect(
        m_systemMediaControls.get(),
        &SystemMediaControls::nextRequested,
        this,
        &PlaybackController::next);

    connect(
        m_systemMediaControls.get(),
        &SystemMediaControls::previousRequested,
        this,
        &PlaybackController::previous);

    connect(
        m_systemMediaControls.get(),
        &SystemMediaControls::seekRequested,
        this,
        [this](
            qint64 positionMs)
        {
            if (m_playerService) {

                m_playerService
                    ->seek(
                        positionMs);
            }
        });

    /*
     * Состояние воспроизведения → система.
     */

    connect(
        this,
        &PlaybackController::currentTrackChanged,
        this,
        [this]()
        {
            if (
                !m_systemMediaControls
                    ->isEnabled()
            ) {

                m_systemMediaControls
                    ->setEnabled(
                        true);
            }

            const auto md =
                makeMediaMetadata(
                    m_currentTrack);

            m_systemMediaControls
                ->setMetadata(
                    md);

            m_systemMediaControls
                ->setDuration(
                    md.durationMs);

            fetchCurrentCover();
        });

    connect(
        this,
        &PlaybackController::stateChanged,
        this,
        [this]()
        {
            switch (m_state)
            {
            case Playing:

                m_systemMediaControls
                    ->setPlaybackStatus(
                        SystemMediaControls
                            ::PlaybackStatus
                            ::Playing);

                break;

            case Paused:

                m_systemMediaControls
                    ->setPlaybackStatus(
                        SystemMediaControls
                            ::PlaybackStatus
                            ::Paused);

                break;

            default:

                m_systemMediaControls
                    ->setPlaybackStatus(
                        SystemMediaControls
                            ::PlaybackStatus
                            ::Stopped);

                break;
            }
        });

    connect(
        this,
        &PlaybackController::repeatModeChanged,
        this,
        [this]()
        {
            if (m_queueService) {

                m_systemMediaControls
                    ->setLoopStatus(
                        mprisLoopStatus(
                            m_queueService
                                ->repeatMode()));
            }
        });

    connect(
        this,
        &PlaybackController::shuffleChanged,
        this,
        [this]()
        {
            if (m_queueService) {

                m_systemMediaControls
                    ->setShuffle(
                        m_queueService
                            ->shuffleEnabled());
            }
        });

    /*
     * Позиция — от PlayerService к системе.
     * Обновляется примерно 4 раза в секунду из AppController.
     */

    m_systemMediaControls
        ->setEnabled(
            true);
}


/*
 * Cover cache
 */

void PlaybackController::fetchCurrentCover()
{
    if (
        m_currentTrack.id.isEmpty()
        || m_currentTrack.coverUri.isEmpty()
    ) {
        return;
    }

    const QString cached =
        coverFilePath(
            m_currentTrack.id);

    if (QFile::exists(cached)) {
        return;
    }

    const QString url =
        normalizeCoverUri(
            m_currentTrack.coverUri);

    m_pendingCoverUri =
        m_currentTrack.coverUri;

    QNetworkRequest request{
        QUrl(url)};

    request.setHeader(
        QNetworkRequest::UserAgentHeader,
        QStringLiteral(
            "YaMusic/1.0 (Qt)"));

    request.setAttribute(
        QNetworkRequest::RedirectPolicyAttribute,
        QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply *reply =
        m_coverNetwork->get(
            request);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply]()
        {
            reply->deleteLater();

            if (
                reply->error()
                != QNetworkReply::NoError
            ) {
                return;
            }

            const QByteArray data =
                reply->readAll();

            if (data.isEmpty()) {
                return;
            }

            QDir dir;

            if (
                !dir.mkpath(
                    coverCacheDir())
            ) {
                return;
            }

            QFile file(
                coverFilePath(
                    m_currentTrack.id));

            if (
                !file.open(
                    QIODevice::WriteOnly)
            ) {
                return;
            }

            file.write(data);
            file.close();

            /*
             * Re-publish metadata so clients pick up
             * the file:// cover.
             */

            const auto md =
                makeMediaMetadata(
                    m_currentTrack);

            m_systemMediaControls
                ->setMetadata(
                    md);
        });
}