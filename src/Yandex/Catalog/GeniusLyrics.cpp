#include "GeniusLyrics.h"

#include <QRegularExpression>
#include <QStringList>

namespace {

// Индекс сразу после </div>, закрывающего div, который открыт в openPos.
// Нужен, потому что контейнеры Genius содержат вложенные div (заголовок,
// «You might also like»), и регулярка на них обрывается не там.
int divEnd(const QString &html, int openPos) {
  static const QString openTag = QStringLiteral("<div");
  static const QString closeTag = QStringLiteral("</div>");

  int depth = 0;
  int i = openPos;

  while (i < html.size()) {
    const int open = html.indexOf(openTag, i, Qt::CaseInsensitive);
    const int close = html.indexOf(closeTag, i, Qt::CaseInsensitive);
    if (close < 0) return -1;

    if (open >= 0 && open < close) {
      ++depth;
      i = open + openTag.size();
    } else {
      --depth;
      i = close + closeTag.size();
      if (depth == 0) return i;
    }
  }

  return -1;
}

// Удаляет служебные div с data-exclude-from-selection вместе с содержимым.
void removeExcludedDivs(QString &html) {
  static const QRegularExpression open(
      R"(<div[^>]*data-exclude-from-selection[^>]*>)",
      QRegularExpression::CaseInsensitiveOption);

  auto match = open.match(html);

  while (match.hasMatch()) {
    const int end = divEnd(html, match.capturedStart());
    if (end < 0) return;

    html.remove(match.capturedStart(), end - match.capturedStart());
    match = open.match(html);
  }
}

QString decodeEntities(QString text) {
  text.replace("&nbsp;", " ");
  text.replace("&lt;", "<");
  text.replace("&gt;", ">");
  text.replace("&quot;", "\"");
  text.replace("&apos;", "'");
  text.replace("&mdash;", "—");
  text.replace("&ndash;", "–");
  text.replace("&hellip;", "…");
  text.replace("&laquo;", "«");
  text.replace("&raquo;", "»");
  text.replace("&rsquo;", "’");
  text.replace("&lsquo;", "‘");
  text.replace("&ldquo;", "“");
  text.replace("&rdquo;", "”");

  // Числовые сущности: &#1055; и &#x41F;
  static const QRegularExpression numeric(
      R"(&#(x?)([0-9A-Fa-f]+);)", QRegularExpression::CaseInsensitiveOption);

  QString out;
  int last = 0;
  auto it = numeric.globalMatch(text);

  while (it.hasNext()) {
    const QRegularExpressionMatch match = it.next();
    out += text.mid(last, match.capturedStart() - last);

    bool ok = false;
    const uint value = match.captured(2).toUInt(&ok, match.captured(1).isEmpty() ? 10 : 16);
    const char32_t code = static_cast<char32_t>(value);

    if (ok && value > 0 && value <= 0x10FFFF) {
      out += QString::fromUcs4(&code, 1);
    } else {
      out += match.captured(0);
    }

    last = match.capturedEnd();
  }

  out += text.mid(last);
  // &amp; последним, чтобы не раскрыть уже декодированные сущности повторно.
  out.replace("&amp;", "&");
  return out;
}

} // namespace

QString geniusLyricsFromHtml(const QString &html) {
  // shortcut: неполный HTML-парсер — теги ищутся регулярками, а границы
  // блоков считаются по вложенности <div>. Хватает текущей вёрстке Genius;
  // менять при её смене.
  static const QRegularExpression containerOpen(
      R"(<div[^>]*data-lyrics-container=["']true["'][^>]*>)",
      QRegularExpression::CaseInsensitiveOption);
  static const QRegularExpression lineBreak(R"(<br\s*/?>)",
                                            QRegularExpression::CaseInsensitiveOption);
  static const QRegularExpression tags(R"(<[^>]*>)");

  QStringList blocks;
  auto it = containerOpen.globalMatch(html);

  while (it.hasNext()) {
    const QRegularExpressionMatch match = it.next();
    const int end = divEnd(html, match.capturedStart());
    if (end < 0) continue;

    QString text = html.mid(match.capturedEnd(),
                            end - match.capturedEnd() - QStringLiteral("</div>").size());
    removeExcludedDivs(text);
    text.replace(lineBreak, "\n");
    text.remove(tags);
    blocks.append(decodeEntities(text));
  }

  if (blocks.isEmpty()) return {};

  QStringList out;

  for (const QString &rawLine : blocks.join('\n').split('\n')) {
    const QString line = rawLine.trimmed();

    if (line.isEmpty()) {
      // Пустая строка — граница куплета.
      if (!out.isEmpty() && !out.last().isEmpty()) out.append(QString());
      continue;
    }

    out.append(line);
  }

  while (!out.isEmpty() && out.last().isEmpty())
    out.removeLast();

  return out.join('\n');
}
