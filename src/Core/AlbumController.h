#pragma once

#include "../Models/Track.h"
#include "../Yandex/Catalog/AlbumModel.h"
#include "../Yandex/Catalog/ArtistAlbumsModel.h"
#include <QObject>
#include <QString>
#include <QStringList>

class AlbumService;
class ArtistService;
class PlaybackController;
class LikesService;

class AlbumController final : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)

  Q_PROPERTY(AlbumModel *albumModel READ albumModel CONSTANT)

  Q_PROPERTY(ArtistAlbumsModel *otherAlbumsModel READ otherAlbumsModel CONSTANT)

  Q_PROPERTY(QString albumId READ albumId NOTIFY albumChanged)

  Q_PROPERTY(QString currentAlbumTitle READ albumTitle NOTIFY albumChanged)

  Q_PROPERTY(QString currentAlbumCoverUri READ albumCoverUri NOTIFY albumChanged)

  Q_PROPERTY(int currentAlbumTrackCount READ albumTrackCount NOTIFY albumChanged)

  // Есть ли в альбоме треки, доступные для воспроизведения через Music API:
  // у книг и части подкастов Яндекс отдаёт звук только во внешнем сервисе.
  Q_PROPERTY(bool currentAlbumPlayable READ isPlayable NOTIFY albumChanged)

  // Кнопка перехода во внешний сервис (у книг — «Читать в Яндекс Книгах»).
  Q_PROPERTY(QString currentAlbumExternalText READ externalText NOTIFY albumChanged)

  Q_PROPERTY(QString currentAlbumExternalUrl READ externalUrl NOTIFY albumChanged)

public:
  explicit AlbumController(AlbumService *albumService, ArtistService *artistService,
                           PlaybackController *playbackController, LikesService *likesService,
                           QObject *parent = nullptr);

  void loadAlbum(const QString &id);

  Q_INVOKABLE void selectAlbumTrack(int index);

  Q_INVOKABLE void playAlbum();

  AlbumModel *albumModel() const;

  ArtistAlbumsModel *otherAlbumsModel() const;

  bool isLoading() const;

  QString albumId() const;

  QString albumTitle() const;

  QString albumCoverUri() const;

  int albumTrackCount() const;

  bool isPlayable() const;

  QString externalText() const;

  QString externalUrl() const;

  // Исполнитель текущего альбома (заполняется при загрузке «Других альбомов»).
  QString currentArtistId() const;

  // Имя исполнителя текущего альбома (из первого трека).
  QString currentArtistName() const;

  // ID треков текущего альбома (уже воспроизведённых).
  QStringList currentAlbumTrackIds() const;

signals:

  void loadingChanged();

  void albumChanged();

  void statusChanged(const QString &message);

  void trackSelected(const Track &track);

private:
  void loadOtherAlbumsForCurrentArtist(const AlbumDetails &album);

  // Сообщение для трека, который нельзя воспроизвести в Music API.
  QString unavailableMessage() const;

  AlbumService *m_albumService = nullptr;

  ArtistService *m_artistService = nullptr;

  PlaybackController *m_playbackController = nullptr;

  LikesService *m_likesService = nullptr;

  AlbumModel *m_albumModel = nullptr;

  ArtistAlbumsModel *m_otherAlbumsModel = nullptr;

  bool m_loading = false;

  QString m_albumId;

  QString m_currentArtistId;

  bool m_hasPlayableTracks = false;

  QString m_externalText;

  QString m_externalUrl;
};
