#include "PersonalController.h"

#include "../Playback/PlaybackController.h"
#include "../Player/PlayerService.h"
#include "../Yandex/Personal/YandexPersonal.h"

PersonalController::PersonalController(
    YandexPersonal *yandexPersonal,
    PersonalLanding *personalLanding,
    RecentListeningService *recentListeningService,
    PlaybackController *playbackController,
    PlayerService *playerService,
    QObject *parent)
    : QObject(parent)
    , m_yandexPersonal(yandexPersonal)
    , m_personalLanding(personalLanding)
    , m_recentListeningService(recentListeningService)
    , m_playbackController(playbackController)
    , m_playerService(playerService)
    , m_myWaveModel(new MyWaveModel(this))
    , m_personalPlaylistsModel(
          new PersonalPlaylistsModel(this))
    , m_recentListeningModel(
          new RecentListeningModel(this))
{
    connectMyWave();
    connectRecommendations();
    connectRecentlyPlayed();
    connectPlayback();
}

MyWaveModel *
PersonalController::myWaveModel() const
{
    return m_myWaveModel;
}

PersonalPlaylistsModel *
PersonalController::personalPlaylistsModel() const
{
    return m_personalPlaylistsModel;
}

RecentListeningModel *
PersonalController::recentListeningModel() const
{
    return m_recentListeningModel;
}

bool PersonalController::isLoadingMyWave() const
{
    return m_loadingMyWave;
}

bool PersonalController::isLoadingMoreMyWave() const
{
    return m_loadingMoreMyWave;
}

bool PersonalController::isLoadingRecommendations() const
{
    return m_loadingRecommendations;
}