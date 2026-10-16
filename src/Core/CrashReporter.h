#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

// Отчёты о падениях для интерфейса: список сохранённых отчётов, текст
// выбранного, отправка и подсказка при запуске после падения.
//
// Отправка — это страница нового issue в трекере проекта с уже заполненным
// текстом: своего сервера у приложения нет, а переносить стек руками
// пользователь не должен. Длинный отчёт целиком уходит в буфер обмена —
// в адресной строке браузера он не поместится.
class CrashReporter : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool hasReports READ hasReports NOTIFY reportsChanged)
  Q_PROPERTY(int reportCount READ reportCount NOTIFY reportsChanged)
  Q_PROPERTY(QString reportsPath READ reportsPath NOTIFY reportsChanged)
  Q_PROPERTY(QStringList reportTitles READ reportTitles NOTIFY reportsChanged)

  Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY selectionChanged)
  Q_PROPERTY(QString selectedTitle READ selectedTitle NOTIFY selectionChanged)
  Q_PROPERTY(QString selectedText READ selectedText NOTIFY selectionChanged)

  // Отчёт, оставленный прошлым запуском и ещё не виденный: окно само
  // предлагает его посмотреть.
  Q_PROPERTY(bool startupCrash READ hasStartupCrash NOTIFY reportsChanged)
  Q_PROPERTY(QString startupSummary READ startupSummary NOTIFY reportsChanged)

  Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
  explicit CrashReporter(QObject *parent = nullptr);

  bool hasReports() const;
  int reportCount() const;
  QString reportsPath() const;
  QStringList reportTitles() const;

  int selectedIndex() const;
  QString selectedTitle() const;
  QString selectedText() const;

  bool hasStartupCrash() const;
  QString startupSummary() const;
  QString status() const;

  Q_INVOKABLE void selectReport(int index);
  Q_INVOKABLE void copySelected();
  Q_INVOKABLE void openReportsFolder();
  Q_INVOKABLE void sendSelectedToTracker();
  Q_INVOKABLE void removeSelected();
  Q_INVOKABLE void clearReports();

  // Диалог закрыт: отчёты увидены, при следующем запуске о них не напоминаем.
  Q_INVOKABLE void acknowledgeReports();

signals:
  void reportsChanged();
  void selectionChanged();
  void statusChanged();

private:
  void refresh();
  void setStatus(const QString &status);
  QStringList unseenReports() const;
  void markSeen();
  QString issueTitle() const;
  QString issueBody() const;

  QStringList m_reports;
  int m_selectedIndex = -1;
  QString m_status;
};

namespace CrashReporterDetail {

// Сколько символов отчёта уходит в адресную строку: остальное браузер
// обрежет, поэтому длинное тело отправляется через буфер обмена.
constexpr int kMaxUrlBodyChars = 4000;

// Адрес трекера проекта: страница нового issue.
QString issueUrl(const QString &title, const QString &body);

} // namespace CrashReporterDetail
