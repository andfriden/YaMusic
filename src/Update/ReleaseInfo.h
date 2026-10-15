#pragma once

#include <QDateTime>
#include <QList>
#include <QString>
#include <QUrl>

namespace Update {

// Файл релиза на GitHub (DMG, AppImage, портативная сборка Windows).
struct ReleaseAsset {
  QString name;
  QUrl url;
  qint64 size = 0;
};

// Релиз целиком: версия, описание и приложенные файлы.
struct ReleaseInfo {
  QString tag;     // "v0.14.0"
  QString version; // "0.14.0" — то, чем сравниваем
  QString title;
  QString notes;
  QDateTime publishedAt;
  QUrl pageUrl;
  QList<ReleaseAsset> assets;

  bool isValid() const {
    return !version.isEmpty();
  }
};

// Числа версии: "v0.14.0" → {0, 14, 0}. Префикс тега и суффикс
// отбрасываются, поэтому "0.14.0-rc1" и "0.14.0" считаются одной версией.
inline QList<int> versionParts(const QString &value) {
  QString trimmed = value.trimmed();

  // Ведущий "v" (или другой префикс тега) в сравнении не участвует.
  while (!trimmed.isEmpty() && !trimmed.at(0).isDigit())
    trimmed.remove(0, 1);

  QList<int> parts;

  for (const QString &chunk : trimmed.split(QLatin1Char('.'), Qt::SkipEmptyParts)) {
    QString digits;

    for (const QChar character : chunk) {
      if (!character.isDigit()) break;
      digits.append(character);
    }

    if (digits.isEmpty()) break;
    parts.append(digits.toInt());
  }

  return parts;
}

// Старше ли version, чем current. Недостающие числа считаются нулями:
// "0.15" старше "0.14.2".
inline bool isNewerVersion(const QString &version, const QString &current) {
  const QList<int> candidate = versionParts(version);
  const QList<int> installed = versionParts(current);

  if (candidate.isEmpty()) return false;

  const int count = qMax(candidate.size(), installed.size());

  for (int i = 0; i < count; ++i) {
    const int left = i < candidate.size() ? candidate.at(i) : 0;
    const int right = i < installed.size() ? installed.at(i) : 0;

    if (left != right) return left > right;
  }

  return false;
}

} // namespace Update
