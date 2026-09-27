#include "ArtistController.h"
#include "../Playback/PlaybackController.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Catalog/AlbumService.h"
#include "../Yandex/Catalog/ArtistService.h"

ArtistController::ArtistController(ArtistService *artistService, AlbumService *albumService,
                                   PlaybackController *playbackController, QObject *parent)
    : QObject(parent), m_artistService(artistService), m_albumService(albumService),
      m_playbackController(playbackController), m_artistModel(new ArtistModel(this)),
      m_albumsModel(new ArtistAlbumsModel(this)), m_similarArtistsModel(new SimilarArtistsModel(this)) {
  Q_ASSERT(m_artistService != nullptr);
  Q_ASSERT(m_albumService != nullptr);
  Q_ASSERT(m_playbackController != nullptr);

  connect(m_artistService, &ArtistService::artistReceived, this,
          [this](const ArtistDetails &artist) {
            m_loading = false;
            m_artistId = artist.id;
            m_artistName = artist.name;

            m_artistCoverUri = artist.coverUri;

            // В brief-info обложка может быть пустой — берём
            // обложку первого трека, а если и её нет,
            // то обложку популярного альбома.
            if (m_artistCoverUri.isEmpty()) {
              for (const Track &track : artist.tracks) {
                if (!track.coverUri.isEmpty()) {
                  m_artistCoverUri = track.coverUri;
                  break;
                }
              }
            }

            if (m_artistCoverUri.isEmpty()) {
              for (const Album &album : artist.popularAlbums) {
                if (!album.coverUri.isEmpty()) {
                  m_artistCoverUri = album.coverUri;
                  break;
                }
              }
            }

            m_artistDescription = artist.description;
            m_artistGenres = artist.genres.join(", ");
            m_newRelease = artist.newRelease;
            m_artistModel->setArtist(artist);

            // Не затираем модели альбомов популярной выборкой,
            // если полный список (direct-albums) уже пришёл —
            // иначе гонка между artistReceived и artistAlbumsReceived
            // оставляет на странице только популярные (или пусто).
            if (m_allAlbums.isEmpty()) {
              m_allAlbums = artist.popularAlbums;
              applyAlbumFilter();
            }

            m_similarArtistsModel->setArtists(artist.similarArtists);

            emit loadingChanged();
            emit artistChanged();
            emit statusChanged(QStringLiteral("Исполнитель загружен: %1").arg(artist.name));
          });

  connect(m_artistService, &ArtistService::errorOccurred, this, [this](const QString &message) {
    m_loading = false;
    emit loadingChanged();
    emit statusChanged(QStringLiteral("Ошибка загрузки исполнителя: %1").arg(message));
  });

  connect(m_artistService, &ArtistService::artistAlbumsReceived, this,
          [this](const QString &artistId, const QList<Album> &albums) {
            // Радио артиста: альбомы текущего радио-артиста идут в радио,
            // альбомы отображаемого на странице исполнителя — на страницу.
            if (m_radioActive && m_radioLoading && artistId == m_radioCurrentArtistId) {
              startLoadingRadioAlbums(albums);
              return;
            }

            if (!m_radioActive || artistId != m_radioCurrentArtistId) {
              if (artistId == m_artistId) {
                m_allAlbums = albums;
                applyAlbumFilter();
              }
            }
          });

  connect(m_artistService, &ArtistService::similarArtistsReceived, this,
          [this](const QString &artistId, const QList<Artist> &artists) {
            if (!m_radioActive || artistId != m_radioCurrentArtistId) {
              return;
            }

            QList<Artist> cleaned;
            for (const Artist &artist : artists) {
              if (artist.id.isEmpty()) continue;
              cleaned.append(artist);
            }

            m_radioSimilarPool = cleaned;
            m_radioSimilarLoaded = true;

            if (m_radioWaitingForSimilar) {
              m_radioWaitingForSimilar = false;
              continueRadioToNextArtist();
            }
          });

  connect(m_albumService, &AlbumService::albumReceived, this,
          [this](const AlbumDetails &details) {
            if (!m_radioActive || !m_radioLoading) {
              return;
            }

            const QString id = details.album.id;
            if (id.isEmpty() || m_pendingAlbumIds.isEmpty()) {
              return;
            }

            // Загружаем альбомы последовательно, сохраняя их порядок.
            if (m_pendingAlbumIds.first() != id) {
              return;
            }
            m_pendingAlbumIds.removeFirst();

            m_radioArtistQueued += appendRadioAlbumTracks(details.tracks);

            if (m_pendingAlbumIds.isEmpty()) {
              m_radioLoading = false;
              emit radioLoadingChanged();
              continueIfStalled();
            } else {
              loadRadioAlbum(m_pendingAlbumIds.first());
            }
          });
}

