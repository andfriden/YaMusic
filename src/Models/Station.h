#pragma once

#include <QString>

// Станция ротора: только описание (id {type, tag} + название + иконка), без треков.
// Треки возвращает сессия ротора (POST /rotor/session/new, /rotor/session/<id>/tracks).
struct Station {
  QString type;            // genre / mood / language / decade / user / ...
  QString tag;             // id станции внутри типа (например "rock")
  QString name;            // отображаемое имя ("Rock", "Счастливое")
  QString imageUrl;        // картинка станции
  QString backgroundColor; // цвет-подложка иконки
};