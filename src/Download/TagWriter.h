#pragma once

#include <QByteArray>
#include <QString>

struct Track;

// Пишет метаданные (теги) и вложенный артворк в аудиофайл.
//
// Делегирует записи TagLib и поддерживает наиболее частые
// контейнеры Яндекс Музыки: MP3 (ID3v2.3), FLAC (Vorbis
// comment + картинка), M4A (iTunes/MP4) и OGG (Vorbis).
//
// Track не включается напрямую через контекст — параметры
// передаются отдельными строками, чтобы не тянуть модели в
// этот слой.
class TagWriter {
public:
  // Возвращает true при успехе. Файл должен существовать и
  // находиться в поддерживаемом формате, иначе — false.
  static bool writeTags(
      const QString &filePath,
      const QString &title,
      const QString &artist,
      const QString &album,
      quint32 trackNumber,
      bool trackCountKnown,
      int durationSeconds,
      const QByteArray &coverData,
      const QString &coverMime);
};