void ArtistController::loadArtist(const QString &id) {
  const QString artistId = id.trimmed();

  if (artistId.isEmpty()) {
    emit statusChanged("ID исполнителя не указан");
    return;
  }

  if (m_loading) {
    return;
  }

  m_loading = true;
  emit loadingChanged();
  m_artistId.clear();
  m_artistName.clear();
  m_artistCoverUri.clear();
  m_artistDescription.clear();
  m_artistGenres.clear();
  m_newRelease = {};
  m_albumFilterType.clear();
  m_allAlbums.clear();
  m_artistModel->clear();
  m_albumsModel->clear();
  m_similarArtistsModel->clear();
  emit artistChanged();
  emit albumFilterChanged();
  emit statusChanged(QStringLiteral("Загрузка исполнителя: %1").arg(artistId));
  m_artistService->loadArtist(artistId);
  m_artistService->loadArtistAlbums(artistId);
}

void ArtistController::selectTrack(int index) {
  const Track track = m_artistModel->trackAt(index);

  if (track.id.isEmpty()) {
    emit statusChanged("Некорректный трек исполнителя");
    return;
  }

  emit trackSelected(track);
  m_playbackController->playFromSource(m_artistModel->tracks(), index, m_artistName, "artist");
}

void ArtistController::selectSimilarArtist(int index) {
  const Artist artist = m_similarArtistsModel->artistAt(index);

  if (artist.id.isEmpty()) {
    emit statusChanged("Некорректный похожий исполнитель");
    return;
  }

  emit similarArtistSelected(artist.id);
}

void ArtistController::playArtist() {
  startArtistRadio();
}

void ArtistController::continueFromAlbumRadio(const QString &artistId, const QString &artistName,
                                              const QStringList &playedTrackIds) {
  const QString id = artistId.trimmed();

  if (id.isEmpty()) {
    emit statusChanged("Нет исполнителя для продолжения альбома");
    return;
  }

  m_radioActive = true;
  m_radioVisitedArtists.clear();
  m_radioQueuedTrackIds.clear();

  for (const QString &trackId : playedTrackIds) {
    if (!trackId.isEmpty()) {
      m_radioQueuedTrackIds.insert(trackId);
    }
  }

  // Альбом уже играл — продолжаем с него, не перезапуская очередь.
  m_radioPlaying = true;
  m_radioNeedsResume = true;
  m_radioWaitingForSimilar = false;
  m_radioSimilarPool.clear();

  QueueService *queue = m_playbackController->queueService();
  queue->setRepeatMode(QueueService::RepeatOff);

  beginRadioArtist(id, artistName);
}

void ArtistController::startArtistRadio() {
  if (m_artistId.isEmpty()) {
    emit statusChanged("Нет исполнителя для радио");
    return;
  }

  m_radioActive = true;
  m_radioVisitedArtists.clear();
  m_radioQueuedTrackIds.clear();
  m_radioPlaying = false;
  m_radioNeedsResume = false;
  m_radioWaitingForSimilar = false;
  m_radioSimilarPool.clear();

  QueueService *queue = m_playbackController->queueService();
  queue->clear();
  queue->clearSource();
  queue->setRepeatMode(QueueService::RepeatOff);

  beginRadioArtist(m_artistId, m_artistName);
}

