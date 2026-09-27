#pragma once

#include "../Models/Track.h"
#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>

#include "../Yandex/Catalog/ArtistAlbumsModel.h"
#include "../Yandex/Catalog/ArtistModel.h"
#include "../Yandex/Catalog/SimilarArtistsModel.h"

class AlbumService;
class ArtistService;
class PlaybackController;

class ArtistController : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)

  Q_PROPERTY(ArtistModel *artistModel READ artistModel CONSTANT)

  Q_PROPERTY(ArtistAlbumsModel *albumsModel READ albumsModel CONSTANT)

  Q_PROPERTY(SimilarArtistsModel *similarArtistsModel READ similarArtistsModel CONSTANT)

  Q_PROPERTY(QString artistId READ artistId NOTIFY artistChanged)

  Q_PROPERTY(QString artistName READ artistName NOTIFY artistChanged)

  Q_PROPERTY(QString artistCoverUri READ artistCoverUri NOTIFY artistChanged)

  Q_PROPERTY(QString artistDescription READ artistDescription NOTIFY artistChanged)

  Q_PROPERTY(QString artistGenres READ artistGenres NOTIFY artistChanged)

  Q_PROPERTY(QString newReleaseId READ newReleaseId NOTIFY artistChanged)

  Q_PROPERTY(QString newReleaseTitle READ newReleaseTitle NOTIFY artistChanged)

  Q_PROPERTY(QString newReleaseCoverUri READ newReleaseCoverUri NOTIFY artistChanged)

  Q_PROPERTY(int newReleaseYear READ newReleaseYear NOTIFY artistChanged)

  Q_PROPERTY(QString albumFilterType READ albumFilterType NOTIFY albumFilterChanged)

  Q_PROPERTY(bool radioLoading READ radioLoading NOTIFY radioLoadingChanged)

public:
  explicit ArtistController(ArtistService *artistService, AlbumService *albumService,
                            PlaybackController *playbackController, QObject *parent = nullptr);

  void loadArtist(const QString &id);

  Q_INVOKABLE void selectTrack(int index);

  Q_INVOKABLE void selectSimilarArtist(int index);

  Q_INVOKABLE void playArtist();

  // Радио исполнителя: все треки по альбомам артиста, затем цепочка похожих.
  Q_INVOKABLE void startArtistRadio();

  // Продолжение альбома через радио артиста: после конца альбома играет
  // остальные альбомы этого же исполнителя, затем цепочку похожих.
  // playedTrackIds — треки уже воспроизведённого альбома (не повторять).
  void continueFromAlbumRadio(const QString &artistId, const QString &artistName,
                              const QStringList &playedTrackIds);

  // Вызывается из AppController при playlistExhausted источника "artistRadio".
  void handleArtistRadioExhausted();

  // Устанавливает фильтр альбомов: "", "album", "single", "compilation".
  // Пустая строка — показать всё.
  Q_INVOKABLE void setAlbumFilterType(const QString &filterType);

  ArtistModel *artistModel() const;

  ArtistAlbumsModel *albumsModel() const;

  SimilarArtistsModel *similarArtistsModel() const;

  bool isLoading() const;

  bool radioLoading() const;

  QString artistId() const;

  QString artistName() const;

  QString artistCoverUri() const;

  QString artistDescription() const;

  QString artistGenres() const;

  QString newReleaseId() const;

  QString newReleaseTitle() const;

  QString newReleaseCoverUri() const;

  int newReleaseYear() const;

  QString albumFilterType() const;

signals:
  void statusChanged(const QString &message);

  void loadingChanged();

  void artistChanged();

  void trackSelected(const Track &track);

  void similarArtistSelected(const QString &artistId);

  void albumFilterChanged();

  void radioLoadingChanged();

private:
  void applyAlbumFilter();

  void startArtistRadioInternal();

  void beginRadioArtist(const QString &artistId, const QString &artistName);

  void startLoadingRadioAlbums(const QList<Album> &albums);

  void loadRadioAlbum(const QString &albumId);

  int appendRadioAlbumTracks(const QList<Track> &tracks);

  void tryAdvanceRadio();

  void continueIfStalled();

  void continueRadioToNextArtist();

  void endRadioArtist();

  ArtistService *m_artistService = nullptr;

  AlbumService *m_albumService = nullptr;

  PlaybackController *m_playbackController = nullptr;

  ArtistModel *m_artistModel = nullptr;

  ArtistAlbumsModel *m_albumsModel = nullptr;

  SimilarArtistsModel *m_similarArtistsModel = nullptr;

  bool m_loading = false;

  QString m_artistId;
  QString m_artistName;
  QString m_artistCoverUri;
  QString m_artistDescription;
  QString m_artistGenres;

  Album m_newRelease;

  QString m_albumFilterType;

  QList<Album> m_allAlbums;

  // Состояние радио исполнителя.
  bool m_radioActive = false;
  bool m_radioLoading = false;
  bool m_radioPlaying = false;
  bool m_radioNeedsResume = false;
  bool m_radioWaitingForSimilar = false;
  bool m_radioSimilarLoaded = false;
  QString m_radioCurrentArtistId;
  QString m_radioCurrentArtistName;
  int m_radioArtistQueued = 0;
  QList<QString> m_pendingAlbumIds;
  QSet<QString> m_radioVisitedArtists;
  QSet<QString> m_radioQueuedTrackIds;
  QList<Artist> m_radioSimilarPool;

  static constexpr int MaxRadioArtists = 30;
};