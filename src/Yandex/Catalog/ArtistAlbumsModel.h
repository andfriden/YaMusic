#pragma once

#include "../../Models/Track.h"
#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

class ArtistAlbumsModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum Roles { IdRole = Qt::UserRole + 1, TitleRole, CoverUriRole, YearRole, TypeRole };

  explicit ArtistAlbumsModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  QHash<int, QByteArray> roleNames() const override;

  void setAlbums(const QList<Album> &albums);

  void clear();

  Album albumAt(int index) const;

  QList<Album> albums() const;

  int count() const;

private:
  QList<Album> m_albums;
};