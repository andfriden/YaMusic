#include "GenreController.h"
#include "../Playback/PlaybackController.h"
#include "../Yandex/Catalog/GenreService.h"
#include "../Yandex/Catalog/GenreStationModel.h"
#include "../Yandex/Catalog/StationService.h"
#include "../Yandex/Personal/PlaylistService.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QSet>
#include <QStringList>
#include <QTextStream>

namespace {
QStringList splitCsvLineInternal(const QString &line) {
  QStringList result;
  QString current;
  bool quoted = false;

  for (int i = 0; i < line.size(); ++i) {
    const QChar ch = line.at(i);
    if (ch == '"') {
      if (quoted && i + 1 < line.size() && line.at(i + 1) == '"') {
        current += '"';
        ++i;
      } else {
        quoted = !quoted;
      }
      continue;
    }

    if (ch == ',' && !quoted) {
      result.append(current.trimmed());
      current.clear();
      continue;
    }

    current += ch;
  }

  result.append(current.trimmed());
  return result;
}

} // namespace

GenreController::GenreController(GenreService *genreService, PlaylistService *playlistService,
                                 StationService *stationService,
                                 PlaybackController *playbackController, QObject *parent)
    : QObject(parent), m_genreService(genreService), m_playlistService(playlistService),
      m_stationService(stationService), m_playbackController(playbackController),
      m_model(new GenreModel(this)), m_stationModel(new GenreStationModel(this)) {
  Q_ASSERT(m_genreService != nullptr);
  Q_ASSERT(m_playlistService != nullptr);
  Q_ASSERT(m_stationService != nullptr);
  Q_ASSERT(m_playbackController != nullptr);

  connect(m_genreService, &GenreService::genresReceived, this,
          [this](const QList<Genre> &genres) {
            QList<Genre> displayedGenres;
            displayedGenres.reserve(genres.size());

            for (const Genre &genre : genres) {
              if (!isDisplayedGenre(genre.id)) continue;
              displayedGenres.append(genre);
            }

            m_model->setGenres(displayedGenres);
            m_loading = false;
            emit loadingChanged();
            emit statusChanged({});
          });

    connect(m_genreService, &GenreService::tagPlaylistIdsReceived, this,
          [this](const QString &, const QList<QPair<QString, int>> &playlists) {
            if (!m_genreLoading || !m_waitingForTagPlaylistIds) {
              return;
            }

            m_waitingForTagPlaylistIds = false;

            if (playlists.isEmpty()) {
              finishGenrePlaylistLoading();
              return;
            }

            startGenrePlaylistQueue(playlists);
          });

  connect(m_genreService, &GenreService::errorOccurred, this, [this](const QString &message) {
    if (m_waitingForTagPlaylistIds) {
      m_waitingForTagPlaylistIds = false;
      finishGenrePlaylistLoading();
      emit errorOccurred(message);
      return;
    }

    emit errorOccurred(message);

    if (m_loading) {
      m_loading = false;
      emit loadingChanged();
      emit statusChanged({});
    }
  });

  connect(m_playlistService, &PlaylistService::playlistsReceived, this,
          [this](const QList<Playlist> &playlists) {
            if (!m_genreLoading || !m_waitingForPlaylists) {
              return;
            }

            m_waitingForPlaylists = false;
            QSet<QString> existingKeys;

            for (const Playlist &playlist : m_genrePlaylists) {
              existingKeys.insert(playlistKey(qMakePair(playlist.uid, playlist.kind)));
            }

            for (const Playlist &playlist : playlists) {
              if (playlist.uid.isEmpty() || playlist.kind <= 0) continue;
              const QString key = playlistKey(qMakePair(playlist.uid, playlist.kind));
              if (existingKeys.contains(key)) continue;
              existingKeys.insert(key);
              m_genrePlaylists.append(playlist);
              m_loadedAnyGenrePlaylist = true;
            }

            emit genreContentChanged();

            if (m_genrePlaylistQueuePosition < m_genrePlaylistQueue.size()) {
              loadNextGenrePlaylistBatch();
              return;
            }

            if (!m_loadedAnyGenrePlaylist && !m_usingApiFallback) {
              startApiFallback();
              return;
            }

            finishGenrePlaylistLoading();
          });

  connect(m_stationService, &StationService::stationTracksReceived, this,
          [this](const QList<Track> &tracks, const QString &batchId) {
            m_stationLoading = false;
            emit stationLoadingChanged();
            m_stationBatchId = batchId;
            m_stationModel->appendTracks(tracks);

            // Автозапуск первой партии
            if (!tracks.isEmpty() && m_stationModel->count() == tracks.size()) {
              selectStationTrack(0);
            }
          });

  connect(m_stationService, &StationService::errorOccurred, this, [this](const QString &message) {
    if (m_stationLoading) {
      m_stationLoading = false;
      emit stationLoadingChanged();
    }

    emit errorOccurred(message);
  });
}

