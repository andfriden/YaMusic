#pragma once

#include <QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>

class PlayerService : public QObject
{
    Q_OBJECT

public:
    explicit PlayerService(QObject *parent = nullptr);

    bool isPlaying() const;
    QString currentUrl() const;

public slots:
    void play();
    void playUrl(const QString &url);
    void pause();
    void resume();
    void stop();
    void togglePlayback();

    signals:
        void playingChanged();

    void playbackStarted();
    void playbackPaused();
    void playbackStopped();

    void currentUrlChanged();

    void errorOccurred(const QString &message);

private:
    QMediaPlayer m_player;
    QAudioOutput m_audioOutput;

    QString m_currentUrl;
};