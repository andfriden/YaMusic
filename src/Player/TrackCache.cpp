#include "TrackCache.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <algorithm>

namespace {

// Расширение файла кэша по кодеку стрима: оно нужно только для порядка
// на диске, содержимое определяется по байтам.
QString extensionFor(const QString &codec) {
  const QString normalized = codec.trimmed().toLower();

  if (normalized == QLatin1String("mp3")) return QStringLiteral("mp3");
  if (normalized == QLatin1String("flac")) return QStringLiteral("flac");
  if (normalized == QLatin1String("opus")) return QStringLiteral("opus");
  if (normalized == QLatin1String("aac") || normalized == QLatin1String("he-aac")
      || normalized == QLatin1String("m4a")) {
    return QStringLiteral("m4a");
  }

  return QStringLiteral("bin");
}

// Рядом с каждым файлом кэша лежит его описание: кодек и битрейт.
QString metaPathFor(const QString &directory, const QString &identity) {
  return directory + QLatin1Char('/') + identity + QStringLiteral(".meta");
}

} // namespace

TrackCache::TrackCache()
    : m_directory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                  + QStringLiteral("/streams")) {}

void TrackCache::setDirectory(const QString &path) {
  m_directory = path;
}

QString TrackCache::directory() const {
  return m_directory;
}

void TrackCache::setLimitBytes(qint64 bytes) {
  m_limitBytes = qMax<qint64>(0, bytes);
}

qint64 TrackCache::limitBytes() const {
  return m_limitBytes;
}

QString TrackCache::identityFor(const QString &trackId) {
  return trackId.trimmed();
}

QString TrackCache::pathFor(const QString &identity) const {
  const QString wanted = identity.trimmed();

  if (wanted.isEmpty()) return {};

  for (const QFileInfo &info : files()) {
    if (info.completeBaseName() == wanted) return info.absoluteFilePath();
  }

  return {};
}

bool TrackCache::contains(const QString &identity) const {
  return !pathFor(identity).isEmpty();
}

void TrackCache::touch(const QString &identity) const {
  const QString path = pathFor(identity);

  if (path.isEmpty()) return;

  // Время изменения файла — это и есть отметка «когда играли»: вытеснение
  // смотрит на неё. Если обновить не удалось, файл просто считается старее.
  QFile file(path);

  if (!file.open(QIODevice::ReadWrite)) return;

  file.setFileTime(QDateTime::currentDateTime(), QFileDevice::FileModificationTime);
  file.close();
}

QString TrackCache::writePath(const QString &identity, const QString &codec) const {
  QDir().mkpath(m_directory);

  // Докачка идёт по одному треку, поэтому любой другой .part — мусор от
  // прошлого запуска или обрыва.
  QDir dir(m_directory);

  for (const QFileInfo &info : dir.entryInfoList({QStringLiteral("*.part")}, QDir::Files)) {
    QFile::remove(info.absoluteFilePath());
  }

  return m_directory + QLatin1Char('/') + identity + QLatin1Char('.') + extensionFor(codec)
      + QStringLiteral(".part");
}

bool TrackCache::finishWrite(const QString &writePath, const QString &identity, const QString &codec,
                             QString *error) {
  const QFileInfo info(writePath);

  if (!info.exists() || info.size() <= 0) {
    QFile::remove(writePath);

    if (error != nullptr) *error = QStringLiteral("Скачанный файл пуст");
    return false;
  }

  const QString target = m_directory + QLatin1Char('/') + identity + QLatin1Char('.')
      + extensionFor(codec);

  QFile::remove(target);

  if (!QFile::rename(writePath, target)) {
    QFile::remove(writePath);

    if (error != nullptr) *error = QStringLiteral("Не удалось переименовать скачанный файл");
    return false;
  }

  return true;
}

void TrackCache::remove(const QString &identity) {
  const QString path = pathFor(identity);

  if (!path.isEmpty()) QFile::remove(path);

  QFile::remove(metaPathFor(m_directory, identity));
}

void TrackCache::saveMeta(const QString &identity, const QString &codec, int bitrateKbps) const {
  if (identity.trimmed().isEmpty()) return;

  QDir().mkpath(m_directory);

  QFile file(metaPathFor(m_directory, identity));

  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return;

  file.write(codec.trimmed().toUtf8());
  file.write("\n");
  file.write(QByteArray::number(bitrateKbps));
  file.close();
}

void TrackCache::loadMeta(const QString &identity, QString *codec, int *bitrateKbps) const {
  if (codec != nullptr) codec->clear();
  if (bitrateKbps != nullptr) *bitrateKbps = 0;

  QFile file(metaPathFor(m_directory, identity));

  if (!file.open(QIODevice::ReadOnly)) return;

  const QList<QByteArray> lines = file.readAll().split('\n');
  file.close();

  if (codec != nullptr && !lines.isEmpty()) *codec = QString::fromUtf8(lines.at(0)).trimmed();

  if (bitrateKbps != nullptr && lines.size() > 1) *bitrateKbps = lines.at(1).trimmed().toInt();
}

void TrackCache::clear() {
  QDir dir(m_directory);

  if (!dir.exists()) return;

  for (const QFileInfo &info : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
    QFile::remove(info.absoluteFilePath());
  }
}

qint64 TrackCache::totalBytes() const {
  qint64 total = 0;

  for (const QFileInfo &info : files())
    total += info.size();

  return total;
}

void TrackCache::evictIfNeeded(const QString &keepIdentity) {
  QList<QFileInfo> cached = files();
  qint64 total = 0;

  for (const QFileInfo &info : cached)
    total += info.size();

  if (total <= m_limitBytes) return;

  std::sort(cached.begin(), cached.end(), [](const QFileInfo &left, const QFileInfo &right) {
    return left.lastModified() < right.lastModified();
  });

  for (const QFileInfo &info : cached) {
    if (total <= m_limitBytes) break;
    if (!keepIdentity.isEmpty() && info.completeBaseName() == keepIdentity) continue;

    const qint64 size = info.size();

    if (QFile::remove(info.absoluteFilePath())) {
      total -= size;
      QFile::remove(metaPathFor(m_directory, info.completeBaseName()));
    }
  }
}

QList<QFileInfo> TrackCache::files() const {
  QList<QFileInfo> result;
  QDir dir(m_directory);

  if (!dir.exists()) return result;

  const QFileInfoList entries =
      dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

  for (const QFileInfo &info : entries) {
    // .part — незавершённая докачка, .meta — описание рядом с файлом:
    // ни то, ни другое не аудио и в подсчёте кэша не участвует.
    if (info.suffix() == QLatin1String("part")) continue;
    if (info.suffix() == QLatin1String("meta")) continue;
    result.append(info);
  }

  return result;
}
