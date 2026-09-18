#pragma once

#include "TrackListModelBase.h"

class GenreStationModel : public TrackListModelBase {
  Q_OBJECT

public:
  enum Roles {
    IdRole = Qt::UserRole + 1,
    TitleRole,
    ArtistRole,
    ArtistIdRole,
    AlbumRole,
    AlbumIdRole,
    CoverUriRole,
    DurationMsRole
  };

  explicit GenreStationModel(QObject *parent = nullptr);
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;
  void appendTracks(const QList<Track> &tracks);
  QString lastTrackId() const;
};