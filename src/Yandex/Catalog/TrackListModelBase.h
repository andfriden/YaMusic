#pragma once

#include "../../Models/Track.h"
#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QVariant>
#include <functional>

// Базовый список треков. Хранит два набора: исходный (m_sourceTracks) и
// видимый (m_tracks). Общий фильтр позволяет скрывать треки во всех
// списках сразу, не теряя исходные данные и индексы для серверных
// операций (например, удаления трека из плейлиста).
class TrackListModelBase : public QAbstractListModel {
  Q_OBJECT

  Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
  using TrackFilter = std::function<bool(const Track &)>;

  explicit TrackListModelBase(QObject *parent = nullptr);
  ~TrackListModelBase() override;

  // Фильтр для всех списков треков: true — трек остаётся видимым.
  // Пустой фильтр показывает всё.
  static void setTrackFilter(TrackFilter filter);
  static void refreshTrackFilters();

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  Track trackAt(int index) const;
  QList<Track> tracks() const;
  int count() const;

  // Индекс трека в исходном списке (до фильтрации); -1, если трек скрыт.
  int sourceIndex(int index) const;

  void clear();
  void setTracks(const QList<Track> &tracks);
  bool setTrackLiked(const QString &trackId, bool liked, int likedRole);
  bool removeTrackById(const QString &trackId);

signals:
  void countChanged();

protected:
  // Добавляет треки в конец: в исходный список — все, в видимый —
  // только прошедшие фильтр.
  void appendTracksFiltered(const QList<Track> &tracks);

  bool acceptsTrack(const Track &track) const;

  QList<Track> m_tracks;
  QList<Track> m_sourceTracks;

private:
  void rebuildVisibleTracks();

  static TrackFilter s_filter;
  static QList<TrackListModelBase *> s_instances;
};
