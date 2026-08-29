#pragma once

#include <QObject>
#include <QString>
#include <QUrl>

#include <QAudioOutput>
#include <QMediaPlayer>


class PlayerService : public QObject
{
    Q_OBJECT

public:

    explicit PlayerService(
        QObject *parent = nullptr);


    // =========================================================
    // Playback state
    // =========================================================

    bool isPlaying() const;

    QString currentUrl() const;

    qint64 position() const;

    qint64 duration() const;


    // =========================================================
    // Volume
    // =========================================================

    float volume() const;

    bool isMuted() const;


public slots:

    // =========================================================
    // Playback
    // =========================================================

    void play();

    void playUrl(
        const QString &url);

    void pause();

    void resume();

    void stop();

    void togglePlayback();

    void seek(
        qint64 position);


    // =========================================================
    // Volume
    // =========================================================

    void setVolume(
        float volume);

    void setMuted(
        bool muted);

    void toggleMute();


signals:

    // =========================================================
    // Playback
    // =========================================================

    void playingChanged();

    void playbackStarted();

    void playbackPaused();

    void playbackStopped();

    void playbackFinished();

    void currentUrlChanged();

    void positionChanged(
        qint64 position);

    void durationChanged(
        qint64 duration);

    void errorOccurred(
        const QString &message);


    // =========================================================
    // Volume
    // =========================================================

    void volumeChanged();

    void mutedChanged();


private:

    QMediaPlayer m_player;

    QAudioOutput m_audioOutput;

    QString m_currentUrl;
};