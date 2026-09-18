#pragma once

#include "../../Models/Track.h"
#include "../MemoryCache.h"
#include "../YandexServiceBase.h"
#include <QList>
#include <QObject>
#include <QString>

struct AlbumDetails {
  Album album;

  QString description;

  int trackCount = 0;

  QList<Track> tracks;
};

class AlbumService : public YandexServiceBase {
  Q_OBJECT

public:
  explicit AlbumService(YandexAuth *auth, QObject *parent = nullptr);

  void loadAlbum(const QString &id);

signals:
  void albumReceived(const AlbumDetails &album);

  void errorOccurred(const QString &message);

private:
  MemoryCache<AlbumDetails> m_cache;
};