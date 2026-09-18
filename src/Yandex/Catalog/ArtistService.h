#pragma once

#include "../../Models/Track.h"
#include "../MemoryCache.h"
#include "../YandexServiceBase.h"
#include <QList>
#include <QString>

struct ArtistDetails {
  QString id;
  QString name;
  QString coverUri;
  QString description;
  QList<QString> genres;
  QList<Track> tracks;
  QList<Album> popularAlbums;
  Album newRelease;
  QList<Artist> similarArtists;
};

class ArtistService : public YandexServiceBase {
  Q_OBJECT

public:
  explicit ArtistService(YandexAuth *auth, QObject *parent = nullptr);
  void loadArtist(const QString &id);
  void loadArtistAlbums(const QString &artistId);

signals:
  void artistReceived(const ArtistDetails &artist);
  void artistAlbumsReceived(const QList<Album> &albums);
  void errorOccurred(const QString &message);

private:
  MemoryCache<ArtistDetails> m_artistCache;
  MemoryCache<QList<Album>> m_albumsCache;
};