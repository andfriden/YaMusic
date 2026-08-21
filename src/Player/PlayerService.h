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

    bool isPlaying() const;

    QString currentUrl() const;

    qint64 position() const;

    qint64 duration() const;

public slots:
    void play();

    void playUrl(
        const QString &url);

    void pause();

    void resume();

    void stop();

    void togglePlayback();

    void seek(
        qint64 position);

    signals:
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

private:
    QMediaPlayer m_player;

    QAudioOutput m_audioOutput;

    QString m_currentUrl;
};