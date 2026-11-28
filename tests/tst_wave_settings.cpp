#include "../src/Core/WaveSettingsController.h"

#include <QSettings>
#include <QTemporaryDir>
#include <QtTest>

// Настройки «Моей волны»: значения по умолчанию, проверка допустимых
// вариантов, сохранение между экземплярами и тело запроса settings3.
class TestWaveSettings : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void init();

  void defaultsAreNeutral();
  void rejectsUnknownValues();
  void storesSelectionBetweenInstances();
  void toJsonCarriesSelection();
  void markAppliedClearsPending();
  void staleRevisionKeepsPending();

private:
  QTemporaryDir m_dir;
};

void TestWaveSettings::initTestCase() {
  QVERIFY(m_dir.isValid());

  QCoreApplication::setOrganizationName(QStringLiteral("YaMusicTests"));
  QCoreApplication::setApplicationName(QStringLiteral("WaveSettings"));
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, m_dir.path());
}

void TestWaveSettings::init() {
  QSettings settings;
  settings.clear();
  settings.sync();
}

void TestWaveSettings::defaultsAreNeutral() {
  WaveSettingsController controller;

  QCOMPARE(controller.language(), QStringLiteral("any"));
  QCOMPARE(controller.diversity(), QStringLiteral("default"));
  QCOMPARE(controller.moodEnergy(), QStringLiteral("all"));
  QVERIFY(!controller.pendingApply());
}

void TestWaveSettings::rejectsUnknownValues() {
  WaveSettingsController controller;

  controller.setLanguage(QStringLiteral("klingon"));
  controller.setDiversity(QStringLiteral("high"));
  controller.setMoodEnergy(QStringLiteral("extreme"));

  QCOMPARE(controller.language(), QStringLiteral("any"));
  QCOMPARE(controller.diversity(), QStringLiteral("default"));
  QCOMPARE(controller.moodEnergy(), QStringLiteral("all"));
  QVERIFY(!controller.pendingApply());
}

void TestWaveSettings::storesSelectionBetweenInstances() {
  {
    WaveSettingsController controller;
    controller.setLanguage(QStringLiteral("russian"));
    controller.setDiversity(QStringLiteral("favorite"));
    controller.setMoodEnergy(QStringLiteral("calm"));
    QVERIFY(controller.pendingApply());
  }

  WaveSettingsController restored;
  QCOMPARE(restored.language(), QStringLiteral("russian"));
  QCOMPARE(restored.diversity(), QStringLiteral("favorite"));
  QCOMPARE(restored.moodEnergy(), QStringLiteral("calm"));
  // Выбор ещё не отправлялся на сервер, поэтому после перезапуска его
  // нужно применить при первой же волне.
  QVERIFY(restored.pendingApply());
}

void TestWaveSettings::toJsonCarriesSelection() {
  WaveSettingsController controller;
  controller.setLanguage(QStringLiteral("not-russian"));
  controller.setDiversity(QStringLiteral("popular"));
  controller.setMoodEnergy(QStringLiteral("fun"));

  const QJsonObject body = controller.toJson();

  QCOMPARE(body.value(QStringLiteral("language")).toString(),
           QStringLiteral("not-russian"));
  QCOMPARE(body.value(QStringLiteral("diversity")).toString(),
           QStringLiteral("popular"));
  QCOMPARE(body.value(QStringLiteral("moodEnergy")).toString(),
           QStringLiteral("fun"));
  QCOMPARE(body.value(QStringLiteral("type")).toString(), QStringLiteral("rotor"));
}

void TestWaveSettings::markAppliedClearsPending() {
  WaveSettingsController controller;
  controller.setLanguage(QStringLiteral("without-words"));
  QVERIFY(controller.pendingApply());

  controller.markApplied(controller.revision());
  QVERIFY(!controller.pendingApply());

  WaveSettingsController restored;
  QVERIFY(!restored.pendingApply());

  controller.setLanguage(QStringLiteral("any"));
  QVERIFY(controller.pendingApply());
}

void TestWaveSettings::staleRevisionKeepsPending() {
  WaveSettingsController controller;
  controller.setLanguage(QStringLiteral("russian"));
  const int appliedRevision = controller.revision();

  controller.setDiversity(QStringLiteral("favorite"));
  controller.markApplied(appliedRevision);

  // Пока запрос летел, выбор изменился: новая настройка ещё не отправлена.
  QVERIFY(controller.pendingApply());
}

QTEST_MAIN(TestWaveSettings)

#include "tst_wave_settings.moc"
