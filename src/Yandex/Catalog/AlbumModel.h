#pragma once

#include "AlbumService.h"
#include "TrackListModelBase.h"
#include <QHash>
#include <QList>
#include <QString>
#include <QVariant>

class AlbumModel final : public TrackListModelBase {
  Q_OBJECT
  Q_PROPERTY(QString title READ title NOTIFY albumChanged)
  Q_PROPERTY(QString coverUri READ coverUri NOTIFY albumChanged)
  Q_PROPERTY(int trackCount READ trackCount NOTIFY albumChanged)

public:
  enum Roles {
    IdRole = Qt::UserRole + 1,
    TitleRole,
    ArtistRole,
    ArtistIdRole,
    AlbumIdRole,
    CoverUriRole,
    DurationMsRole,
    LikedRole,
    // Права на воспроизведение (у глав книг и части подкастов — false).
    AvailableRole,
    // Все исполнители трека: список карт {"id", "name"} для ссылок в строке.
    ArtistsRole
  };
  Q_ENUM(Roles)

  explicit AlbumModel(QObject *parent = nullptr);

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  void setAlbum(const AlbumDetails &album);
  void clear();
  QString title() const;
  QString coverUri() const;
  int trackCount() const;
  void setTrackLiked(const QString &trackId, bool liked);

signals:
  void countChanged();
  void albumChanged();

private:
  AlbumDetails m_album;
};