#pragma once

#include <QString>

// Разобранная ссылка на плейлист Яндекс Музыки.
//
// Понимает два вида ссылок:
//   https://music.yandex.ru/users/<login>/playlists/<kind>
//   https://music.yandex.ru/playlists/<uuid>
// Схема, query-параметры и завершающий слэш не важны; хост должен быть
// яндексовым (yandex.ru, yandex.com и зеркала).
struct PlaylistLink {
  enum class Type { Invalid, UserPlaylist, Uuid };

  Type type = Type::Invalid;
  QString login; // для UserPlaylist
  int kind = 0;  // для UserPlaylist
  QString uuid;  // для Uuid

  static PlaylistLink parse(const QString &text);
};
