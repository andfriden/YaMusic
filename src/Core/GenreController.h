#pragma once

#include "../Models/Playlist.h"
#include "../Yandex/Catalog/GenreModel.h"
#include "../Yandex/Catalog/GenreStationModel.h"
#include <QList>
#include <QObject>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QVariantList>

class GenreService;
class PlaybackController;
class PlaylistService;
class StationService;

class GenreController : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

  Q_PROPERTY(GenreModel *model READ model CONSTANT)

  Q_PROPERTY(bool genreLoading READ genreLoading NOTIFY genreLoadingChanged)

  Q_PROPERTY(QVariantList genrePlaylists READ genrePlaylists NOTIFY genreContentChanged)

  Q_PROPERTY(GenreStationModel *stationModel READ stationModel CONSTANT)

  Q_PROPERTY(bool stationLoading READ stationLoading NOTIFY stationLoadingChanged)

public:
  explicit GenreController(GenreService *genreService, PlaylistService *playlistService,
                           StationService *stationService, PlaybackController *playbackController,
                           QObject *parent = nullptr);

  Q_INVOKABLE void loadGenres();

  Q_INVOKABLE void loadGenre(const QString &genreId);

  Q_INVOKABLE void loadTagPlaylists(const QString &tagId);

  Q_INVOKABLE void loadGenreStation(const QString &genreId);

  Q_INVOKABLE void loadMoreGenreStation();

  Q_INVOKABLE void selectStationTrack(int index);

  bool loading() const;

  GenreModel *model() const;

  bool genreLoading() const;

  QVariantList genrePlaylists() const;

  GenreStationModel *stationModel() const;

  bool stationLoading() const;

signals:

  void loadingChanged();

  void genreLoadingChanged();

  void genreContentChanged();

  void stationLoadingChanged();

  void stationTrackSelected(int index);

  void statusChanged(const QString &status);

  void errorOccurred(const QString &message);

private:
  void startGenrePlaylistQueue(const QList<QPair<QString, int>> &playlists);

  void loadNextGenrePlaylistBatch();

  void finishGenrePlaylistLoading();

  void startApiFallback();

  QList<QPair<QString, int>> loadPlaylistIdsFromCsv(const QString &genreId);

  static QStringList splitCsvLine(const QString &line);

  static bool isDisplayedGenre(const QString &genreId);

  static QString playlistKey(const QPair<QString, int> &playlist);

private:
  GenreService *m_genreService = nullptr;

  PlaylistService *m_playlistService = nullptr;

  StationService *m_stationService = nullptr;

  PlaybackController *m_playbackController = nullptr;

  GenreModel *m_model = nullptr;

  GenreStationModel *m_stationModel = nullptr;

  QString m_stationGenreId;

  QString m_stationBatchId;

  QList<Playlist> m_genrePlaylists;

  QString m_loadingGenreId;

  bool m_loading = false;

  bool m_genreLoading = false;

  bool m_stationLoading = false;

  bool m_waitingForPlaylists = false;

  bool m_waitingForTagPlaylistIds = false;

  QList<QPair<QString, int>> m_genrePlaylistQueue;

  int m_genrePlaylistQueuePosition = 0;

  bool m_usingApiFallback = false;

  bool m_csvHadCandidates = false;

  bool m_loadedAnyGenrePlaylist = false;

  static constexpr int GenrePlaylistBatchSize = 12;
};