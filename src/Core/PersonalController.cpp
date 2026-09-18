#include "PersonalController.h"
#include "../Yandex/Personal/YandexPersonal.h"

PersonalController::PersonalController(YandexPersonal *yandexPersonal,
                                       PersonalLanding *personalLanding,
                                       NewPlaylistsService *newPlaylistsService,
                                       RecentListeningService *recentListeningService,
                                       PlaybackController *playbackController,
                                       PlayerService *playerService, QObject *parent)
    : QObject(parent), m_yandexPersonal(yandexPersonal), m_personalLanding(personalLanding),
      m_newPlaylistsService(newPlaylistsService), m_recentListeningService(recentListeningService),
      m_playbackController(playbackController), m_playerService(playerService),
      m_myWaveModel(new MyWaveModel(this)),
      m_personalPlaylistsModel(new PersonalPlaylistsModel(this)),
      m_recentListeningModel(new RecentListeningModel(this)),
      m_chartModel(new PersonalChartModel(this)) {
  connectMyWave();
  connectRecommendations();
  connectRecentlyPlayed();
  connectPlayback();

  if (m_newPlaylistsService != nullptr) {
    connect(m_newPlaylistsService, &NewPlaylistsService::playlistsReceived, this,
            [this](const QList<Playlist> &playlists) {
              if (playlists.isEmpty()) {
                return;
              }

              PersonalLandingSection section;
              section.id = "new-playlists";
              section.title = "Новые плейлисты";
              section.type = "new-playlists";

              for (const Playlist &playlist : playlists) {
                if (playlist.uid.isEmpty() || playlist.kind <= 0) continue;
                PersonalPlaylist personalPlaylist;
                personalPlaylist.uid = playlist.uid;
                personalPlaylist.kind = playlist.kind;

                personalPlaylist.id =
                    personalPlaylist.uid + ":" + QString::number(personalPlaylist.kind);
                personalPlaylist.title = playlist.title;
                personalPlaylist.description = playlist.description;
                personalPlaylist.trackCount = playlist.trackCount;
                personalPlaylist.coverUri = playlist.coverUri;
                if (personalPlaylist.title.isEmpty()) continue;
                section.playlists.append(personalPlaylist);
              }

              if (section.playlists.isEmpty()) {
                return;
              }

              bool replaced = false;

              for (PersonalLandingSection &existingSection : m_recommendationSections) {
                if (existingSection.type == "new-playlists") {
                  existingSection = section;
                  replaced = true;
                  break;
                }
              }

              if (!replaced) {
                m_recommendationSections.append(section);
              }

              m_recommendationPlaylists.clear();

              for (const PersonalLandingSection &recommendationSection : m_recommendationSections) {
                for (const PersonalPlaylist &personalPlaylist : recommendationSection.playlists) {
                  if (!personalPlaylist.uid.isEmpty()) {
                    m_recommendationPlaylists.append(personalPlaylist);
                  }
                }
              }

              if (m_personalPlaylistsModel != nullptr) {
                m_personalPlaylistsModel->setSections(m_recommendationSections);
              }

              emit statusChanged(
                  QString("Новых плейлистов загружено: %1").arg(section.playlists.size()));
            });
  }
}

MyWaveModel *PersonalController::myWaveModel() const {
  return m_myWaveModel;
}

PersonalPlaylistsModel *PersonalController::personalPlaylistsModel() const {
  return m_personalPlaylistsModel;
}

RecentListeningModel *PersonalController::recentListeningModel() const {
  return m_recentListeningModel;
}

PersonalChartModel *PersonalController::chartModel() const {
  return m_chartModel;
}

QVariantList PersonalController::recommendationPlaylistsData(const QString &excludeUid,
                                                             int excludeKind) const {
  QVariantList result;

  // Берём только раздел «Собираем для вас» (personal-playlists).
  for (const PersonalLandingSection &section : m_recommendationSections) {
    if (section.type != "personal-playlists") continue;

    for (const PersonalPlaylist &playlist : section.playlists) {
      if (playlist.uid.isEmpty() || playlist.kind <= 0) continue;

      // Исключаем текущий открытый плейлист.
      if (playlist.uid == excludeUid && playlist.kind == excludeKind) continue;
      QVariantMap item;
      item.insert("uid", playlist.uid);
      item.insert("kind", playlist.kind);
      item.insert("title", playlist.title);
      item.insert("coverUri", playlist.coverUri);
      result.append(item);
    }
  }
  return result;
}

bool PersonalController::isLoadingMyWave() const {
  return m_loadingMyWave;
}

bool PersonalController::isLoadingMoreMyWave() const {
  return m_loadingMoreMyWave;
}

bool PersonalController::isLoadingRecommendations() const {
  return m_loadingRecommendations;
}