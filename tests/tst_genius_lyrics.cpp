#include "../src/Yandex/Catalog/GeniusLyrics.h"
#include <QTest>

// Разбор HTML страницы Genius: контейнеры, <br>, теги, сущности.
class GeniusLyricsTest : public QObject {
  Q_OBJECT

private slots:
  void joinsContainers();

  void convertsBreaksAndTags();

  void decodesEntities();

  void dropsServiceBlocks();

  void keepsLyricsAfterNestedHeader();

  void keepsStanzaBreak();

  void emptyWhenNoContainer();
};

void GeniusLyricsTest::joinsContainers() {
  const QString html =
      "<div class=\"x\"><div data-lyrics-container=\"true\" class=\"c\">"
      "Первая строка<br>Вторая</div>"
      "<div data-lyrics-container=\"true\">Припев</div></div>";

  QCOMPARE(geniusLyricsFromHtml(html), QString("Первая строка\nВторая\nПрипев"));
}

void GeniusLyricsTest::convertsBreaksAndTags() {
  const QString html =
      "<div data-lyrics-container=\"true\">"
      "<a href=\"/123\">Строка</a><br/>и <span class=\"x\">ещё</span></div>";

  QCOMPARE(geniusLyricsFromHtml(html), QString("Строка\nи ещё"));
}

void GeniusLyricsTest::decodesEntities() {
  const QString html =
      "<div data-lyrics-container=\"true\">"
      "Don&#39;t &amp; &quot;stop&quot; &#x41F;</div>";

  QCOMPARE(geniusLyricsFromHtml(html), QString("Don't & \"stop\" П"));
}

void GeniusLyricsTest::dropsServiceBlocks() {
  const QString html =
      "<div data-lyrics-container=\"true\">Строка<br>"
      "<div data-exclude-from-selection=\"true\">You might also like</div>"
      "Ещё</div>";

  QCOMPARE(geniusLyricsFromHtml(html), QString("Строка\nЕщё"));
}

void GeniusLyricsTest::keepsLyricsAfterNestedHeader() {
  const QString html =
      "<div data-lyrics-container=\"true\" class=\"c\">"
      "<div data-exclude-from-selection=\"true\" class=\"h\">"
      "<button><span>3 Contributors</span></button>"
      "<div class=\"g\"><div class=\"t\"><h2>Дорога сна Lyrics</h2></div></div>"
      "</div>"
      "[Куплет 1]<br>Налей еще вина</div>";

  QCOMPARE(geniusLyricsFromHtml(html), QString("[Куплет 1]\nНалей еще вина"));
}

void GeniusLyricsTest::keepsStanzaBreak() {
  const QString html =
      "<div data-lyrics-container=\"true\">Куплет<br><br>Припев</div>";

  QCOMPARE(geniusLyricsFromHtml(html), QString("Куплет\n\nПрипев"));
}

void GeniusLyricsTest::emptyWhenNoContainer() {
  QVERIFY(geniusLyricsFromHtml("<html><body>Нет текста</body></html>").isEmpty());
}

QTEST_MAIN(GeniusLyricsTest)

#include "tst_genius_lyrics.moc"
