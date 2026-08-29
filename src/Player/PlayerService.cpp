#include "PlayerService.h"

#include <QDebug>


PlayerService::PlayerService(
    QObject *parent)
    : QObject(parent)
    , m_player(this)
    , m_audioOutput(this)
{
    // =========================================================
    // Audio output
    // =========================================================

    m_audioOutput.setVolume(
        1.0f);

    m_audioOutput.setMuted(
        false);

    m_player.setAudioOutput(
        &m_audioOutput);


    // =========================================================
    // Playback state
    // =========================================================

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


    // =========================================================
    // Media status
    // =========================================================

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


    // =========================================================
    // Position
    // =========================================================

    connect(
        &m_player,
        &QMediaPlayer::positionChanged,
        this,
        [this](
            qint64 position)
        {
            emit positionChanged(
                position);
        });


    // =========================================================
    // Duration
    // =========================================================

    connect(
        &m_player,
        &QMediaPlayer::durationChanged,
        this,
        [this](
            qint64 duration)
        {
            emit durationChanged(
                duration);
        });


    // =========================================================
    // Player errors
    // =========================================================

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


    // =========================================================
    // Volume
    // =========================================================

    connect(
        &m_audioOutput,
        &QAudioOutput::volumeChanged,
        this,
        [this]()
        {
            qDebug()
                << "QAudioOutput volume changed:"
                << m_audioOutput.volume();

            emit volumeChanged();
        });


    // =========================================================
    // Mute
    // =========================================================

    connect(
        &m_audioOutput,
        &QAudioOutput::mutedChanged,
        this,
        [this]()
        {
            emit mutedChanged();
        });
}


// =============================================================
// Playback state
// =============================================================

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


// =============================================================
// Volume
// =============================================================

float PlayerService::volume() const
{
    return m_audioOutput.volume();
}


bool PlayerService::isMuted() const
{
    return m_audioOutput.isMuted();
}


// =============================================================
// Playback
// =============================================================

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

        m_currentUrl =
            url;

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


// =============================================================
// Volume
// =============================================================

void PlayerService::setVolume(
    float volume)
{
    const float clampedVolume =
        qBound(
            0.0f,
            volume,
            1.0f);

    qDebug()
        << "PlayerService::setVolume:"
        << clampedVolume;

    m_audioOutput.setVolume(
        clampedVolume);
}


void PlayerService::setMuted(
    bool muted)
{
    if (
        m_audioOutput.isMuted()
        == muted
    ) {
        return;
    }

    qDebug()
        << "PlayerService::setMuted:"
        << muted;

    m_audioOutput.setMuted(
        muted);
}


void PlayerService::toggleMute()
{
    const bool muted =
        m_audioOutput.isMuted();

    qDebug()
        << "PlayerService::toggleMute:"
        << !muted;

    m_audioOutput.setMuted(
        !muted);
}