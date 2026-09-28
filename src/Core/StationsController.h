#pragma once

#include "../Models/Station.h"
#include "../Yandex/Catalog/GenreStationModel.h"
#include <QObject>
#include <QSet>
#include <QString>
#include <QVariantList>

class PlaybackController;
class PlayerService;
class StationService;

// Контроллер раздела «Станции»: список станций, сгруппированный по типу.
// Группировка и фиксированный порядок секций — здесь; оба QML-вида
// (полки / табы) читают один и тот же список `groups`.
class StationsController : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

  Q_PROPERTY(QVariantList groups READ groups NOTIFY groupsChanged)

  Q_PROPERTY(bool stationLoading READ stationLoading NOTIFY stationLoadingChanged)

  Q_PROPERTY(GenreStationModel *tracksModel READ tracksModel CONSTANT)

public:
  explicit StationsController(StationService *stationService, PlaybackController *playbackController,
                              PlayerService *playerService, QObject *parent = nullptr);

  Q_INVOKABLE void loadStations();

  Q_INVOKABLE void selectStation(const QString &type, const QString &tag, const QString &title);

  // Вызывается из AppController при playlistExhausted источника "station".
  void handleStationPlaylistExhausted();

  bool isStationActive() const;

  // Пропуск текущего трека станции (сыграно x сек).
  void reportStationSkip();

  // Изменение лайка трека станции (только фидбек ротора, лайки уже сделаны).
  void reportStationLikeChanged(const QString &trackId, bool liked);

  bool loading() const;

  QVariantList groups() const;

  bool stationLoading() const;

  GenreStationModel *tracksModel() const;

signals:
  void loadingChanged();

  void groupsChanged();

  void stationLoadingChanged();

  void statusChanged(const QString &message);

private:
  void rebuildGroups(const QList<Station> &stations);

  void appendTrackBatch(const QList<Track> &tracks);

  void tryAdvanceStation();

  bool isStationSource() const;

  static QString labelForType(const QString &type);

  static int typeOrder(const QString &type);

  QVariantMap stationToMap(const Station &station) const;

  StationService *m_stationService = nullptr;

  PlaybackController *m_playbackController = nullptr;

  PlayerService *m_playerService = nullptr;

  GenreStationModel *m_tracksModel = nullptr;

  QVariantList m_groups;

  bool m_loading = false;
  bool m_stationLoading = false;
  bool m_stationActive = false;
  bool m_stationPlaying = false;
  bool m_stationNeedsResume = false;
  QString m_stationTitle;
  QString m_currentStationTrackId;
  bool m_stationTrackStarted = false;
  QSet<QString> m_queuedTrackIds;
  QList<QString> m_queueTokens;
};