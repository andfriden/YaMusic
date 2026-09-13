#include "PlayerService.h"

PlayerService::PlayerService(
    QObject *parent)
    : QObject(parent)
    , m_player(this)
    , m_audioOutput(this)
{
    // Audio output

    m_audioOutput.setVolume(
        1.0f);

    m_audioOutput.setMuted(
        false);

    m_player.setAudioOutput(
        &m_audioOutput);

    // Playback state

    connect(
        &m_player,
        &QMediaPlayer::playbackStateChanged,
        this,
        [this](
            QMediaPlayer::PlaybackState state)
        {

            emit playingChanged();

            switch (state) {

            case QMediaPlayer::PlayingState:

                emit playbackStarted();

                break;

            case QMediaPlayer::PausedState:

                emit playbackPaused();

                break;

            case QMediaPlayer::StoppedState:

                emit playbackStopped();

                break;

            default:

                break;
            }
        });

    // Media status

    connect(
        &m_player,
        &QMediaPlayer::mediaStatusChanged,
        this,
        [this](
            QMediaPlayer::MediaStatus status)
        {

            switch (status) {

            case QMediaPlayer::NoMedia:

                break;

            case QMediaPlayer::LoadingMedia:

                break;

            case QMediaPlayer::LoadedMedia:

                break;

            case QMediaPlayer::BufferingMedia:

                break;

            case QMediaPlayer::BufferedMedia:

                break;

            case QMediaPlayer::StalledMedia:

                break;

            case QMediaPlayer::EndOfMedia:

                emit playbackFinished();

                break;

            case QMediaPlayer::InvalidMedia:

                break;

            default:

                break;
            }
        });

    // Position

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

    // Duration

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

    // Player errors

    connect(
        &m_player,
        &QMediaPlayer::errorOccurred,
        this,
        [this](
            QMediaPlayer::Error error,
            const QString &errorString)
        {

            emit errorOccurred(
                errorString);
        });

    // Volume

    connect(
        &m_audioOutput,
        &QAudioOutput::volumeChanged,
        this,
        [this]()
        {

            emit volumeChanged();
        });

    // Mute

    connect(
        &m_audioOutput,
        &QAudioOutput::mutedChanged,
        this,
        [this]()
        {
            emit mutedChanged();
        });
}

// Playback state

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

// Volume

float PlayerService::volume() const
{
    return m_audioOutput.volume();
}

bool PlayerService::isMuted() const
{
    return m_audioOutput.isMuted();
}

// Playback

void PlayerService::play()
{

    m_player.play();
}

void PlayerService::playUrl(
    const QString &url)
{
    if (url.isEmpty()) {

        return;
    }

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

    m_player.pause();
}

void PlayerService::resume()
{

    m_player.play();
}

void PlayerService::stop()
{

    m_player.stop();
}

void PlayerService::togglePlayback()
{

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

// Volume

void PlayerService::setVolume(
    float volume)
{
    const float clampedVolume =
        qBound(
            0.0f,
            volume,
            1.0f);

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

    m_audioOutput.setMuted(
        muted);
}

void PlayerService::toggleMute()
{
    const bool muted =
        m_audioOutput.isMuted();

    m_audioOutput.setMuted(
        !muted);
}