#include "CrashReporter.h"

#include "../Crash/CrashHandler.h"

#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QSettings>
#include <QUrl>
#include <QUrlQuery>

namespace {

// Отчёты, о которых пользователю уже сказали: иначе окно напоминало бы о
// них при каждом запуске.
constexpr auto kSeenReportsKey = "crash/seenReports";

constexpr auto kRepositoryUrl = "https://github.com/andfriden/YaMusic";

} // namespace

namespace CrashReporterDetail {

QString issueUrl(const QString &title, const QString &body) {
  QUrl url(QStringLiteral("%1/issues/new").arg(QString::fromLatin1(kRepositoryUrl)));

  QUrlQuery query;
  query.addQueryItem(QStringLiteral("title"), title);
  query.addQueryItem(QStringLiteral("body"), body.mid(0, kMaxUrlBodyChars));
  url.setQuery(query);

  // Именно закодированный вид: в адресе со пробелами и кириллицей браузер
  // иначе обрежет запрос.
  return url.toString(QUrl::FullyEncoded);
}

} // namespace CrashReporterDetail

CrashReporter::CrashReporter(QObject *parent) : QObject(parent) {
  refresh();
}

void CrashReporter::refresh() {
  m_reports = Crash::reportPaths();

  const int previous = m_selectedIndex;

  m_selectedIndex = m_reports.isEmpty() ? -1 : qBound(0, previous, static_cast<int>(m_reports.size()) - 1);

  emit reportsChanged();

  if (m_selectedIndex != previous)
    emit selectionChanged();
}

bool CrashReporter::hasReports() const {
  return !m_reports.isEmpty();
}

int CrashReporter::reportCount() const {
  return static_cast<int>(m_reports.size());
}

QString CrashReporter::reportsPath() const {
  return Crash::reportsDir();
}

QStringList CrashReporter::reportTitles() const {
  QStringList titles;

  for (const QString &path : m_reports) {
    const Crash::Header header = Crash::parseHeader(Crash::readReport(path));
    const QString when = QFileInfo(path).lastModified().toString(QStringLiteral("dd.MM HH:mm"));

    titles.append(QStringLiteral("%1 · %2 · %3")
                      .arg(when, header.reason.isEmpty() ? QStringLiteral("отчёт") : header.reason,
                           header.version));
  }

  return titles;
}

int CrashReporter::selectedIndex() const {
  return m_selectedIndex;
}

QString CrashReporter::selectedTitle() const {
  const QStringList titles = reportTitles();

  if (m_selectedIndex < 0 || m_selectedIndex >= titles.size())
    return {};

  return titles.at(m_selectedIndex);
}

QString CrashReporter::selectedText() const {
  if (m_selectedIndex < 0 || m_selectedIndex >= m_reports.size())
    return {};

  return Crash::readReport(m_reports.at(m_selectedIndex));
}

QString CrashReporter::status() const {
  return m_status;
}

bool CrashReporter::hasStartupCrash() const {
  return !unseenReports().isEmpty();
}

QString CrashReporter::startupSummary() const {
  const QStringList unseen = unseenReports();

  if (unseen.isEmpty())
    return {};

  const Crash::Header header = Crash::parseHeader(Crash::readReport(unseen.first()));

  return QStringLiteral("Прошлый запуск завершился падением: %1, %2")
      .arg(header.reason, header.timestamp);
}

void CrashReporter::selectReport(int index) {
  if (index == m_selectedIndex)
    return;

  m_selectedIndex = index;
  emit selectionChanged();
}

void CrashReporter::copySelected() {
  const QString text = selectedText();

  if (text.isEmpty()) {
    setStatus(QStringLiteral("Отчёт не читается"));
    return;
  }

  QGuiApplication::clipboard()->setText(text);
  setStatus(QStringLiteral("Отчёт скопирован в буфер обмена"));
}

void CrashReporter::openReportsFolder() {
  const QString path = reportsPath();

  QDir().mkpath(path);

  if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
    setStatus(QStringLiteral("Не удалось открыть папку: %1").arg(path));
}

void CrashReporter::sendSelectedToTracker() {
  const QString text = selectedText();

  if (text.isEmpty()) {
    setStatus(QStringLiteral("Отчёт не читается"));
    return;
  }

  // Полный отчёт — в буфер обмена: в адрес трекера он не поместится, а
  // вставлять стек руками пользователь не должен.
  QGuiApplication::clipboard()->setText(text);

  if (!QDesktopServices::openUrl(
          QUrl(CrashReporterDetail::issueUrl(issueTitle(), issueBody())))) {
    setStatus(QStringLiteral("Не удалось открыть трекер проекта"));
    return;
  }

  setStatus(QStringLiteral("Полный отчёт скопирован — вставьте его в описание issue"));
}

void CrashReporter::removeSelected() {
  if (m_selectedIndex < 0 || m_selectedIndex >= m_reports.size())
    return;

  if (!QFile::remove(m_reports.at(m_selectedIndex))) {
    setStatus(QStringLiteral("Не удалось удалить отчёт"));
    return;
  }

  refresh();
  setStatus(QStringLiteral("Отчёт удалён"));
}

void CrashReporter::clearReports() {
  int removed = 0;

  for (const QString &path : std::as_const(m_reports)) {
    if (QFile::remove(path))
      ++removed;
  }

  m_selectedIndex = -1;

  refresh();
  setStatus(QStringLiteral("Удалено отчётов: %1").arg(removed));
}

void CrashReporter::acknowledgeReports() {
  markSeen();

  emit reportsChanged();
}

void CrashReporter::setStatus(const QString &status) {
  m_status = status;

  emit statusChanged();
}

QStringList CrashReporter::unseenReports() const {
  QSettings settings;

  const QStringList seen = settings.value(kSeenReportsKey).toStringList();
  QStringList unseen;

  for (const QString &path : m_reports) {
    if (!seen.contains(QFileInfo(path).fileName()))
      unseen.append(path);
  }

  return unseen;
}

void CrashReporter::markSeen() {
  QSettings settings;

  QStringList seen = settings.value(kSeenReportsKey).toStringList();

  for (const QString &path : std::as_const(m_reports)) {
    const QString name = QFileInfo(path).fileName();

    if (!seen.contains(name))
      seen.append(name);
  }

  settings.setValue(kSeenReportsKey, seen);
}

QString CrashReporter::issueTitle() const {
  const Crash::Header header = Crash::parseHeader(selectedText());

  return QStringLiteral("Падение: %1 (версия %2)")
      .arg(header.reason.isEmpty() ? QStringLiteral("неизвестная причина") : header.reason,
           header.version);
}

QString CrashReporter::issueBody() const {
  const QString text = selectedText();

  if (text.size() <= CrashReporterDetail::kMaxUrlBodyChars)
    return text;

  return QStringLiteral("%1\n\n(отчёт обрезан — полный текст в буфере обмена)")
      .arg(text.left(CrashReporterDetail::kMaxUrlBodyChars));
}
