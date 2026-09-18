#pragma once

#include "Track.h"
#include <QList>
#include <QString>

struct Playlist {
  QString uid;
  int kind = 0;
  QString uuid;
  QString title;
  QString description;
  QString coverUri;
  int trackCount = 0;
  int revision = 0;
  QList<Track> tracks;
};