void ArtistController::beginRadioArtist(const QString &artistId, const QString &artistName) {
  const QString id = artistId.trimmed();

  if (id.isEmpty()) {
    if (m_radioActive) {
      endRadioArtist();
    }
    return;
  }

  m_radioActive = true;
  m_radioCurrentArtistId = id;
  m_radioCurrentArtistName = artistName;
  m_radioLoading = true;
  m_radioWaitingForSimilar = false;
  m_radioSimilarLoaded = false;
  m_radioArtistQueued = 0;
  m_pendingAlbumIds.clear();
  m_radioVisitedArtists.insert(id);
  emit radioLoadingChanged();
  emit statusChanged(QStringLiteral("Радио исполнителя: %1").arg(artistName));

  // Для похожих исполнителей цепочки берём их список похожих, чтобы продолжать по ним.
  m_artistService->loadSimilarArtists(id);

  if (id == m_artistId && !m_allAlbums.isEmpty()) {
    startLoadingRadioAlbums(m_allAlbums);
  } else {
    m_artistService->loadArtistAlbums(id);
  }
}

void ArtistController::startLoadingRadioAlbums(const QList<Album> &albums) {
  m_pendingAlbumIds.clear();

  for (const Album &album : albums) {
    const QString id = album.id.trimmed();
    if (id.isEmpty()) continue;
    m_pendingAlbumIds.append(id);
  }

  if (m_pendingAlbumIds.isEmpty()) {
    m_radioLoading = false;
    emit radioLoadingChanged();
    continueIfStalled();
    return;
  }

  loadRadioAlbum(m_pendingAlbumIds.first());
}

void ArtistController::loadRadioAlbum(const QString &albumId) {
  m_albumService->loadAlbum(albumId);
}

int ArtistController::appendRadioAlbumTracks(const QList<Track> &tracks) {
  QueueService *queue = m_playbackController->queueService();
  QList<Track> toAdd;

  for (const Track &track : tracks) {
    if (track.id.isEmpty()) continue;
    if (m_radioQueuedTrackIds.contains(track.id)) continue;
    m_radioQueuedTrackIds.insert(track.id);
    toAdd.append(track);
  }

  if (toAdd.isEmpty()) {
    return 0;
  }

  queue->addTracks(toAdd);
  queue->setSource(m_radioCurrentArtistName, "artistRadio");
  tryAdvanceRadio();
  return toAdd.size();
}

void ArtistController::tryAdvanceRadio() {
  QueueService *queue = m_playbackController->queueService();

  if (!m_radioPlaying) {
    if (queue->count() <= 0) {
      return;
    }
    m_radioPlaying = true;
    m_radioNeedsResume = false;
    if (!queue->setCurrentIndex(0)) {
      m_radioPlaying = false;
      return;
    }
    const Track first = queue->currentTrack();
    if (first.id.isEmpty()) {
      m_radioPlaying = false;
      return;
    }
    m_playbackController->playTrack(first);
    emit statusChanged(QStringLiteral("Радио исполнителя: %1").arg(m_radioCurrentArtistName));
    return;
  }

  if (!m_radioNeedsResume) {
    return;
  }

  m_radioNeedsResume = false;

  if (queue->hasNext()) {
    queue->next();
    const Track next = queue->currentTrack();
    if (!next.id.isEmpty()) {
      m_playbackController->playTrack(next);
    }
    return;
  }

  // Очередь пуста даже после добавления — значит артист не дал треков.
  m_radioNeedsResume = true;
}

