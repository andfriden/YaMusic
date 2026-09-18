#include "ArtistController.h"
#include "../Playback/PlaybackController.h"
#include "../Queue/QueueService.h"
#include "../Yandex/Catalog/ArtistService.h"

ArtistController::ArtistController(ArtistService *artistService,
                                   PlaybackController *playbackController, QObject *parent)
    : QObject(parent), m_artistService(artistService), m_playbackController(playbackController),
      m_artistModel(new ArtistModel(this)), m_albumsModel(new ArtistAlbumsModel(this)),
      m_similarArtistsModel(new SimilarArtistsModel(this)) {
  Q_ASSERT(m_artistService != nullptr);
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
          [this](const QList<Album> &albums) {
            m_allAlbums = albums;
            applyAlbumFilter();
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
  const QList<Track> tracks = m_artistModel->tracks();

  if (tracks.isEmpty()) {
    emit statusChanged("У исполнителя нет доступных треков");
    return;
  }

  const Track &track = tracks.first();
  emit trackSelected(track);
  m_playbackController->playFromSource(tracks, 0, m_artistName, "artist");
  emit statusChanged(QStringLiteral("Воспроизведение исполнителя: %1").arg(m_artistName));
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