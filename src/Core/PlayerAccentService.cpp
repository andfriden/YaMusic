#include "PlayerAccentService.h"
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <algorithm>

PlayerAccentService::PlayerAccentService(QObject *parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this)),
      m_accentColor(QColor("#7C4DFF")) {}

QColor PlayerAccentService::accentColor() const {
  return m_accentColor;
}

void PlayerAccentService::updateForCover(const QString &coverUri) {
  const QString uri = coverUri.trimmed();

  if (uri.isEmpty()) {
    return;
  }

  if (uri == m_currentCoverUri) {
    return;
  }

  m_currentCoverUri = uri;

  if (m_cache.contains(uri)) {
    const QColor cachedColor = m_cache.value(uri);

    if (cachedColor.isValid()) {
      if (cachedColor != m_accentColor) {
        m_accentColor = cachedColor;
        emit accentColorChanged();
      }
    }
    return;
  }

  const QString urlString = createUrl(uri);

  if (urlString.isEmpty()) {
    return;
  }

  const QUrl url(urlString);

  if (!url.isValid()) {
    return;
  }

  QNetworkRequest request(url);

  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                       QNetworkRequest::NoLessSafeRedirectPolicy);
  QNetworkReply *reply = m_networkManager->get(request);

  if (reply == nullptr) {
    return;
  }

  connect(reply, &QNetworkReply::finished, this, [this, reply, uri]() {
    const QByteArray data = reply->readAll();
    const bool successful = reply->error() == QNetworkReply::NoError;
    reply->deleteLater();

    if (uri != m_currentCoverUri) {
      return;
    }

    if (!successful || data.isEmpty()) {
      return;
    }

    const QColor color = calculateDominantColor(data);

    if (!color.isValid()) {
      return;
    }

    m_cache.insert(uri, color);

    if (color == m_accentColor) {
      return;
    }

    m_accentColor = color;
    emit accentColorChanged();
  });
}

QColor PlayerAccentService::calculateDominantColor(const QByteArray &data) const {
  QImage image;

  if (!image.loadFromData(data)) {
    return {};
  }

  if (image.isNull()) {
    return {};
  }

  image = image.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  image = image.convertToFormat(QImage::Format_RGB32);
  quint64 red = 0;
  quint64 green = 0;
  quint64 blue = 0;
  quint64 count = 0;

  for (int y = 0; y < image.height(); ++y) {
    for (int x = 0; x < image.width(); ++x) {
      const QColor pixel = image.pixelColor(x, y);
      if (!pixel.isValid()) continue;
      const int maxChannel = std::max({pixel.red(), pixel.green(), pixel.blue()});
      const int minChannel = std::min({pixel.red(), pixel.green(), pixel.blue()});
      if (maxChannel < 20) continue;
      if (minChannel > 238) continue;
      red += static_cast<quint64>(pixel.red());
      green += static_cast<quint64>(pixel.green());
      blue += static_cast<quint64>(pixel.blue());
      ++count;
    }
  }

  if (count == 0) {
    return {};
  }

  QColor result(static_cast<int>(red / count),

                static_cast<int>(green / count),

                static_cast<int>(blue / count));

  if (!result.isValid()) {
    return {};
  }

  result = result.lighter(115);
  return result;
}

QString PlayerAccentService::createUrl(QString uri) const {
  uri = uri.trimmed();

  if (uri.isEmpty()) {
    return {};
  }

  if (uri.startsWith("https://", Qt::CaseInsensitive) ||
      uri.startsWith("http://", Qt::CaseInsensitive)) {
    return uri;
  }

  if (uri.startsWith("//")) {
    return QString("https:%1").arg(uri);
  }

  // В coverUri у Яндекса размер кодируется как %% — подставляем конкретный.

  uri.replace("%%", "600x600");

  // Основные хосты картинок Яндекса

  if (uri.startsWith("avatars.yandex.net/", Qt::CaseInsensitive) ||
      uri.startsWith("avatars.mds.yandex.net/", Qt::CaseInsensitive)) {
    return QString("https://%1").arg(uri);
  }
  return QString("https://avatars.mds.yandex.net/%1").arg(uri);
}