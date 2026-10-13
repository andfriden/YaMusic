#include "AlbumController.h"
#include "../Playback/PlaybackController.h"
#include "../Yandex/Catalog/AlbumService.h"
#include "../Yandex/Catalog/ArtistService.h"
#include "../Yandex/Personal/LikesService.h"

AlbumController::AlbumController(AlbumService *albumService, ArtistService *artistService,
                                 PlaybackController *playbackController, LikesService *likesService,
                                 QObject *parent)
    : QObject(parent), m_albumService(albumService), m_artistService(artistService),
      m_playbackController(playbackController), m_likesService(likesService),
      m_albumModel(new AlbumModel(this)), m_otherAlbumsModel(new ArtistAlbumsModel(this)) {
  Q_ASSERT(m_albumService);
  Q_ASSERT(m_artistService);
  Q_ASSERT(m_playbackController);

  connect(m_albumService, &AlbumService::albumReceived, this, [this](const AlbumDetails &album) {
    m_loading = false;
    emit loadingChanged();
    m_albumId = album.album.id;
    m_externalText = album.actionButtonText;
    m_externalUrl = album.actionButtonUrl;
    m_hasPlayableTracks = false;
    for (const Track &track : album.tracks) {
      if (track.available) {
        m_hasPlayableTracks = true;
        break;
      }
    }
    AlbumDetails albumWithLikes = album;

    if (m_likesService) {
      for (Track &track : albumWithLikes.tracks) {
        track.liked = m_likesService->isLiked(track.id);
      }
    }

    m_albumModel->setAlbum(albumWithLikes);
    emit albumChanged();
    loadOtherAlbumsForCurrentArtist(albumWithLikes);
  });

  connect(m_albumService, &AlbumService::errorOccurred, this, [this](const QString &message) {
    m_loading = false;
    emit loadingChanged();
    emit statusChanged(message);
  });

  connect(m_artistService, &ArtistService::artistAlbumsReceived, this,
          [this](const QString &, const QList<Album> &albums) {
            m_otherAlbumsModel->setAlbums(albums);
          });

  if (m_likesService) {
    connect(m_likesService, &LikesService::likeChanged, this,
            [this](const QString &trackId, bool liked) {
              m_albumModel->setTrackLiked(trackId, liked);
            });

    connect(m_likesService, &LikesService::tracksReceived, this,
            [this](const QList<Track> &tracks) {
              for (const Track &track : tracks) {
                m_albumModel->setTrackLiked(track.id, true);
              }
            });
  }
}

void AlbumController::loadAlbum(const QString &id) {
  if (id.isEmpty()) {
    return;
  }

  m_loading = true;
  emit loadingChanged();
  m_albumId = id;
  m_currentArtistId.clear();
  m_externalText.clear();
  m_externalUrl.clear();
  m_hasPlayableTracks = false;
  m_albumModel->clear();
  m_otherAlbumsModel->clear();
  emit albumChanged();
  m_albumService->loadAlbum(id);
}

void AlbumController::selectAlbumTrack(int index) {
  const QList<Track> tracks = m_albumModel->tracks();

  if (index < 0 || index >= tracks.size()) {
    return;
  }

  if (!tracks.at(index).available) {
    emit statusChanged(unavailableMessage());
    return;
  }

  // В очередь кладём только воспроизводимые треки: иначе переход
  // «далее» по недоступному треку снова упрётся в 403 no-rights.
  QList<Track> playable;
  int playIndex = 0;

  for (int i = 0; i < tracks.size(); ++i) {
    const Track &track = tracks.at(i);
    if (!track.available) continue;
    if (i < index) ++playIndex;
    playable.append(track);
  }

  if (playIndex >= playable.size()) {
    emit statusChanged(unavailableMessage());
    return;
  }

  emit trackSelected(tracks.at(index));
  m_playbackController->playFromSource(playable, playIndex, m_albumModel->title(), "album");
}

void AlbumController::playAlbum() {
  const QList<Track> tracks = m_albumModel->tracks();

  QList<Track> playable;
  for (const Track &track : tracks) {
    if (track.available) playable.append(track);
  }

  if (playable.isEmpty()) {
    emit statusChanged(unavailableMessage());
    return;
  }

  m_playbackController->playFromSource(playable, 0, m_albumModel->title(), "album");
}

QString AlbumController::unavailableMessage() const {
  return QStringLiteral("Прослушивание доступно только в Яндекс Книгах");
}

AlbumModel *AlbumController::albumModel() const {
  return m_albumModel;
}

ArtistAlbumsModel *AlbumController::otherAlbumsModel() const {
  return m_otherAlbumsModel;
}

bool AlbumController::isLoading() const {
  return m_loading;
}

QString AlbumController::albumId() const {
  return m_albumId;
}

QString AlbumController::albumTitle() const {
  return m_albumModel ? m_albumModel->title() : QString();
}

QString AlbumController::albumCoverUri() const {
  return m_albumModel ? m_albumModel->coverUri() : QString();
}

int AlbumController::albumTrackCount() const {
  return m_albumModel ? m_albumModel->count() : 0;
}

bool AlbumController::isPlayable() const {
  return m_hasPlayableTracks;
}

QString AlbumController::externalText() const {
  return m_externalText;
}

QString AlbumController::externalUrl() const {
  return m_externalUrl;
}

QString AlbumController::currentArtistId() const {
  return m_currentArtistId;
}

QString AlbumController::currentArtistName() const {
  const QList<Track> tracks = m_albumModel ? m_albumModel->tracks() : QList<Track>();

  if (tracks.isEmpty()) {
    return QString();
  }

  const Track &first = tracks.first();

  if (first.artists.isEmpty()) {
    return QString();
  }

  return first.artists.first().name;
}

QStringList AlbumController::currentAlbumTrackIds() const {
  QStringList ids;

  if (!m_albumModel) {
    return ids;
  }

  for (const Track &track : m_albumModel->tracks()) {
    if (!track.id.isEmpty()) {
      ids.append(track.id);
    }
  }

  return ids;
}

void AlbumController::loadOtherAlbumsForCurrentArtist(const AlbumDetails &album) {
  QString artistId;

  if (!album.tracks.isEmpty()) {
    const Track &firstTrack = album.tracks.first();

    if (!firstTrack.artists.isEmpty()) {
      artistId = firstTrack.artists.first().id;
    }
  }

  if (artistId.isEmpty()) {
    m_otherAlbumsModel->clear();
    return;
  }

  m_currentArtistId = artistId;
  m_otherAlbumsModel->clear();
  m_artistService->loadArtistAlbums(artistId);
}