bool GenreController::loading() const {
  return m_loading;
}

GenreModel *GenreController::model() const {
  return m_model;
}

bool GenreController::genreLoading() const {
  return m_genreLoading;
}

QVariantList GenreController::genrePlaylists() const {
  QVariantList result;
  result.reserve(m_genrePlaylists.size());

  for (const Playlist &playlist : m_genrePlaylists) {
    QVariantMap map;
    map.insert("uid", playlist.uid);
    map.insert("kind", playlist.kind);
    map.insert("title", playlist.title);
    map.insert("description", playlist.description);
    map.insert("trackCount", playlist.trackCount);
    map.insert("coverUri", playlist.coverUri);
    result.append(map);
  }
  return result;
}

void GenreController::loadGenres() {
  if (m_loading) {
    return;
  }

  m_loading = true;
  emit loadingChanged();
  emit statusChanged("Загрузка жанров...");
  m_genreService->loadGenres();
}

void GenreController::loadGenre(const QString &genreId) {
  const QString trimmedGenreId = genreId.trimmed();

  if (trimmedGenreId.isEmpty()) {
    emit errorOccurred("Идентификатор жанра не указан");
    return;
  }

  if (m_genreLoading) {
    return;
  }

  m_loadingGenreId = trimmedGenreId;
  m_genreLoading = true;
  m_waitingForPlaylists = false;
  m_waitingForTagPlaylistIds = false;
  m_usingApiFallback = false;
  m_csvHadCandidates = false;
  m_loadedAnyGenrePlaylist = false;
  m_genrePlaylistQueue.clear();
  m_genrePlaylistQueuePosition = 0;
  m_genrePlaylists.clear();
  emit genreLoadingChanged();
  emit genreContentChanged();
  emit statusChanged("Загрузка плейлистов...");
  const QList<QPair<QString, int>> csvPlaylists = loadPlaylistIdsFromCsv(trimmedGenreId);

  if (!csvPlaylists.isEmpty()) {
    m_csvHadCandidates = true;
    startGenrePlaylistQueue(csvPlaylists);
    return;
  }

  // В CSV кандидатов нет — уходим на API.

  startApiFallback();
}

void GenreController::loadTagPlaylists(const QString &tagId) {
  const QString trimmedTagId = tagId.trimmed();

  if (trimmedTagId.isEmpty()) {
    emit errorOccurred("Идентификатор поджанра не указан");
    return;
  }

  if (m_genreLoading) {
    return;
  }

  m_genreLoading = true;
  m_waitingForPlaylists = false;
  m_waitingForTagPlaylistIds = true;
  m_usingApiFallback = true;
  m_csvHadCandidates = false;
  m_loadedAnyGenrePlaylist = false;
  m_genrePlaylistQueue.clear();
  m_genrePlaylistQueuePosition = 0;
  m_genrePlaylists.clear();
  emit genreLoadingChanged();
  emit genreContentChanged();
  emit statusChanged("Загрузка плейлистов...");
  m_genreService->loadTagPlaylistIds(trimmedTagId);
}

