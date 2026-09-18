#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QVariant>

#include "../../Models/Track.h"

class LikedArtistsModel : public QAbstractListModel {
  Q_OBJECT

  Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
  enum Roles { ArtistIdRole = Qt::UserRole + 1, NameRole, CoverUriRole };

  Q_ENUM(Roles)

  explicit LikedArtistsModel(QObject *parent = nullptr);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;
  void setArtists(const QList<Artist> &artists);
  void clear();
  Artist artistAt(int index) const;
  int count() const;

signals:
  void countChanged();

private:
  QList<Artist> m_artists;
};
