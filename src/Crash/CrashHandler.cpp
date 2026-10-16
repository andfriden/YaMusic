#include "CrashHandler.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QSysInfo>
#include <QtGlobal>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <execinfo.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace Crash {

LogRing::LogRing(int capacity) : m_capacity(qMax(1, capacity)) {}

void LogRing::append(const QString &line) {
  m_lines.append(line);

  while (m_lines.size() > m_capacity)
    m_lines.removeFirst();
}

QStringList LogRing::lines() const {
  return m_lines;
}

int LogRing::capacity() const {
  return m_capacity;
}

namespace {

// Версия, имя и каталоги: нужны обработчикам сигналов, поэтому хранятся
// статически, а не спрашиваются у приложения в момент падения.
QString g_version = QStringLiteral("0.0.0");
QByteArray g_reportsDir;
LogRing g_logRing(200);
QtMessageHandler g_previousHandler = nullptr;
bool g_installed = false;

QString formatLogLine(QtMsgType type, const QMessageLogContext &context, const QString &message) {
  const char *level = "debug";

  switch (type) {
  case QtDebugMsg:
    level = "debug";
    break;
  case QtInfoMsg:
    level = "info";
    break;
  case QtWarningMsg:
    level = "warning";
    break;
  case QtCriticalMsg:
    level = "critical";
    break;
  case QtFatalMsg:
    level = "fatal";
    break;
  }

  const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz"));
  const QString where = context.category != nullptr && qstrlen(context.category) > 0
                            ? QString::fromLatin1(context.category)
                            : QString();

  return QStringLiteral("[%1] %2 %3%4")
      .arg(stamp, QString::fromLatin1(level), where.isEmpty() ? QString() : where + QStringLiteral(": "),
           message);
}

QString crashReportsDir() {
  const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

  return (base.isEmpty() ? QDir::homePath() + QStringLiteral("/.yamusic") : base) +
         QStringLiteral("/crashes");
}

QString reportFileName() {
  // Миллисекунды в имени — чтобы два отчёта из одной секунды (qFatal и
  // следом сигнал) не затёрли друг друга.
  const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss-zzz"));

  return QStringLiteral("crash-%1-%2.log").arg(stamp).arg(QCoreApplication::applicationPid());
}

QString backtraceText() {
#ifdef Q_OS_WIN
  void *frames[64] = {};
  const USHORT count = CaptureStackBackTrace(0, 64, frames, nullptr);

  QStringList lines;

  for (USHORT i = 0; i < count; ++i)
    lines.append(QStringLiteral("%1  %2").arg(i, 3).arg(QString::number(reinterpret_cast<quintptr>(frames[i]), 16)));

  // Символы на Windows требуют DbgHelp и доступа к pdb — адресов достаточно.
  return lines.join(QLatin1Char('\n'));
#else
  void *frames[64] = {};
  const int count = ::backtrace(frames, 64);

  if (count <= 0)
    return {};

  char **symbols = ::backtrace_symbols(frames, count);

  if (symbols == nullptr)
    return {};

  QStringList lines;

  for (int i = 0; i < count; ++i)
    lines.append(QString::fromLatin1(symbols[i]));

  free(symbols);
  return lines.join(QLatin1Char('\n'));
#endif
}

QString signalName(int signal) {
  switch (signal) {
  case SIGSEGV:
    return QStringLiteral("SIGSEGV (ошибка сегментации)");
  case SIGABRT:
    return QStringLiteral("SIGABRT (аварийное завершение)");
#ifndef Q_OS_WIN
  case SIGBUS:
    return QStringLiteral("SIGBUS (ошибка доступа к памяти)");
#endif
  case SIGILL:
    return QStringLiteral("SIGILL (недопустимая инструкция)");
  case SIGFPE:
    return QStringLiteral("SIGFPE (ошибка вычислений)");
  default:
    return QStringLiteral("сигнал %1").arg(signal);
  }
}

// Отчёт для падения из обычного кода: qFatal, terminate, исключение
// Windows. Пустой стек означает «снять его сейчас». Ошибка записи здесь не
// имеет смысла — приложение всё равно завершается, поэтому путь просто
// возвращается наружу.
QString persistReport(const QString &reason, const QString &stack) {
  const QString text =
      composeReport(makeHeader(reason), g_logRing.lines(),
                    stack.isEmpty() ? backtraceText() : stack);

  const QDir dir(g_reportsDir.isEmpty() ? crashReportsDir() : QString::fromUtf8(g_reportsDir));

  if (!dir.exists() && !dir.mkpath(QStringLiteral(".")))
    return {};

  const QString path = dir.filePath(reportFileName());
  QFile file(path);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    return {};

  file.write(text.toUtf8());
  file.close();

  return path;
}

void writeToStderr(QtMsgType type, const QMessageLogContext &context, const QString &message) {
  const QByteArray line = qFormatLogMessage(type, context, message).toLocal8Bit();

  std::fputs(line.constData(), stderr);
  std::fputc('\n', stderr);
  std::fflush(stderr);
}

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message) {
  g_logRing.append(formatLogLine(type, context, message));

  // qFatal завершает процесс без сигнала: пишем отчёт здесь, пока можно
  // пользоваться обычным кодом. Само сообщение уже лежит в хвосте журнала.
  if (type == QtFatalMsg)
    persistReport(QStringLiteral("qFatal"), {});

  if (g_previousHandler != nullptr) {
    g_previousHandler(type, context, message);
    return;
  }

  // Прежнего обработчика не было: Qt печатал сообщения сам. Повторяем это,
  // иначе вывод в консоль пропал бы вместе с отчётами.
  writeToStderr(type, context, message);
}

