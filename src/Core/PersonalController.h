#pragma once

#include "../Models/PersonalPlaylist.h"
#include "../Models/Podcast.h"
#include "../Models/Track.h"
#include "../Yandex/Personal/MyWaveModel.h"
#include "../Yandex/Personal/NewPlaylistsService.h"
#include "../Yandex/Personal/PersonalChartModel.h"
#include "../Yandex/Personal/PersonalLanding.h"
#include "../Yandex/Personal/PersonalPlaylistsModel.h"
#include "../Yandex/Personal/RecentListeningModel.h"
#include "../Yandex/Personal/RecentListeningService.h"

#include <QHash>
#include <QList>
#include <QObject>
#include <QSet>
#include <QString>

class PlayerService;
class PlaybackController;
class YandexPersonal;

class PersonalController : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool loadingMyWave READ isLoadingMyWave NOTIFY loadingMyWaveChanged)

  Q_PROPERTY(bool loadingMoreMyWave READ isLoadingMoreMyWave NOTIFY loadingMoreMyWaveChanged)

  Q_PROPERTY(bool loadingRecommendations READ isLoadingRecommendations NOTIFY
                 loadingRecommendationsChanged)

  Q_PROPERTY(MyWaveModel *myWaveModel READ myWaveModel CONSTANT)

  Q_PROPERTY(PersonalPlaylistsModel *personalPlaylistsModel READ personalPlaylistsModel CONSTANT)

  Q_PROPERTY(RecentListeningModel *recentListeningModel READ recentListeningModel CONSTANT)

  Q_PROPERTY(PersonalChartModel *chartModel READ chartModel CONSTANT)

  Q_PROPERTY(QVariantList podcasts READ podcasts NOTIFY podcastsChanged)

  Q_PROPERTY(QVariantList wheelItems READ wheelItems NOTIFY wheelItemsChanged)

public:
  explicit PersonalController(YandexPersonal *yandexPersonal,
                              PersonalLanding *personalLanding,
                              NewPlaylistsService *newPlaylistsService,
                              RecentListeningService *recentListeningService,
                              PlaybackController *playbackController,
                              PlayerService *playerService,
                              QObject *parent = nullptr);

  Q_INVOKABLE void loadMyWave();

  Q_INVOKABLE void loadMoreMyWave();

  Q_INVOKABLE void selectMyWaveTrack(int index);

  Q_INVOKABLE void loadRecommendations();

  Q_INVOKABLE void loadWheel();

  Q_INVOKABLE void selectPersonalPlaylist(const QString &uid, int kind);

  Q_INVOKABLE void selectRecentListening(int index);

  // Деактивирует сессию «Моей волны»: при выходе в обычный плейлист,
  // альбом или артиста волна не должна перехватывать навигацию и «далее».
  void deactivateMyWave();

  MyWaveModel *myWaveModel() const;

  PersonalPlaylistsModel *personalPlaylistsModel() const;

  RecentListeningModel *recentListeningModel() const;

  PersonalChartModel *chartModel() const;

  QVariantList podcasts() const;

  QVariantList wheelItems() const;

  // Плейлисты из раздела «personal-playlists» в виде QVariantList
  // (uid/kind/title/coverUri), без указанного текущего.
  // Используется как fallback для «Похожие плейлисты» у личных
  // подборок (Премьера, Дежавю, Тайник, Плейлист дня).
  QVariantList recommendationPlaylistsData(const QString &excludeUid = {},
                                           int excludeKind = 0) const;

  // Альбомы секции «new-releases» лендинга в виде QVariantList
  // (albumId/title/coverUri/year) — для страницы «Новые релизы».
  Q_INVOKABLE QVariantList newReleasesAlbums() const;

  bool isLoadingMyWave() const;

  bool isLoadingMoreMyWave() const;

  bool isLoadingRecommendations() const;

  // Вызывается AppController, когда PlaybackController
  // полностью дошёл до конца очереди My Wave.
  void handleMyWavePlaybackFinished();

  // Фидбек ротора на дизлайк трека Моей волны (dislike/undislike):
  // учитывается только для трека текущей очереди.
  void reportMyWaveDislike(const QString &trackId, bool disliked);

  // Пропуск трека волны кнопкой «Следующий»: отправляет ротору фидбек
  // skip, но очередь не трогает — играет следующий трек уже загруженной
  // партии. true — волна активна и фидбек отправлен.
  bool skipMyWaveTrack();

signals:
  void statusChanged(const QString &message);

  void loadingMyWaveChanged();

  void loadingMoreMyWaveChanged();

  void loadingRecommendationsChanged();

  void recommendationsLoaded();

  void podcastsChanged();

  void wheelItemsChanged();

  void myWaveTrackSelected(const Track &track);

  void personalPlaylistSelected(const PersonalPlaylist &playlist);

private:
  void connectMyWave();

  void connectRecommendations();

  void connectRecentlyPlayed();

  void connectPlayback();

  void rebuildRecommendationSections();

  void handleMyWaveReceived(const QList<Track> &tracks, const QString &batchId);

  void startMyWaveQueue(int index);

  void appendMyWaveTracksToQueue(const QList<Track> &tracks);

  void stopCurrentMyWaveTrack(const QString &event);

  void sendMyWaveFeedback(const QString &event,
                          const QString &trackId,
                          qint64 totalPlayedSeconds = 0);

  QString batchIdForTrack(const QString &trackId) const;

  // Замена очереди волны свежей партией: ротор пересчитывает
  // последовательность, поэтому оставшееся в очереди отбрасывается.
  void replaceMyWaveQueue(const QString &fromTrackId);

  // Отмечает трек волны как сыгранный, чтобы ротор не приносил его снова.
  void markMyWaveTrackPlayed(const QString &trackId);

private:
  YandexPersonal *m_yandexPersonal = nullptr;

  PersonalLanding *m_personalLanding = nullptr;

  NewPlaylistsService *m_newPlaylistsService = nullptr;

  RecentListeningService *m_recentListeningService = nullptr;

  PlaybackController *m_playbackController = nullptr;

  PlayerService *m_playerService = nullptr;

  MyWaveModel *m_myWaveModel = nullptr;

  PersonalPlaylistsModel *m_personalPlaylistsModel = nullptr;

  RecentListeningModel *m_recentListeningModel = nullptr;

  PersonalChartModel *m_chartModel = nullptr;

  QList<PersonalLandingSection> m_recommendationSections;

  QList<PersonalPlaylist> m_recommendationPlaylists;

  QList<Podcast> m_podcasts;

  QVariantList m_wheelItems;

  // настройки и прочее
  QList<QPair<QString, int>> m_personalPlaylistQueue;

  bool m_loadingMyWave = false;

  bool m_loadingMoreMyWave = false;

  bool m_loadingRecommendations = false;

  bool m_myWaveQueueActive = false;

  bool m_waitingForMoreMyWave = false;

  QHash<QString, QString> m_myWaveTrackBatches;

  // Недавно сыгранное волной: id треков и порядок появления, чтобы
  // отсекать повторы и не держать историю бесконечно.
  QSet<QString> m_recentMyWaveTrackIds;

  QList<QString> m_recentMyWaveOrder;

  // Следующая партия должна заменить очередь, а не дополнить её.
  bool m_replaceMyWaveQueue = false;

  // Потолок очереди волны — она не растёт бесконечно.
  static constexpr int kMaxQueueTracks = 100;

  static constexpr int kRecentTrackLimit = 30;

  QString m_currentMyWaveTrackId;

  bool m_myWaveTrackStarted = false;
};