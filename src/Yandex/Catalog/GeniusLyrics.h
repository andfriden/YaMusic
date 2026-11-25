#pragma once

#include <QString>

// Текст песни из HTML страницы Genius: склеивает контейнеры
// data-lyrics-container, превращает <br> в переводы строк, снимает теги
// и декодирует HTML-сущности. Пустая строка — текст не найден.
QString geniusLyricsFromHtml(const QString &html);