void signalHandler(int signal) {
  persistReport(signalName(signal), {});

  // Обработчик по умолчанию и повтор сигнала: система должна получить своё
  // падение и, если настроено, записать core dump.
  ::signal(signal, SIG_DFL);
  ::raise(signal);
}

void terminateHandler() {
  QString reason = QStringLiteral("terminate (необработанное исключение)");

  // Что именно выбросили, видно только из текущего исключения: после
  // terminate восстановить его уже нельзя.
  if (std::current_exception() != nullptr) {
    try {
      std::rethrow_exception(std::current_exception());
    } catch (const std::exception &error) {
      reason = QStringLiteral("terminate: %1").arg(QString::fromUtf8(error.what()));
    } catch (...) {
      reason = QStringLiteral("terminate: неизвестное исключение");
    }
  }

  persistReport(reason, {});

  std::_Exit(EXIT_FAILURE);
}

} // namespace

Header makeHeader(const QString &reason) {
  Header header;

  header.reason = reason;
  header.timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
  header.version = g_version;
  header.platform = QStringLiteral("%1 (%2)")
                        .arg(QSysInfo::prettyProductName(), QSysInfo::currentCpuArchitecture());
  header.environment = QStringLiteral("Qt %1 · процесс %2 · каталог %3")
                           .arg(QString::fromLatin1(qVersion()))
                           .arg(QCoreApplication::applicationPid())
                           .arg(QDir::currentPath());

  return header;
}

Header parseHeader(const QString &text) {
  Header header;

  const QStringList lines = text.split(QLatin1Char('\n'));

  // Разбираем только шапку: дальше идут журнал и стек, и строка оттуда с
  // тем же началом не должна попасть в поле.
  for (const QString &line : lines) {
    const QString trimmed = line.trimmed();

    if (trimmed.startsWith(QStringLiteral("---")))
      break;

    auto valueOf = [&trimmed](const QString &prefix) {
      return trimmed.startsWith(prefix) ? trimmed.mid(prefix.size()).trimmed() : QString();
    };

    if (const QString reason = valueOf(QStringLiteral("Причина:")); !reason.isEmpty())
      header.reason = reason;
    else if (const QString stamp = valueOf(QStringLiteral("Время:")); !stamp.isEmpty())
      header.timestamp = stamp;
    else if (const QString version = valueOf(QStringLiteral("Версия:")); !version.isEmpty())
      header.version = version;
    else if (const QString platform = valueOf(QStringLiteral("Платформа:")); !platform.isEmpty())
      header.platform = platform;
    else if (const QString environment = valueOf(QStringLiteral("Окружение:"));
             !environment.isEmpty())
      header.environment = environment;
  }

  return header;
}

QString composeReport(const Header &header, const QStringList &logTail, const QString &backtrace) {
  QStringList lines;

  lines.append(QStringLiteral("Отчёт о падении YaMusic"));
  lines.append(QString());
  lines.append(QStringLiteral("Причина: %1").arg(header.reason));
  lines.append(QStringLiteral("Время: %1").arg(header.timestamp));
  lines.append(QStringLiteral("Версия: %1").arg(header.version));
  lines.append(QStringLiteral("Платформа: %1").arg(header.platform));

  if (!header.environment.isEmpty())
    lines.append(QStringLiteral("Окружение: %1").arg(header.environment));

  lines.append(QString());
  lines.append(QStringLiteral("--- Последние сообщения журнала ---"));

  if (logTail.isEmpty())
    lines.append(QStringLiteral("(журнал пуст)"));
  else
    lines.append(logTail.join(QLatin1Char('\n')));

  lines.append(QString());
  lines.append(QStringLiteral("--- Стек вызовов ---"));
  lines.append(backtrace.isEmpty() ? QStringLiteral("(стек недоступен)") : backtrace);
  lines.append(QString());

  return lines.join(QLatin1Char('\n'));
}

QString reportsDir() {
  return crashReportsDir();
}

QStringList reportPaths() {
  const QDir dir(reportsDir());

  if (!dir.exists())
    return {};

  const QStringList names =
      dir.entryList({QStringLiteral("crash-*.log")}, QDir::Files, QDir::Time);

  QStringList paths;

  for (const QString &name : names)
    paths.append(dir.filePath(name));

  return paths;
}

QString readReport(const QString &path) {
  QFile file(path);

  if (!file.open(QIODevice::ReadOnly))
    return {};

  return QString::fromUtf8(file.readAll());
}

void install(const QString &version) {
  if (g_installed)
    return;

  g_installed = true;
  g_version = version.isEmpty() ? QStringLiteral("0.0.0") : version;
  g_reportsDir = crashReportsDir().toUtf8();

  QDir().mkpath(QString::fromUtf8(g_reportsDir));

  g_previousHandler = qInstallMessageHandler(messageHandler);
  std::set_terminate(terminateHandler);

#ifdef Q_OS_WIN
  // На Windows сигналы POSIX-модели не ловят нарушение доступа: падение
  // приходит необработанным исключением, и стек снимается здесь.
  SetUnhandledExceptionFilter([](EXCEPTION_POINTERS *) -> LONG {
    persistReport(QStringLiteral("необработанное исключение"), {});

    return EXCEPTION_EXECUTE_HANDLER;
  });
#else
  for (int signal : {SIGSEGV, SIGABRT, SIGBUS, SIGILL, SIGFPE})
    ::signal(signal, signalHandler);
#endif
}

QString writeReport(const QString &reason, const QString &backtrace) {
  return persistReport(reason, backtrace);
}

} // namespace Crash
