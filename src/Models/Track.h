#pragma once

#include <QList>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

struct Artist {
  QString id;
  QString name;
  QString coverUri;
};

struct Album {
  QString id;
  QString title;
  QString coverUri;
  int year = 0;

  // Тип релиза из API: "album", "single", "compilation".
  // Пустая строка, если тип не указан.
  QString type;
};

struct Track {
  QString id;
  QString title;
  QString coverUri;
  int durationMs = 0;
  QList<Artist> artists;
  QList<Album> albums;
  bool liked = false;
  bool disliked = false;

  // Права на воспроизведение через Music API. У глав аудиокниг и выпусков
  // подкастов сервер отдаёт available=false, а звук даёт только в Яндекс
  // Книгах — запрос за стримом по такому треку вернёт 403 no-rights.
  bool available = true;
};

// Все исполнители трека в виде списка карт {"id", "name"} для QML.
// Пустые имена пропускаются, порядок сохраняется.
inline QVariantList artistsToVariantList(const QList<Artist> &artists) {
  QVariantList result;
  result.reserve(artists.size());

  for (const Artist &artist : artists) {
    if (artist.name.trimmed().isEmpty()) {
      continue;
    }

    QVariantMap item;
    item.insert(QStringLiteral("id"), artist.id);
    item.insert(QStringLiteral("name"), artist.name);
    result.append(item);
  }

  return result;
}

// Одна строка синхронизированного текста.
struct LyricLine {
  qint64 timestampMs = 0;
  QString text;
};

// Текст песни из /tracks/{id}/supplement.
struct TrackSupplementary {
  QString trackId;
  QString fullText;
  QList<LyricLine> lines;

  bool hasTimedLines() const { return !lines.isEmpty(); }
};