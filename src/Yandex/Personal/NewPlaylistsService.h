#pragma once

#include "../../Models/Playlist.h"
#include "../YandexServiceBase.h"
#include <QList>
#include <QString>

class PlaylistService;

class NewPlaylistsService : public YandexServiceBase {
  Q_OBJECT

public:
  explicit NewPlaylistsService(YandexAuth *auth, PlaylistService *playlistService,
                               QObject *parent = nullptr);

  void load();

signals:

  void playlistsReceived(const QList<Playlist> &playlists);

  void errorOccurred(const QString &message);

private:
  PlaylistService *m_playlistService = nullptr;
};