void ArtistController::continueIfStalled() {
  if (!m_radioActive) {
    return;
  }

  QueueService *queue = m_playbackController->queueService();

  if (m_radioNeedsResume) {
    if (queue->hasNext()) {
      m_radioNeedsResume = false;
      queue->next();
      const Track next = queue->currentTrack();
      if (!next.id.isEmpty()) {
        m_radioPlaying = true;
        m_playbackController->playTrack(next);
      }
      return;
    }
    continueRadioToNextArtist();
    return;
  }

  // Ещё ни разу не запускались и очередь пуста — текущий артист не дал треков.
  if (!m_radioPlaying && queue->count() == 0) {
    continueRadioToNextArtist();
  }
}

void ArtistController::handleArtistRadioExhausted() {
  if (!m_radioActive) {
    return;
  }

  if (!m_pendingAlbumIds.isEmpty()) {
    // Ещё грузятся альбомы текущего артиста — дождёмся их.
    m_radioNeedsResume = true;
    return;
  }

  m_radioNeedsResume = true;
  continueRadioToNextArtist();
}

void ArtistController::continueRadioToNextArtist() {
  if (!m_radioActive) {
    return;
  }

  if (m_radioVisitedArtists.size() >= MaxRadioArtists) {
    endRadioArtist();
    return;
  }

  if (!m_radioSimilarLoaded) {
    m_radioWaitingForSimilar = true;
    return;
  }

  Artist next;
  for (const Artist &artist : m_radioSimilarPool) {
    if (artist.id.isEmpty()) continue;
    if (m_radioVisitedArtists.contains(artist.id)) continue;
    if (!m_radioCurrentArtistId.isEmpty() && artist.id == m_radioCurrentArtistId) continue;
    next = artist;
    break;
  }

  m_radioSimilarPool.clear();

  if (next.id.isEmpty()) {
    endRadioArtist();
    return;
  }

  m_radioWaitingForSimilar = false;
  beginRadioArtist(next.id, next.name);
}

void ArtistController::endRadioArtist() {
  m_radioActive = false;
  m_radioLoading = false;
  m_radioNeedsResume = false;
  m_radioWaitingForSimilar = false;
  m_pendingAlbumIds.clear();
  emit radioLoadingChanged();
  emit statusChanged("Радио исполнителя завершено");
}

bool ArtistController::radioLoading() const {
  return m_radioLoading;
}

ArtistModel *ArtistController::artistModel() const {
  return m_artistModel;
}

ArtistAlbumsModel *ArtistController::albumsModel() const {
  return m_albumsModel;
}

SimilarArtistsModel *ArtistController::similarArtistsModel() const {
  return m_similarArtistsModel;
}

bool ArtistController::isLoading() const {
  return m_loading;
}

QString ArtistController::artistId() const {
  return m_artistId;
}

QString ArtistController::artistName() const {
  return m_artistName;
}

QString ArtistController::artistCoverUri() const {
  return m_artistCoverUri;
}

QString ArtistController::artistDescription() const {
  return m_artistDescription;
}

QString ArtistController::artistGenres() const {
  return m_artistGenres;
}

QString ArtistController::newReleaseId() const {
  return m_newRelease.id;
}

QString ArtistController::newReleaseTitle() const {
  return m_newRelease.title;
}

QString ArtistController::newReleaseCoverUri() const {
  return m_newRelease.coverUri;
}

int ArtistController::newReleaseYear() const {
  return m_newRelease.year;
}

QString ArtistController::albumFilterType() const {
  return m_albumFilterType;
}

void ArtistController::setAlbumFilterType(const QString &filterType) {
  const QString type = filterType.trimmed();

  if (type != "" && type != "album" && type != "single" && type != "compilation") {
    return;
  }

  if (m_albumFilterType == type) {
    return;
  }

  m_albumFilterType = type;
  emit albumFilterChanged();
  applyAlbumFilter();
}

void ArtistController::applyAlbumFilter() {
  QList<Album> filtered;

  for (const Album &album : m_allAlbums) {
    if (m_albumFilterType.isEmpty() || album.type == m_albumFilterType) {
      filtered.append(album);
    }
  }

  m_albumsModel->setAlbums(filtered);
}