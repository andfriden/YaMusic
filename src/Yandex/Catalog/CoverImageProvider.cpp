#include "CoverImageProvider.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

// Loads an artwork image asynchronously.
CoverImageResponse::CoverImageResponse(
    const QString &url,
    const QSize &requestedSize)
    : m_url(url),
      m_requestedSize(requestedSize)
{
    load();
}

QQuickTextureFactory *CoverImageResponse::textureFactory() const
{
    if (m_image.isNull()) {
        return nullptr;
    }

    return QQuickTextureFactory::textureFactoryForImage(
        m_image);
}

void CoverImageResponse::cancel()
{
    // Nothing to cancel.
}

void CoverImageResponse::load()
{
    if (m_url.isEmpty()) {
        emit finished();
        return;
    }

    auto *networkManager =
        new QNetworkAccessManager();

    QNetworkRequest request{QUrl(m_url)};

    // Yandex artwork works reliably over HTTP/1.1.
    request.setAttribute(
        QNetworkRequest::Http2AllowedAttribute,
        false);

    QNetworkReply *reply =
        networkManager->get(request);

    QObject::connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, networkManager]() {
            if (reply->error() != QNetworkReply::NoError) {
                reply->deleteLater();
                networkManager->deleteLater();

                emit finished();
                return;
            }

            const QByteArray data =
                reply->readAll();

            QImage image;

            if (image.loadFromData(data)) {
                if (m_requestedSize.isValid() &&
                    !m_requestedSize.isEmpty()) {

                    image = image.scaled(
                        m_requestedSize,
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation);
                }

                m_image = image;
            }

            reply->deleteLater();
            networkManager->deleteLater();

            emit finished();
        });
}

CoverImageProvider::CoverImageProvider()
    : QQuickAsyncImageProvider()
{
}

QQuickImageResponse *
CoverImageProvider::requestImageResponse(
    const QString &id,
    const QSize &requestedSize)
{
    return new CoverImageResponse(
        createUrl(id),
        requestedSize);
}

QString CoverImageProvider::createUrl(
    const QString &uri) const
{
    if (uri.isEmpty()) {
        return {};
    }

    // QML can percent-encode the '%' characters
    // from Yandex's "%%" placeholder.
    QString url =
        QUrl::fromPercentEncoding(
            uri.toUtf8());

    if (url.contains("%%")) {
        url.replace("%%", "100x100");
    }

    if (!url.startsWith("http://") &&
        !url.startsWith("https://")) {
        url = "https://" + url;
    }

    return url;
}