void GenreController::startGenrePlaylistQueue(const QList<QPair<QString, int>> &playlists) {
  m_genrePlaylistQueue.clear();
  m_genrePlaylistQueuePosition = 0;
  QSet<QString> uniqueKeys;

  for (const QPair<QString, int> &playlist : playlists) {
    const QString uid = playlist.first.trimmed();
    const int kind = playlist.second;
    if (uid.isEmpty() || kind <= 0) continue;
    const QPair<QString, int> normalized = qMakePair(uid, kind);
    const QString key = playlistKey(normalized);
    if (uniqueKeys.contains(key)) continue;
    uniqueKeys.insert(key);
    m_genrePlaylistQueue.append(normalized);
  }

  if (m_genrePlaylistQueue.isEmpty()) {
    if (!m_usingApiFallback) {
      startApiFallback();
      return;
    }
    finishGenrePlaylistLoading();
    return;
  }

  loadNextGenrePlaylistBatch();
}

void GenreController::loadNextGenrePlaylistBatch() {
  if (!m_genreLoading) {
    return;
  }

  if (m_waitingForPlaylists) {
    return;
  }

  if (m_genrePlaylistQueuePosition >= m_genrePlaylistQueue.size()) {
    if (!m_loadedAnyGenrePlaylist && !m_usingApiFallback) {
      startApiFallback();
      return;
    }

    finishGenrePlaylistLoading();
    return;
  }

  const int start = m_genrePlaylistQueuePosition;
  const int end = qMin(start + GenrePlaylistBatchSize, m_genrePlaylistQueue.size());
  QList<QPair<QString, int>> batch;
  batch.reserve(end - start);

  for (int i = start; i < end; ++i) {
    batch.append(m_genrePlaylistQueue.at(i));
  }

  m_genrePlaylistQueuePosition = end;
  m_waitingForPlaylists = true;
  m_playlistService->loadPlaylists(batch);
}

void GenreController::startApiFallback() {
  if (!m_genreLoading) {
    return;
  }

  if (m_usingApiFallback) {
    finishGenrePlaylistLoading();
    return;
  }

  m_usingApiFallback = true;
  m_csvHadCandidates = false;
  m_genrePlaylistQueue.clear();
  m_genrePlaylistQueuePosition = 0;
  m_waitingForPlaylists = false;
  m_waitingForTagPlaylistIds = true;
  emit statusChanged("Получение плейлистов жанра...");

  // ID жанров каталога совпадают с tag ID, поэтому грузим плейлисты по ним.

  m_genreService->loadTagPlaylistIds(m_loadingGenreId);
}

void GenreController::finishGenrePlaylistLoading() {
  m_waitingForPlaylists = false;
  m_waitingForTagPlaylistIds = false;
  m_genrePlaylistQueue.clear();
  m_genrePlaylistQueuePosition = 0;

  if (m_genreLoading) {
    m_genreLoading = false;
    emit genreLoadingChanged();
  }

  emit genreContentChanged();
  emit statusChanged({});
}

GenreStationModel *GenreController::stationModel() const {
  return m_stationModel;
}

bool GenreController::stationLoading() const {
  return m_stationLoading;
}

void GenreController::loadGenreStation(const QString &genreId) {
  const QString id = genreId.trimmed();

  if (id.isEmpty()) {
    return;
  }

  if (m_stationLoading) {
    return;
  }

  m_stationGenreId = id;
  m_stationBatchId.clear();
  m_stationLoading = true;
  emit stationLoadingChanged();
  m_stationModel->clear();
  emit statusChanged("Загрузка радиостанции...");
  m_stationService->loadStationTracks("genre", id);
}

