#pragma once

#include "../../Models/Genre.h"
#include "../YandexServiceBase.h"
#include <QList>
#include <QString>

class GenreService : public YandexServiceBase {
  Q_OBJECT

public:
  explicit GenreService(YandexAuth *auth, QObject *parent = nullptr);

  void loadGenres();

  void loadTagPlaylistIds(const QString &tagId);

signals:

  void genresReceived(const QList<Genre> &genres);

  void tagPlaylistIdsReceived(const QString &tagId, const QList<QPair<QString, int>> &playlists);

  void errorOccurred(const QString &message);
};