#pragma once

#include <QHash>
#include <QObject>
#include <QString>

#include "../Models/PersonalPlaylist.h"
#include "../Models/Track.h"

#include "../Yandex/Personal/MyWaveModel.h"
#include "../Yandex/Personal/PersonalLanding.h"
#include "../Yandex/Personal/PersonalPlaylistsModel.h"
#include "../Yandex/Personal/RecentListeningModel.h"
#include "../Yandex/Personal/RecentListeningService.h"

class PlayerService;
class PlaybackController;
class YandexPersonal;

class PersonalController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool loadingMyWave
        READ isLoadingMyWave
        NOTIFY loadingMyWaveChanged)

    Q_PROPERTY(
        bool loadingMoreMyWave
        READ isLoadingMoreMyWave
        NOTIFY loadingMoreMyWaveChanged)

    Q_PROPERTY(
        bool loadingRecommendations
        READ isLoadingRecommendations
        NOTIFY loadingRecommendationsChanged)

    Q_PROPERTY(
        MyWaveModel *myWaveModel
        READ myWaveModel
        CONSTANT)

    Q_PROPERTY(
        PersonalPlaylistsModel *personalPlaylistsModel
        READ personalPlaylistsModel
        CONSTANT)

    Q_PROPERTY(
        RecentListeningModel *recentListeningModel
        READ recentListeningModel
        CONSTANT)

public:
    explicit PersonalController(
        YandexPersonal *yandexPersonal,
        PersonalLanding *personalLanding,
        RecentListeningService *recentListeningService,
        PlaybackController *playbackController,
        PlayerService *playerService,
        QObject *parent = nullptr);

    void loadMyWave();

    void loadMoreMyWave();

    void loadRecommendations();

    void selectMyWaveTrack(
        int index);

    void selectPersonalPlaylist(
        int index);

    void selectRecentListening(
        int index);

    MyWaveModel *myWaveModel() const;

    PersonalPlaylistsModel *
    personalPlaylistsModel() const;

    RecentListeningModel *
    recentListeningModel() const;

    bool isLoadingMyWave() const;

    bool isLoadingMoreMyWave() const;

    bool isLoadingRecommendations() const;

signals:
    void statusChanged(
        const QString &message);

    void loadingMyWaveChanged();

    void loadingMoreMyWaveChanged();

    void loadingRecommendationsChanged();

    void recommendationsLoaded();

    void myWaveTrackSelected(
        const Track &track);

    void personalPlaylistSelected(
        const PersonalPlaylist &playlist);

private:
    /*
     * Connections
     */

    void connectMyWave();

    void connectRecommendations();

    void connectRecentlyPlayed();

    void connectPlayback();

    /*
     * My Wave
     */

    void handleMyWaveReceived(
        const QList<Track> &tracks,
        const QString &batchId);

    void handleMyWavePlaybackFinished();

    void startMyWaveQueue(
        int index);

    void appendMyWaveTracksToQueue(
        const QList<Track> &tracks);

    void stopCurrentMyWaveTrack(
        const QString &event);

    void sendMyWaveFeedback(
        const QString &event,
        const QString &trackId,
        qint64 totalPlayedSeconds = 0);

    QString batchIdForTrack(
        const QString &trackId) const;

private:
    YandexPersonal *m_yandexPersonal = nullptr;

    PersonalLanding *m_personalLanding = nullptr;

    RecentListeningService *
        m_recentListeningService = nullptr;

    PlaybackController *
        m_playbackController = nullptr;

    PlayerService *
        m_playerService = nullptr;

    MyWaveModel *
        m_myWaveModel = nullptr;

    PersonalPlaylistsModel *
        m_personalPlaylistsModel = nullptr;

    RecentListeningModel *
        m_recentListeningModel = nullptr;

    bool m_loadingMyWave = false;

    bool m_loadingMoreMyWave = false;

    bool m_loadingRecommendations = false;

    bool m_myWaveQueueActive = false;

    bool m_waitingForMoreMyWave = false;

    QHash<QString, QString>
        m_myWaveTrackBatches;

    QString m_currentMyWaveTrackId;

    bool m_myWaveTrackStarted = false;
};