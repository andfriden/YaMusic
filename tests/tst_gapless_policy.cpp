#include "../src/Player/GaplessPolicy.h"

#include <QtTest>

// Окно подготовки бесшовного перехода: готовить следующий трек заранее
// имеет смысл только в последние секунды текущего.
class GaplessPolicyTest : public QObject {
  Q_OBJECT

private slots:
  void invalidDurations();
  void outsideWindow();
  void atWindowBoundary();
  void insideWindow();
  void shortTrackIsImmediate();
};

void GaplessPolicyTest::invalidDurations() {
  QVERIFY(!Playback::isGaplessPrepareDue(0, 0));
  QVERIFY(!Playback::isGaplessPrepareDue(1000, -1));
  QVERIFY(!Playback::isGaplessPrepareDue(-1, 200000));
}

void GaplessPolicyTest::outsideWindow() {
  const qint64 duration = 200000;

  QVERIFY(!Playback::isGaplessPrepareDue(0, duration));
  QVERIFY(!Playback::isGaplessPrepareDue(
      duration - Playback::kGaplessPrepareLeadMs - 1, duration));
}

void GaplessPolicyTest::atWindowBoundary() {
  const qint64 duration = 200000;

  QVERIFY(Playback::isGaplessPrepareDue(
      duration - Playback::kGaplessPrepareLeadMs, duration));
}

void GaplessPolicyTest::insideWindow() {
  const qint64 duration = 200000;

  QVERIFY(Playback::isGaplessPrepareDue(duration - 1000, duration));
  QVERIFY(Playback::isGaplessPrepareDue(duration, duration));
}

void GaplessPolicyTest::shortTrackIsImmediate() {
  const qint64 duration = 5000;

  QVERIFY(Playback::isGaplessPrepareDue(0, duration));
}

QTEST_GUILESS_MAIN(GaplessPolicyTest)

#include "tst_gapless_policy.moc"
