#include "../src/Yandex/Personal/PlaylistLink.h"

#include <QtTest>

// Разбор ссылок на плейлисты Яндекс Музыки: формы с логином и uuid,
// варианты без схемы и с query-параметрами, отсев чужих ссылок.
class PlaylistLinkTest : public QObject {
  Q_OBJECT

private slots:
  void userPlaylist_data();
  void userPlaylist();

  void uuidPlaylist_data();
  void uuidPlaylist();

  void rejectsInvalid();
};

void PlaylistLinkTest::userPlaylist_data() {
  QTest::addColumn<QString>("link");
  QTest::addColumn<QString>("login");
  QTest::addColumn<int>("kind");

  QTest::newRow("схема и хост") << QStringLiteral(
                                       "https://music.yandex.ru/users/and.friden/playlists/1007")
                                << QStringLiteral("and.friden") << 1007;
  QTest::newRow("без схемы") << QStringLiteral("music.yandex.ru/users/and.friden/playlists/42")
                             << QStringLiteral("and.friden") << 42;
  QTest::newRow("query и слэш")
      << QStringLiteral("https://music.yandex.ru/users/and.friden/playlists/1007/?utm_source=share")
      << QStringLiteral("and.friden") << 1007;
  QTest::newRow("yandex.ru/music") << QStringLiteral(
                                          "https://yandex.ru/music/users/and.friden/playlists/5")
                                   << QStringLiteral("and.friden") << 5;
  QTest::newRow("пробелы вокруг") << QStringLiteral(
                                         "  https://music.yandex.com/users/test.user/playlists/9  ")
                                  << QStringLiteral("test.user") << 9;
}

void PlaylistLinkTest::userPlaylist() {
  QFETCH(QString, link);
  QFETCH(QString, login);
  QFETCH(int, kind);

  const PlaylistLink parsed = PlaylistLink::parse(link);

  QCOMPARE(parsed.type, PlaylistLink::Type::UserPlaylist);
  QCOMPARE(parsed.login, login);
  QCOMPARE(parsed.kind, kind);
  QVERIFY(parsed.uuid.isEmpty());
}

void PlaylistLinkTest::uuidPlaylist_data() {
  QTest::addColumn<QString>("link");
  QTest::addColumn<QString>("uuid");

  QTest::newRow("uuid с дефисами")
      << QStringLiteral(
             "https://music.yandex.ru/playlists/eb653303-c8e8-5c86-9af1-967746f73882")
      << QStringLiteral("eb653303-c8e8-5c86-9af1-967746f73882");
  QTest::newRow("share-параметр")
      << QStringLiteral("https://music.yandex.ru/playlists/eb653303-c8e8-5c86-9af1-967746f73882"
                        "?share=1")
      << QStringLiteral("eb653303-c8e8-5c86-9af1-967746f73882");
  QTest::newRow("32 hex без дефисов")
      << QStringLiteral("https://music.yandex.ru/playlist/EB653303C8E85C869AF1967746F73882")
      << QStringLiteral("eb653303c8e85c869af1967746f73882");
}

void PlaylistLinkTest::uuidPlaylist() {
  QFETCH(QString, link);
  QFETCH(QString, uuid);

  const PlaylistLink parsed = PlaylistLink::parse(link);

  QCOMPARE(parsed.type, PlaylistLink::Type::Uuid);
  QCOMPARE(parsed.uuid, uuid);
  QVERIFY(parsed.login.isEmpty());
  QCOMPARE(parsed.kind, 0);
}

void PlaylistLinkTest::rejectsInvalid() {
  const QStringList links = {
      QString(),
      QStringLiteral("   "),
      QStringLiteral("просто текст"),
      QStringLiteral("https://example.com/users/and.friden/playlists/1007"),
      QStringLiteral("https://music.yandex.ru/users/and.friden"),
      QStringLiteral("https://music.yandex.ru/users/and.friden/albums/1007"),
      QStringLiteral("https://music.yandex.ru/users/and.friden/playlists/abc"),
      QStringLiteral("https://music.yandex.ru/users/and.friden/playlists/0"),
      QStringLiteral("https://music.yandex.ru/playlists/not-a-uuid"),
      QStringLiteral("https://music.yandex.ru/playlists/"),
  };

  for (const QString &link : links) {
    QCOMPARE(PlaylistLink::parse(link).type, PlaylistLink::Type::Invalid);
  }
}

QTEST_GUILESS_MAIN(PlaylistLinkTest)

#include "tst_playlist_link.moc"
