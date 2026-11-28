#include "PlaylistLink.h"

#include <QRegularExpression>
#include <QStringList>
#include <QUrl>

namespace {

bool isYandexHost(const QString &host) {
  static const QStringList kSuffixes = {
      QStringLiteral("yandex.ru"),  QStringLiteral("yandex.com"), QStringLiteral("yandex.by"),
      QStringLiteral("yandex.kz"), QStringLiteral("yandex.uz"),
  };

  for (const QString &suffix : kSuffixes) {
    if (host == suffix || host.endsWith(QLatin1Char('.') + suffix)) {
      return true;
    }
  }

  return false;
}

bool looksLikeLogin(const QString &value) {
  static const QRegularExpression kLogin(QStringLiteral("^(?=.*[A-Za-z0-9])[A-Za-z0-9._-]{1,64}$"));
  return kLogin.match(value).hasMatch();
}

bool looksLikeUuid(const QString &value) {
  static const QRegularExpression kUuid(
      QStringLiteral("^(?:[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-"
                     "[0-9a-fA-F]{4}-[0-9a-fA-F]{12}|[0-9a-fA-F]{32})$"));
  return kUuid.match(value).hasMatch();
}

} // namespace

PlaylistLink PlaylistLink::parse(const QString &text) {
  PlaylistLink link;

  const QString trimmed = text.trimmed();
  if (trimmed.isEmpty()) return link;

  // Из буфера обмена часто приходит ссылка без схемы.
  const QUrl url = QUrl::fromUserInput(trimmed);
  if (!url.isValid() || !isYandexHost(url.host().toLower())) return link;

  QStringList segments = url.path().split(QLatin1Char('/'), Qt::SkipEmptyParts);

  // https://yandex.ru/music/...
  if (!segments.isEmpty() && segments.first().compare(QStringLiteral("music"), Qt::CaseInsensitive) == 0) {
    segments.removeFirst();
  }

  for (int i = 0; i < segments.size(); ++i) {
    const QString segment = segments.at(i);

    if (segment.compare(QStringLiteral("users"), Qt::CaseInsensitive) == 0) {
      if (i + 3 >= segments.size()) return link;
      if (segments.at(i + 2).compare(QStringLiteral("playlists"), Qt::CaseInsensitive) != 0) {
        return link;
      }

      bool ok = false;
      const int kind = segments.at(i + 3).toInt(&ok);

      if (!ok || kind <= 0) return link;

      const QString login = segments.at(i + 1);

      // Логин уходит в путь запроса: пускаем только допустимые символы.
      if (!looksLikeLogin(login)) return link;

      link.type = Type::UserPlaylist;
      link.login = login;
      link.kind = kind;
      return link;
    }

    if (segment.compare(QStringLiteral("playlists"), Qt::CaseInsensitive) == 0 ||
        segment.compare(QStringLiteral("playlist"), Qt::CaseInsensitive) == 0) {
      if (i + 1 >= segments.size()) return link;

      const QString uuid = segments.at(i + 1);
      if (!looksLikeUuid(uuid)) return link;

      link.type = Type::Uuid;
      link.uuid = uuid.toLower();
      return link;
    }
  }

  return link;
}
