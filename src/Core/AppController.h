#pragma once

#include <QObject>
#include <QString>

#include "../Playback/PlaybackController.h"
#include "../Yandex/Catalog/SearchModel.h"

class AccountService;
class PlayerService;
class SearchService;
class TrackService;
class YandexAuth;

class AppController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool searching
        READ isSearching
        NOTIFY searchingChanged)

    Q_PROPERTY(
        bool playing
        READ isPlaying
        NOTIFY playingChanged)

    Q_PROPERTY(
        SearchModel *searchModel
        READ searchModel
        CONSTANT)

    Q_PROPERTY(
        QString currentTrackTitle
        READ currentTrackTitle
        NOTIFY currentTrackChanged)

    Q_PROPERTY(
        QString currentTrackArtist
        READ currentTrackArtist
        NOTIFY currentTrackChanged)

    Q_PROPERTY(
        QString currentTrackCoverUri
        READ currentTrackCoverUri
        NOTIFY currentTrackChanged)

    Q_PROPERTY(
        qint64 position
        READ position
        NOTIFY positionChanged)

    Q_PROPERTY(
        qint64 duration
        READ duration
        NOTIFY durationChanged)

    Q_PROPERTY(
        PlaybackController::PlaybackState playbackState
        READ playbackState
        NOTIFY playbackStateChanged)

public:
    explicit AppController(
        QObject *parent = nullptr);

    Q_INVOKABLE void testConnection();

    Q_INVOKABLE void testYandexApi();

    Q_INVOKABLE void testSearch(
        const QString &query);

    Q_INVOKABLE void selectSearchResult(
        int index);

    Q_INVOKABLE void play();

    Q_INVOKABLE void pause();

    Q_INVOKABLE void stop();

    Q_INVOKABLE void seek(
        qint64 position);

    SearchModel *searchModel() const;

    bool isSearching() const;

    bool isPlaying() const;

    QString currentTrackTitle() const;

    QString currentTrackArtist() const;

    QString currentTrackCoverUri() const;

    qint64 position() const;

    qint64 duration() const;

    PlaybackController::PlaybackState
    playbackState() const;

signals:
    void statusChanged(
        const QString &message);

    void searchingChanged();

    void playingChanged();

    void currentTrackChanged();

    void positionChanged();

    void durationChanged();

    void playbackStateChanged();

private:
    YandexAuth *m_auth = nullptr;

    AccountService *m_accountService = nullptr;

    SearchService *m_searchService = nullptr;

    TrackService *m_trackService = nullptr;

    SearchModel *m_searchModel = nullptr;

    PlayerService *m_playerService = nullptr;

    PlaybackController *m_playbackController = nullptr;

    bool m_searching = false;
};