void GenreController::loadMoreGenreStation() {
  if (m_stationLoading) {
    return;
  }

  const QString lastId = m_stationModel->lastTrackId();

  if (lastId.isEmpty()) {
    return;
  }

  m_stationLoading = true;
  emit stationLoadingChanged();
  m_stationService->loadMoreStationTracks("genre", m_stationGenreId, lastId);
}

void GenreController::selectStationTrack(int index) {
  const QList<Track> tracks = m_stationModel->tracks();

  if (index < 0 || index >= tracks.size()) {
    return;
  }

  emit stationTrackSelected(index);

  m_playbackController->playFromSource(tracks, index, QStringLiteral("Радио: %1").arg(m_stationGenreId),
                                     "station");
}

QList<QPair<QString, int>> GenreController::loadPlaylistIdsFromCsv(const QString &genreId) {
  QList<QPair<QString, int>> playlists;
  QString csvPath = ":/qt/qml/YaMusic/data/genre_playlists.csv";
  QFile resourceFile(csvPath);
  if (!resourceFile.exists() || !resourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString currentPath = QCoreApplication::applicationDirPath();

    for (int level = 0; level < 6; ++level) {
      const QString candidate = currentPath + "/data/genre_playlists.csv";
      if (QFileInfo::exists(candidate)) {
        csvPath = candidate;
        break;
      }

      const QString parentPath = QFileInfo(currentPath).absolutePath();
      if (parentPath == currentPath) {
        break;
      }

      currentPath = parentPath;
    }
  }

  QFile file(csvPath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return playlists;
  }

  QTextStream stream(&file);
  if (stream.atEnd()) {
    return playlists;
  }

  const QString headerLine = stream.readLine();
  const QStringList headers = splitCsvLine(headerLine);
  int genreIdColumn = -1;
  int uidColumn = -1;
  int kindColumn = -1;

  for (int i = 0; i < headers.size(); ++i) {
    const QString header = headers.at(i).trimmed().toLower();
    if (header == "genre_id") {
      genreIdColumn = i;
    } else if (header == "uid") {
      uidColumn = i;
    } else if (header == "kind") {
      kindColumn = i;
    }
  }

  if (genreIdColumn < 0 || uidColumn < 0 || kindColumn < 0) {
    return playlists;
  }

  QSet<QString> uniqueKeys;

  while (!stream.atEnd()) {
    const QString line = stream.readLine();
    if (line.trimmed().isEmpty()) continue;
    const QStringList columns = splitCsvLine(line);
    const int requiredColumn = qMax(genreIdColumn, qMax(uidColumn, kindColumn));
    if (columns.size() <= requiredColumn) continue;
    const QString rowGenreId = columns.at(genreIdColumn).trimmed();
    if (rowGenreId != genreId) continue;
    const QString uid = columns.at(uidColumn).trimmed();
    bool ok = false;
    const int kind = columns.at(kindColumn).trimmed().toInt(&ok);
    if (uid.isEmpty() || !ok || kind <= 0) continue;
    const QPair<QString, int> playlist = qMakePair(uid, kind);
    const QString key = playlistKey(playlist);
    if (uniqueKeys.contains(key)) continue;
    uniqueKeys.insert(key);
    playlists.append(playlist);
  }
  return playlists;
}

QStringList GenreController::splitCsvLine(const QString &line) {
  return splitCsvLineInternal(line);
}

bool GenreController::isDisplayedGenre(const QString &genreId) {
  static const QSet<QString> displayedGenres = {
      "pop",     "allrock", "rap",         "metal",  "electronics", "dance",          "relax",
      "folk",    "jazz",    "alternative", "indie",  "rnb",         "classicalmusic", "punk",
      "country", "ska",     "soundtrack",  "reggae", "estrada",     "blues"};
  return displayedGenres.contains(genreId.trimmed());
}

QString GenreController::playlistKey(const QPair<QString, int> &playlist) {
  return QString("%1:%2").arg(playlist.first.trimmed()).arg(playlist.second);
}