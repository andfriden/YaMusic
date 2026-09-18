#pragma once

#include "../../Models/Track.h"
#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QVariant>

class SearchArtistsModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum Roles { IdRole = Qt::UserRole + 1, NameRole, CoverUriRole };

  explicit SearchArtistsModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  void setArtists(const QList<Artist> &artists);
  void appendArtists(const QList<Artist> &artists);
  void clear();
  Artist artistAt(int index) const;
  int count() const;

private:
  QList<Artist> m_artists;
};