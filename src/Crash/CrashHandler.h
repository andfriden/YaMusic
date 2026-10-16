#pragma once

#include <QString>
#include <QStringList>

// Отчёты о падениях.
//
// Приложение — клиент с сетевыми потоками, ffmpeg и QML: падение в чужом
// коде ничего не оставляет после себя, кроме пустого окна. Поэтому при
// запуске ставятся обработчики сигналов и terminate, а журнал Qt пишется в
// кольцевой буфер: в момент падения всё это уходит в файл отчёта, и при
// следующем запуске окно может предложить посмотреть и отправить его.
//
// Отчёты лежат в каталоге данных приложения и не чистятся вместе с кэшем
// треков.
namespace Crash {

// Кольцевой буфер последних сообщений журнала: падение должно унести с
// собой след того, что происходило до него.
class LogRing {
public:
  explicit LogRing(int capacity = 200);

  void append(const QString &line);
  QStringList lines() const;
  int capacity() const;

private:
  int m_capacity;
  QStringList m_lines;
};

// Шапка отчёта: то, что показывается в списке, не читая отчёт целиком.
struct Header {
  QString reason;      // «SIGSEGV (ошибка сегментации)», «qFatal», «terminate»
  QString timestamp;   // «2026-10-16 14:22:31»
  QString version;     // версия приложения
  QString platform;    // «macOS 15.3 (arm64)»
  QString environment; // Qt, номер процесса, каталог запуска
};

// Шапка для отчёта, который пишется сейчас: время берётся текущее, версия —
// из install().
Header makeHeader(const QString &reason);

// Разбор шапки из текста отчёта. Незаполненные поля остаются пустыми.
Header parseHeader(const QString &text);

// Текст отчёта: шапка, хвост журнала и стек вызовов.
QString composeReport(const Header &header, const QStringList &logTail,
                      const QString &backtrace);

// Каталог отчётов (~/Library/Application Support/YaMusic/crashes и
// аналогичные на других системах).
QString reportsDir();

// Файлы отчётов, новые первыми.
QStringList reportPaths();

QString readReport(const QString &path);

// Перехват падений: сигналы, terminate и журнал Qt. Ставится один раз при
// запуске, после создания приложения (нужен путь к каталогу данных).
void install(const QString &version);

// Запись отчёта из обычного кода: обработчик qFatal и падения на Windows.
// Возвращает путь файла или пустую строку, если записать не удалось.
QString writeReport(const QString &reason, const QString &backtrace = {});

} // namespace Crash
