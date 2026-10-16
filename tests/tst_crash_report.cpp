#include "../src/Core/CrashReporter.h"
#include "../src/Crash/CrashHandler.h"
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QtTest>

using Crash::Header;
using Crash::LogRing;
using Crash::composeReport;
using Crash::parseHeader;

namespace {

// Переменная, с которой тест запускает сам себя: в этом режиме он ставит
// перехват, как приложение, и падает. Аргументом это делать нельзя — QTest
// не пропускает незнакомые ключи.
constexpr auto kCrashEnv = "YAMUSIC_CRASH_TEST";

} // namespace

// Отчёт о падении — единственное, что остаётся после падения: от него
// зависит и список в диалоге, и то, что уйдёт в трекер. Поэтому тест держит
// и формат отчёта, и разбор его шапки, и адрес страницы отправки.
class CrashReportTest : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();

  void ringKeepsLastLines();
  void reportCarriesHeaderAndTail();
  void reportWithoutStackSaysSo();
  void parsesHeaderBack();
  void issueUrlCarriesReport();
  void writesReportOnCrash();
  void listsNewestReportFirst();
};

void CrashReportTest::initTestCase() {
  // Отчёты — в тестовый каталог: рабочие отчёты разработчика тест трогать
  // не должен.
  QStandardPaths::setTestModeEnabled(true);

  if (!qEnvironmentVariableIsSet(kCrashEnv))
    return;

  // Дочерний запуск: перехват ставится так же, как в main.cpp, и процесс
  // падает — отчёт должен появиться на диске.
  Crash::install(QStringLiteral("0.0.0-test"));

  volatile int *broken = nullptr;
  *broken = 1;

  QFAIL("падения не случилось");
}

void CrashReportTest::ringKeepsLastLines() {
  LogRing ring(3);

  for (int i = 1; i <= 5; ++i)
    ring.append(QStringLiteral("строка %1").arg(i));

  QCOMPARE(ring.lines().size(), 3);
  QCOMPARE(ring.lines().first(), QStringLiteral("строка 3"));
  QCOMPARE(ring.lines().last(), QStringLiteral("строка 5"));
}

void CrashReportTest::reportCarriesHeaderAndTail() {
  Header header;
  header.reason = QStringLiteral("SIGSEGV (ошибка сегментации)");
  header.timestamp = QStringLiteral("2026-10-16 14:22:31");
  header.version = QStringLiteral("0.14.1");
  header.platform = QStringLiteral("macOS 15.3 (arm64)");
  header.environment = QStringLiteral("Qt 6.11.2 · процесс 4711");

  const QString text = composeReport(
      header,
      {QStringLiteral("[14:22:28.100] debug старт"),
       QStringLiteral("[14:22:31.900] critical поток не поднялся")},
      QStringLiteral("0   libsystem_platform.dylib  0x0000000193e2a1e4 _sigtramp + 56"));

  QVERIFY(text.contains(QStringLiteral("Причина: SIGSEGV (ошибка сегментации)")));
  QVERIFY(text.contains(QStringLiteral("Время: 2026-10-16 14:22:31")));
  QVERIFY(text.contains(QStringLiteral("Версия: 0.14.1")));
  QVERIFY(text.contains(QStringLiteral("Окружение: Qt 6.11.2 · процесс 4711")));
  QVERIFY(text.contains(QStringLiteral("critical поток не поднялся")));
  QVERIFY(text.contains(QStringLiteral("_sigtramp")));
}

void CrashReportTest::reportWithoutStackSaysSo() {
  Header header;
  header.reason = QStringLiteral("qFatal");

  const QString text = composeReport(header, {}, {});

  QVERIFY(text.contains(QStringLiteral("(журнал пуст)")));
  QVERIFY(text.contains(QStringLiteral("(стек недоступен)")));
}

void CrashReportTest::parsesHeaderBack() {
  Header source;
  source.reason = QStringLiteral("SIGABRT (аварийное завершение)");
  source.timestamp = QStringLiteral("2026-10-16 09:05:00");
  source.version = QStringLiteral("0.14.1");
  source.platform = QStringLiteral("macOS 15.3 (arm64)");
  source.environment = QStringLiteral("Qt 6.11.2 · процесс 77");

  // В журнале может лежать строка с тем же началом, что и поле шапки: в
  // шапку она попасть не должна.
  const QString text = composeReport(
      source, {QStringLiteral("Причина: не отсюда"), QStringLiteral("Время: и не отсюда")},
      QStringLiteral("0 stack"));

  const Header parsed = parseHeader(text);

  QCOMPARE(parsed.reason, source.reason);
  QCOMPARE(parsed.timestamp, source.timestamp);
  QCOMPARE(parsed.version, source.version);
  QCOMPARE(parsed.platform, source.platform);
  QCOMPARE(parsed.environment, source.environment);
}

void CrashReportTest::issueUrlCarriesReport() {
  const QString url = CrashReporterDetail::issueUrl(
      QStringLiteral("Падение: SIGSEGV (версия 0.14.1)"), QStringLiteral("Причина: SIGSEGV"));

  QVERIFY(url.startsWith(QStringLiteral("https://github.com/andfriden/YaMusic/issues/new?")));
  QVERIFY(url.contains(QStringLiteral("title=")));
  QVERIFY(url.contains(QStringLiteral("body=")));
  QVERIFY(!url.contains(QLatin1Char(' ')));
}

void CrashReportTest::writesReportOnCrash() {
  QDir reportsDir(Crash::reportsDir());

  reportsDir.removeRecursively();

  QProcess child;

  QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();

  environment.insert(QString::fromLatin1(kCrashEnv), QStringLiteral("1"));
  child.setProcessEnvironment(environment);
  child.start(QCoreApplication::applicationFilePath());

  QVERIFY2(child.waitForFinished(30000), "дочерний процесс не завершился");

  const QStringList reports = Crash::reportPaths();

  QCOMPARE(reports.size(), 1);

  const Header header = parseHeader(Crash::readReport(reports.first()));

  QCOMPARE(header.reason, QStringLiteral("SIGSEGV (ошибка сегментации)"));
  QCOMPARE(header.version, QStringLiteral("0.0.0-test"));
  QVERIFY(Crash::readReport(reports.first()).contains(QStringLiteral("Стек вызовов")));
}

void CrashReportTest::listsNewestReportFirst() {
  Header header;
  header.reason = QStringLiteral("SIGABRT (аварийное завершение)");

  const QString older = Crash::writeReport(header.reason, QStringLiteral("0 stack"));
  const QString newer = Crash::writeReport(QStringLiteral("qFatal"), QStringLiteral("1 stack"));

  QVERIFY(!older.isEmpty());
  QVERIFY(!newer.isEmpty());

  const QStringList reports = Crash::reportPaths();

  QVERIFY(reports.contains(older));
  QVERIFY(reports.contains(newer));

  // Список отсортирован по времени изменения, новые первыми: при запуске
  // показывается именно первый.
  for (int i = 1; i < reports.size(); ++i) {
    QVERIFY(QFileInfo(reports.at(i - 1)).lastModified() >=
            QFileInfo(reports.at(i)).lastModified());
  }
}

QTEST_GUILESS_MAIN(CrashReportTest)

#include "tst_crash_report.moc"
