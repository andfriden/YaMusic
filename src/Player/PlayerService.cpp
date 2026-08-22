#include "PlayerService.h"

#include <QDebug>

PlayerService::PlayerService(
    QObject *parent)
    : QObject(parent)
    , m_player(this)
    , m_audioOutput(this)
{
    m_audioOutput.setVolume(
        1.0);

    m_player.setAudioOutput(
        &m_audioOutput);

    connect(
        &m_player,
        &QMediaPlayer::playbackStateChanged,
        this,
        [this](
            QMediaPlayer::PlaybackState state)
        {
            qDebug()
                << "Playback state:"
                << state;

            emit playingChanged();

            switch (state) {

            case QMediaPlayer::PlayingState:

                qDebug()
                    << "Playback started";

                emit playbackStarted();

                break;

            case QMediaPlayer::PausedState:

                qDebug()
                    << "Playback paused";

                emit playbackPaused();

                break;

            case QMediaPlayer::StoppedState:

                qDebug()
                    << "Playback stopped";

                emit playbackStopped();

                break;

            default:
                break;
            }
        });

    connect(
        &m_player,
        &QMediaPlayer::mediaStatusChanged,
        this,
        [this](
            QMediaPlayer::MediaStatus status)
        {
            qDebug()
                << "Media status:"
                << status;

            switch (status) {

            case QMediaPlayer::NoMedia:

                qDebug()
                    << "Media status: NoMedia";

                break;

            case QMediaPlayer::LoadingMedia:

                qDebug()
                    << "Media status: LoadingMedia";

                break;

            case QMediaPlayer::LoadedMedia:

                qDebug()
                    << "Media status: LoadedMedia";

                break;

            case QMediaPlayer::BufferingMedia:

                qDebug()
                    << "Media status: BufferingMedia";

                break;

            case QMediaPlayer::BufferedMedia:

                qDebug()
                    << "Media status: BufferedMedia";

                break;

            case QMediaPlayer::StalledMedia:

                qDebug()
                    << "Media status: StalledMedia";

                break;

            case QMediaPlayer::EndOfMedia:

                qDebug()
                    << "Media status: EndOfMedia";

                qDebug()
                    << "Playback finished";

                emit playbackFinished();

                break;

            case QMediaPlayer::InvalidMedia:

                qDebug()
                    << "Media status: InvalidMedia";

                break;

            default:

                qDebug()
                    << "Media status: Unknown";

                break;
            }
        });

    connect(
        &m_player,
        &QMediaPlayer::positionChanged,
        this,
        [this](qint64 position)
        {
            emit positionChanged(
                position);
        });

    connect(
        &m_player,
        &QMediaPlayer::durationChanged,
        this,
        [this](qint64 duration)
        {
            emit durationChanged(
                duration);
        });

    connect(
        &m_player,
        &QMediaPlayer::errorOccurred,
        this,
        [this](
            QMediaPlayer::Error error,
            const QString &errorString)
        {
            qDebug()
                << "QMediaPlayer error:"
                << error
                << errorString;

            emit errorOccurred(
                errorString);
        });
}

bool PlayerService::isPlaying() const
{
    return m_player.playbackState()
        == QMediaPlayer::PlayingState;
}

QString PlayerService::currentUrl() const
{
    return m_currentUrl;
}

qint64 PlayerService::position() const
{
    return m_player.position();
}

qint64 PlayerService::duration() const
{
    return m_player.duration();
}

void PlayerService::play()
{
    qDebug()
        << "PlayerService::play";

    m_player.play();
}

void PlayerService::playUrl(
    const QString &url)
{
    if (url.isEmpty()) {

        qDebug()
            << "PlayerService::playUrl:"
            << "empty URL";

        return;
    }

    qDebug()
        << "PlayerService::playUrl:"
        << url;

    if (m_currentUrl != url) {

        m_currentUrl = url;

        m_player.setSource(
            QUrl(url));

        emit currentUrlChanged();
    }

    m_player.play();
}

void PlayerService::pause()
{
    qDebug()
        << "PlayerService::pause";

    m_player.pause();
}

void PlayerService::resume()
{
    qDebug()
        << "PlayerService::resume";

    m_player.play();
}

void PlayerService::stop()
{
    qDebug()
        << "PlayerService::stop";

    m_player.stop();
}

void PlayerService::togglePlayback()
{
    qDebug()
        << "PlayerService::togglePlayback";

    if (isPlaying()) {
        pause();
    } else {
        resume();
    }
}

void PlayerService::seek(
    qint64 position)
{
    const qint64 playerDuration =
        m_player.duration();

    if (playerDuration <= 0) {
        return;
    }

    const qint64 clampedPosition =
        qBound(
            qint64(0),
            position,
            playerDuration);

    m_player.setPosition(
        clampedPosition);
}