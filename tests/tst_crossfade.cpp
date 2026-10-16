#include "../src/Player/CrossfadeCurve.h"
#include <QtTest>

using Playback::crossfadeGains;

// Кривая перекрёстного затухания: концы, монотонность и постоянная
// мощность. От неё зависит, не провалится ли громкость в середине
// перехода и не окажется ли уходящий трек слышен после его конца.
class CrossfadeCurveTest : public QObject {
  Q_OBJECT

private slots:
  void startsWithOutgoingOnly();
  void endsWithIncomingOnly();
  void keepsConstantPower();
  void isMonotonic();
  void clampsOutsideRange();
};

void CrossfadeCurveTest::startsWithOutgoingOnly() {
  const Playback::CrossfadeGains gains = crossfadeGains(0.0);

  QCOMPARE(gains.out, 1.0);
  QCOMPARE(gains.in, 0.0);
}

void CrossfadeCurveTest::endsWithIncomingOnly() {
  const Playback::CrossfadeGains gains = crossfadeGains(1.0);

  QVERIFY(qAbs(gains.out) < 1e-9);
  QCOMPARE(gains.in, 1.0);
}

void CrossfadeCurveTest::keepsConstantPower() {
  for (int step = 0; step <= 100; ++step) {
    const Playback::CrossfadeGains gains = crossfadeGains(step / 100.0);
    const double power = gains.out * gains.out + gains.in * gains.in;

    QVERIFY2(qAbs(power - 1.0) < 1e-9, qPrintable(QString::number(power)));
  }

  // На середине оба голоса звучат одинаково громко.
  const Playback::CrossfadeGains middle = crossfadeGains(0.5);

  QVERIFY(qAbs(middle.out - middle.in) < 1e-9);
  QVERIFY(qAbs(middle.out - 0.7071067811865476) < 1e-6);
}

void CrossfadeCurveTest::isMonotonic() {
  for (int step = 1; step <= 100; ++step) {
    const Playback::CrossfadeGains previous = crossfadeGains((step - 1) / 100.0);
    const Playback::CrossfadeGains current = crossfadeGains(step / 100.0);

    QVERIFY(current.out <= previous.out);
    QVERIFY(current.in >= previous.in);
  }
}

void CrossfadeCurveTest::clampsOutsideRange() {
  QCOMPARE(crossfadeGains(-1.0).out, crossfadeGains(0.0).out);
  QCOMPARE(crossfadeGains(-1.0).in, crossfadeGains(0.0).in);
  QCOMPARE(crossfadeGains(2.0).out, crossfadeGains(1.0).out);
  QCOMPARE(crossfadeGains(2.0).in, crossfadeGains(1.0).in);
}

QTEST_GUILESS_MAIN(CrossfadeCurveTest)

#include "tst_crossfade.moc"
