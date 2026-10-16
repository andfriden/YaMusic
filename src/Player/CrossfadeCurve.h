#pragma once

#include <QtGlobal>
#include <cmath>

namespace Playback {

// Громкости двух голосов во время перекрёстного затухания: t — доля
// перехода от 0 (начало) до 1 (конец).
struct CrossfadeGains {
  double out = 1.0; // уходящий трек
  double in = 0.0;  // приходящий
};

// Кривая равной мощности: сумма квадратов громкостей постоянна, поэтому
// на слух переход не проваливается в середине, как было бы при линейном
// затухании. На концах значения точные: (1, 0) и (0, 1).
inline CrossfadeGains crossfadeGains(double t) {
  // Половина пи, без M_PI: его нет в стандарте, а Qt определяет его не на
  // всех компиляторах.
  constexpr double halfPi = 1.5707963267948966;

  const double clamped = qBound(0.0, t, 1.0);
  const double angle = clamped * halfPi;

  CrossfadeGains gains;
  gains.out = std::cos(angle);
  gains.in = std::sin(angle);

  return gains;
}

} // namespace Playback
