#pragma once

#include "SearchResult.h"
#include "TrackListModelBase.h"
#include <QHash>
#include <QVariant>

class SearchModel final : public TrackListModelBase {
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

  explicit SearchModel(QObject *parent = nullptr);

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  void setResults(const SearchResults &results);

  // Дописывает треки из новой страницы результатов
  // к уже загруженным (для пагинации).
  void appendResults(const QList<Track> &tracks);
};