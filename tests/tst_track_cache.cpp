#include "../src/Player/TrackCache.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTest>

// Кэш стримов: имена файлов, вытеснение по давности и служебные файлы
// рядом с аудио.
class TrackCacheTest : public QObject {
  Q_OBJECT

private slots:
  void writeAndReadBack();
  void ignoresTemporaryFiles();
  void evictsOldestFirst();
  void keepsPlayingTrack();
  void removeDeletesFileAndMeta();

private:
  static QString makeFile(TrackCache &cache, const QString &trackId, const QString &codec,
                          const QByteArray &content);

  static void ageFile(const QString &path, int minutesAgo);
};

QString TrackCacheTest::makeFile(TrackCache &cache, const QString &trackId, const QString &codec,
                                 const QByteArray &content) {
  const QString identity = TrackCache::identityFor(trackId);
  const QString writePath = cache.writePath(identity, codec);

  QFile file(writePath);

  if (!file.open(QIODevice::WriteOnly)) return {};

  file.write(content);
  file.close();

  QString error;

  if (!cache.finishWrite(writePath, identity, codec, &error)) return {};

  return cache.pathFor(identity);
}

void TrackCacheTest::ageFile(const QString &path, int minutesAgo) {
  QFile file(path);

  if (!file.open(QIODevice::ReadWrite)) return;

  file.setFileTime(QDateTime::currentDateTime().addSecs(-60 * minutesAgo),
                   QFileDevice::FileModificationTime);
  file.close();
}

void TrackCacheTest::writeAndReadBack() {
  QTemporaryDir dir;
  QVERIFY(dir.isValid());

  TrackCache cache;
  cache.setDirectory(dir.path() + QStringLiteral("/streams"));

  QVERIFY(!cache.contains(QStringLiteral("100")));

  const QString path = makeFile(cache, QStringLiteral("100"), QStringLiteral("mp3"),
                                QByteArray(2048, 'a'));

  QVERIFY(!path.isEmpty());
  QVERIFY(path.endsWith(QStringLiteral("100.mp3")));
  QVERIFY(cache.contains(QStringLiteral("100")));
  QCOMPARE(cache.totalBytes(), 2048LL);

  cache.saveMeta(QStringLiteral("100"), QStringLiteral("mp3"), 320);

  QString codec;
  int bitrate = 0;
  cache.loadMeta(QStringLiteral("100"), &codec, &bitrate);

  QCOMPARE(codec, QStringLiteral("mp3"));
  QCOMPARE(bitrate, 320);
}

void TrackCacheTest::ignoresTemporaryFiles() {
  QTemporaryDir dir;
  QVERIFY(dir.isValid());

  TrackCache cache;
  cache.setDirectory(dir.path() + QStringLiteral("/streams"));

  makeFile(cache, QStringLiteral("200"), QStringLiteral("mp3"), QByteArray(1024, 'b'));

  // Недописанная докачка лежит рядом, но кэшем не считается.
  QFile part(cache.writePath(QStringLiteral("201"), QStringLiteral("mp3")));

  if (!part.open(QIODevice::WriteOnly)) QFAIL("не удалось создать .part");

  part.write(QByteArray(4096, 'c'));
  part.close();

  QCOMPARE(cache.totalBytes(), 1024LL);
  QVERIFY(!cache.contains(QStringLiteral("201")));

  QString codec;
  int bitrate = 0;
  cache.loadMeta(QStringLiteral("201"), &codec, &bitrate);

  QVERIFY(codec.isEmpty());
  QCOMPARE(bitrate, 0);
}

void TrackCacheTest::evictsOldestFirst() {
  QTemporaryDir dir;
  QVERIFY(dir.isValid());

  TrackCache cache;
  cache.setDirectory(dir.path() + QStringLiteral("/streams"));
  cache.setLimitBytes(2048);

  const QString oldest = makeFile(cache, QStringLiteral("301"), QStringLiteral("mp3"),
                                  QByteArray(1024, 'd'));
  const QString middle = makeFile(cache, QStringLiteral("302"), QStringLiteral("mp3"),
                                  QByteArray(1024, 'e'));
  const QString newest = makeFile(cache, QStringLiteral("303"), QStringLiteral("mp3"),
                                  QByteArray(1024, 'f'));

  QVERIFY(!oldest.isEmpty() && !middle.isEmpty() && !newest.isEmpty());
  QCOMPARE(cache.totalBytes(), 3072LL);

  ageFile(oldest, 30);
  ageFile(middle, 20);
  ageFile(newest, 10);

  cache.evictIfNeeded();

  QVERIFY(cache.totalBytes() <= 2048);
  QVERIFY(!cache.contains(QStringLiteral("301")));
  QVERIFY(cache.contains(QStringLiteral("303")));
}

void TrackCacheTest::keepsPlayingTrack() {
  QTemporaryDir dir;
  QVERIFY(dir.isValid());

  TrackCache cache;
  cache.setDirectory(dir.path() + QStringLiteral("/streams"));
  cache.setLimitBytes(1024);

  const QString playing = makeFile(cache, QStringLiteral("401"), QStringLiteral("mp3"),
                                   QByteArray(1024, 'g'));
  const QString other = makeFile(cache, QStringLiteral("402"), QStringLiteral("mp3"),
                                 QByteArray(1024, 'h'));

  QVERIFY(!playing.isEmpty() && !other.isEmpty());

  ageFile(playing, 60);
  ageFile(other, 30);

  cache.evictIfNeeded(QStringLiteral("401"));

  // Самый старый файл играет прямо сейчас — его вытеснять нельзя.
  QVERIFY(cache.contains(QStringLiteral("401")));
  QVERIFY(!cache.contains(QStringLiteral("402")));
}

void TrackCacheTest::removeDeletesFileAndMeta() {
  QTemporaryDir dir;
  QVERIFY(dir.isValid());

  TrackCache cache;
  cache.setDirectory(dir.path() + QStringLiteral("/streams"));

  const QString path = makeFile(cache, QStringLiteral("500"), QStringLiteral("mp3"),
                                QByteArray(512, 'i'));

  QVERIFY(!path.isEmpty());

  cache.saveMeta(QStringLiteral("500"), QStringLiteral("mp3"), 128);
  cache.remove(QStringLiteral("500"));

  QVERIFY(!cache.contains(QStringLiteral("500")));
  QCOMPARE(cache.totalBytes(), 0LL);

  QString codec;
  int bitrate = 0;
  cache.loadMeta(QStringLiteral("500"), &codec, &bitrate);

  QVERIFY(codec.isEmpty());
  QCOMPARE(bitrate, 0);
}

QTEST_GUILESS_MAIN(TrackCacheTest)

#include "tst_track_cache.moc"
