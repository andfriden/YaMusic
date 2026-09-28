#include "StationsController.h"
#include "../Playback/PlaybackController.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Catalog/StationService.h"

StationsController::StationsController(StationService *stationService,
                                       PlaybackController *playbackController, QObject *parent)
    : QObject(parent), m_stationService(stationService),
      m_playbackController(playbackController), m_tracksModel(new GenreStationModel(this)) {
  Q_ASSERT(m_stationService != nullptr);
  Q_ASSERT(m_playbackController != nullptr);

  connect(m_stationService, &StationService::stationsReceived, this,
          [this](const QList<Station> &stations) {
            m_loading = false;
            emit loadingChanged();
            rebuildGroups(stations);
          });

  connect(m_stationService, &StationService::sessionStarted, this,
          [this](const QString &, const QString &, const QList<Track> &tracks) {
            m_stationLoading = false;
            m_stationActive = true;
            emit stationLoadingChanged();
            QueueService *queue = m_playbackController->queueService();
            queue->clear();
            queue->clearSource();
            queue->setRepeatMode(QueueService::RepeatOff);
            m_queuedTrackIds.clear();
            m_stationPlaying = false;
            m_stationNeedsResume = false;
            m_tracksModel->clear();
            appendTrackBatch(tracks);
          });

  connect(m_stationService, &StationService::moreSessionTracksReceived, this,
          [this](const QString &, const QList<Track> &tracks) {
            m_stationLoading = false;
            emit stationLoadingChanged();
            appendTrackBatch(tracks);
          });

  connect(m_stationService, &StationService::errorOccurred, this,
          [this](const QString &message) {
            m_loading = false;
            m_stationLoading = false;
            emit loadingChanged();
            emit stationLoadingChanged();
            emit statusChanged(message);
          });
}

QString StationsController::labelForType(const QString &type) {
  const QString t = type.trimmed();
  if (t == "user") return "Мои станции";
  if (t == "genre") return "Жанры";
  if (t == "mood") return "Настроение";
  if (t == "language") return "Язык";
  if (t == "decade") return "Десятилетия";
  if (t == "diversity") return "Разнообразие";
  if (t == "activity") return "Активности";
  return t;
}

int StationsController::typeOrder(const QString &type) {
  const QString t = type.trimmed();
  if (t == "user") return 0;
  if (t == "genre") return 1;
  if (t == "mood") return 2;
  if (t == "language") return 3;
  if (t == "decade") return 4;
  if (t == "diversity") return 5;
  if (t == "activity") return 6;
  return 100;
}

QVariantMap StationsController::stationToMap(const Station &station) const {
  QVariantMap map;
  map.insert("type", station.type);
  map.insert("tag", station.tag);
  map.insert("title", station.name);
  map.insert("imageUrl", station.imageUrl);
  map.insert("backgroundColor", station.backgroundColor);
  return map;
}

void StationsController::rebuildGroups(const QList<Station> &stations) {
  QMap<QString, QList<Station>> byType;

  for (const Station &station : stations) {
    if (byType.contains(station.type)) {
      byType[station.type].append(station);
    } else {
      byType.insert(station.type, {station});
    }
  }

  QStringList types = byType.keys();
  std::sort(types.begin(), types.end(), [](const QString &a, const QString &b) {
    const int orderA = typeOrder(a);
    const int orderB = typeOrder(b);
    if (orderA != orderB) return orderA < orderB;
    return a < b;
  });

  QVariantList groups;

  for (const QString &type : types) {
    QVariantMap group;
    group.insert("type", type);
    group.insert("label", labelForType(type));

    QVariantList items;
    for (const Station &station : byType.value(type)) {
      items.append(stationToMap(station));
    }
    group.insert("items", items);
    groups.append(group);
  }

  m_groups = groups;
  emit groupsChanged();
  emit statusChanged(QStringLiteral("Станции: %1").arg(stations.size()));
}

void StationsController::loadStations() {
  if (m_loading) {
    return;
  }

  m_loading = true;
  emit loadingChanged();
  emit statusChanged("Загрузка станций...");
  m_stationService->loadStations();
}

void StationsController::selectStation(const QString &type, const QString &tag,
                                       const QString &title) {
  const QString stationType = type.trimmed();
  const QString stationTag = tag.trimmed();

  if (stationType.isEmpty() || stationTag.isEmpty()) {
    return;
  }

  m_stationActive = false;
  m_stationLoading = true;
  m_stationTitle = title;
  emit stationLoadingChanged();
  emit statusChanged(QStringLiteral("Запуск станции: %1").arg(title));
  m_stationService->startStationSession(stationType, stationTag);
}

void StationsController::appendTrackBatch(const QList<Track> &tracks) {
  QueueService *queue = m_playbackController->queueService();
  QList<Track> toAdd;

  for (const Track &track : tracks) {
    if (track.id.isEmpty()) continue;
    if (m_queuedTrackIds.contains(track.id)) continue;
    m_queuedTrackIds.insert(track.id);
    toAdd.append(track);
  }

  if (toAdd.isEmpty()) {
    return;
  }

  queue->addTracks(toAdd);
  queue->setSource(QStringLiteral("Станция: %1").arg(m_stationTitle), "station");
  m_tracksModel->appendTracks(toAdd);
  tryAdvanceStation();
}

void StationsController::tryAdvanceStation() {
  QueueService *queue = m_playbackController->queueService();

  if (!m_stationPlaying) {
    if (queue->count() <= 0) {
      return;
    }
    m_stationPlaying = true;
    m_stationNeedsResume = false;
    if (!queue->setCurrentIndex(0)) {
      m_stationPlaying = false;
      return;
    }
    const Track first = queue->currentTrack();
    if (first.id.isEmpty()) {
      m_stationPlaying = false;
      return;
    }
    m_playbackController->playTrack(first);
    emit statusChanged(QStringLiteral("Играет станция: %1").arg(m_stationTitle));
    return;
  }

  if (!m_stationNeedsResume) {
    return;
  }

  m_stationNeedsResume = false;

  if (queue->hasNext()) {
    queue->next();
    const Track next = queue->currentTrack();
    if (!next.id.isEmpty()) {
      m_playbackController->playTrack(next);
    }
    return;
  }

  m_stationNeedsResume = true;
}

void StationsController::handleStationPlaylistExhausted() {
  if (!m_stationActive) {
    return;
  }

  if (m_stationLoading) {
    m_stationNeedsResume = true;
    return;
  }

  const QString token = m_tracksModel->lastQueueToken();

  if (token.isEmpty()) {
    m_stationActive = false;
    emit statusChanged("Станция не дала треков");
    return;
  }

  m_stationNeedsResume = true;
  m_stationLoading = true;
  emit stationLoadingChanged();
  emit statusChanged("Загрузка следующих треков станции...");
  m_stationService->loadMoreStationSession(token);
}

bool StationsController::loading() const {
  return m_loading;
}

QVariantList StationsController::groups() const {
  return m_groups;
}

bool StationsController::stationLoading() const {
  return m_stationLoading;
}

GenreStationModel *StationsController::tracksModel() const {
  return m_tracksModel;
}