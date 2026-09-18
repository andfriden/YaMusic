#pragma once

#include "../../Models/Track.h"
#include "TrackListModelBase.h"
#include <QHash>
#include <QString>
#include <QVariant>

class ChartModel final : public TrackListModelBase {
  Q_OBJECT

public:
  enum Roles {
    IdRole = Qt::UserRole + 1,
    TitleRole,
    ArtistRole,
    ArtistIdRole,
    CoverUriRole,
    DurationMsRole,
    PositionRole
  };
  Q_ENUM(Roles)

  explicit ChartModel(QObject *parent = nullptr);

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  void setTracks(const QList<Track> &tracks);

private:
};