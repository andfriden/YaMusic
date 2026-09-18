#pragma once

#include "../../Models/PersonalPlaylist.h"
#include "../../Models/Playlist.h"
#include "../MemoryCache.h"
#include "../YandexServiceBase.h"
#include <QList>
#include <QObject>
#include <QPair>
#include <QString>

class PlaylistService : public YandexServiceBase {
  Q_OBJECT

public:
  explicit PlaylistService(YandexAuth *auth, QObject *parent = nullptr);

  void loadPlaylist(const QString &uid, int kind);

  void loadSimilarPlaylists(const QString &uuid);

  void loadPlaylists(const QList<QPair<QString, int>> &playlists);

  void loadUserPlaylists(const QString &uid);

  void createPlaylist(const QString &uid, const QString &title);

  void deletePlaylist(const QString &uid, int kind);

  void renamePlaylist(const QString &uid, int kind, const QString &newTitle);

  void addTracksToPlaylist(const QString &uid, int kind, const QStringList &trackIds,
                           const QStringList &albumIds, int revision = 1);

  void removeTrackFromPlaylist(const QString &uid, int kind, int trackIndex, int revision = 1);

signals:

  void playlistReceived(const Playlist &playlist);

  void playlistsReceived(const QList<Playlist> &playlists);

  void similarPlaylistsReceived(const QList<Playlist> &playlists);

  void userPlaylistsReceived(const QList<PersonalPlaylist> &playlists);

  void playlistCreated(const QString &title);

  void playlistDeleted(int kind);

  void playlistRenamed(const QString &newTitle);

  void tracksAdded(int kind, int count);

  void tracksRemoved(int kind, int count);

  void errorOccurred(const QString &message);

private:
  void startNextPlaylistBatchRequests();

  void finishPlaylistBatch();

  QString cacheKeyFor(const QString &uid, int kind) const;

private:
  QList<QPair<QString, int>> m_playlistBatchQueue;

  QList<Playlist> m_playlistBatchResults;

  int m_playlistBatchActive = 0;

  int m_playlistBatchCompleted = 0;

  bool m_playlistBatchError = false;

  // L1-кэш открытого плейлиста. Ключ — "uid:kind".
  // Инвалидируется при любых мутациях (add/remove/rename/delete).

  MemoryCache<Playlist> m_playlistCache;

  static constexpr int MaxConcurrentPlaylistRequests = 5;
};