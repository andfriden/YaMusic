#include "CoverImageProvider.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QPainterPath>
#include <QUrl>

namespace
{

constexpr auto kCirclePrefix =
    "circle/";

}


// =============================================================
// CoverImageResponse
// =============================================================

CoverImageResponse::CoverImageResponse(
    const QString &url,
    const QSize &requestedSize,
    bool circular)
    : m_url(url)
    , m_requestedSize(requestedSize)
    , m_circular(circular)
{
    load();
}


QQuickTextureFactory *
CoverImageResponse::textureFactory() const
{
    if (m_image.isNull()) {

        qDebug()
            << "CoverImageResponse:"
            << "empty image";

        return nullptr;
    }

    qDebug()
        << "CoverImageResponse:"
        << "texture"
        << "| size:"
        << m_image.size()
        << "| format:"
        << m_image.format()
        << "| circular:"
        << m_circular;

    return QQuickTextureFactory::textureFactoryForImage(
        m_image);
}


void CoverImageResponse::cancel()
{
    /*
     * The current provider performs every request
     * independently.
     *
     * The response is allowed to finish normally.
     */
}


QImage CoverImageResponse::makeCircular(
    const QImage &image) const
{
    if (image.isNull()) {
        return {};
    }

    /*
     * ---------------------------------------------------------
     * Make the image square first.
     * ---------------------------------------------------------
     *
     * We use the smallest dimension, so the circle is always
     * completely contained inside the original artwork.
     */

    const int side =
        qMin(
            image.width(),
            image.height());

    if (side <= 0) {
        return {};
    }

    const int x =
        (image.width() - side) / 2;

    const int y =
        (image.height() - side) / 2;

    QImage square =
        image.copy(
            x,
            y,
            side,
            side);

    if (square.isNull()) {
        return {};
    }

    /*
     * ---------------------------------------------------------
     * Resize to the requested size if one was provided.
     * ---------------------------------------------------------
     */

    QSize targetSize =
        square.size();

    if (
        m_requestedSize.width() > 0 &&
        m_requestedSize.height() > 0
    ) {

        const int target =
            qMin(
                m_requestedSize.width(),
                m_requestedSize.height());

        if (target > 0) {

            targetSize =
                QSize(
                    target,
                    target);
        }
    }

    if (
        square.size() != targetSize
    ) {

        square =
            square.scaled(
                targetSize,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation);
    }

    /*
     * ---------------------------------------------------------
     * Create transparent ARGB image.
     * ---------------------------------------------------------
     */

    QImage result(
        square.size(),
        QImage::Format_ARGB32_Premultiplied);

    result.fill(
        Qt::transparent);

    /*
     * ---------------------------------------------------------
     * Draw the artwork through a circular clipping path.
     * ---------------------------------------------------------
     */

    QPainter painter(
        &result);

    painter.setRenderHint(
        QPainter::Antialiasing,
        true);

    painter.setRenderHint(
        QPainter::SmoothPixmapTransform,
        true);

    const QRectF targetRect(
        0,
        0,
        result.width(),
        result.height());

    const qreal radius =
        qMin(
            targetRect.width(),
            targetRect.height()) /
        2.0;

    QPainterPath path;

    path.addEllipse(
        targetRect.center(),
        radius,
        radius);

    painter.setClipPath(
        path);

    painter.drawImage(
        targetRect,
        square);

    painter.end();

    return result;
}


void CoverImageResponse::load()
{
    if (m_url.isEmpty()) {

        qDebug()
            << "CoverImageResponse:"
            << "empty URL";

        emit finished();

        return;
    }

    qDebug()
        << "CoverImageResponse:"
        << "loading:"
        << m_url
        << "| requested:"
        << m_requestedSize
        << "| circular:"
        << m_circular;

    auto *networkManager =
        new QNetworkAccessManager();

    QNetworkRequest request{
        QUrl(m_url)
    };

    request.setHeader(
        QNetworkRequest::UserAgentHeader,
        QStringLiteral(
            "YaMusic/1.0 (Qt)"
        ));

    request.setAttribute(
        QNetworkRequest::Http2AllowedAttribute,
        false);

    QNetworkReply *reply =
        networkManager->get(
            request);

    QObject::connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, networkManager]() {

            const QVariant statusCode =
                reply->attribute(
                    QNetworkRequest::HttpStatusCodeAttribute);

            qDebug()
                << "CoverImageResponse:"
                << "HTTP"
                << statusCode
                << "| error:"
                << reply->errorString();

            if (
                reply->error() !=
                QNetworkReply::NoError
            ) {

                reply->deleteLater();

                networkManager
                    ->deleteLater();

                emit finished();

                return;
            }

            const QByteArray data =
                reply->readAll();

            qDebug()
                << "CoverImageResponse:"
                << "bytes:"
                << data.size();

            QImage image;

            if (
                !image.loadFromData(
                    data)
            ) {

                qDebug()
                    << "CoverImageResponse:"
                    << "failed to decode image";

                reply->deleteLater();

                networkManager
                    ->deleteLater();

                emit finished();

                return;
            }

            qDebug()
                << "CoverImageResponse:"
                << "decoded:"
                << image.size()
                << "| format:"
                << image.format();

            /*
             * -------------------------------------------------
             * Normal artwork
             * -------------------------------------------------
             */

            if (!m_circular) {

                m_image =
                    image;
            }

            /*
             * -------------------------------------------------
             * Circular artwork
             * -------------------------------------------------
             */

            else {

                m_image =
                    makeCircular(
                        image);
            }

            if (m_image.isNull()) {

                qDebug()
                    << "CoverImageResponse:"
                    << "failed to process image";

                reply->deleteLater();

                networkManager
                    ->deleteLater();

                emit finished();

                return;
            }

            qDebug()
                << "CoverImageResponse:"
                << "final:"
                << m_image.size()
                << "| format:"
                << m_image.format()
                << "| circular:"
                << m_circular;

            reply->deleteLater();

            networkManager
                ->deleteLater();

            emit finished();
        });
}


// =============================================================
// CoverImageProvider
// =============================================================

CoverImageProvider::CoverImageProvider()
    : QQuickAsyncImageProvider()
{
}


QQuickImageResponse *
CoverImageProvider::requestImageResponse(
    const QString &id,
    const QSize &requestedSize)
{
    const bool circular =
        isCircularRequest(
            id);

    const QString cleanId =
        stripRequestPrefix(
            id);

    const QString url =
        createUrl(
            cleanId);

    qDebug()
        << "CoverImageProvider:"
        << "id:"
        << id
        << "| cleanId:"
        << cleanId
        << "| url:"
        << url
        << "| circular:"
        << circular;

    return new CoverImageResponse(
        url,
        requestedSize,
        circular);
}


bool CoverImageProvider::isCircularRequest(
    const QString &id) const
{
    return id.startsWith(
        QString::fromLatin1(
            kCirclePrefix));
}


QString CoverImageProvider::stripRequestPrefix(
    QString id) const
{
    const QString prefix =
        QString::fromLatin1(
            kCirclePrefix);

    if (
        id.startsWith(
            prefix)
    ) {

        id.remove(
            0,
            prefix.size());
    }

    return id;
}


QString CoverImageProvider::createUrl(
    QString uri) const
{
    if (
        uri.isEmpty()
    ) {

        return {};
    }

    /*
     * QML may pass:
     *
     * avatars.yandex.net/.../%%
     *
     * as:
     *
     * avatars.yandex.net/.../%25%25
     *
     * Decode the percent escaping first.
     */

    QString normalized =
        QUrl::fromPercentEncoding(
            uri.toUtf8());

    normalized =
        normalized.trimmed();

    /*
     * Yandex Music uses %% as the image-size placeholder.
     */

    normalized.replace(
        "%%",
        "200x200");

    /*
     * Also handle the encoded form in case something
     * reaches this function without being decoded above.
     */

    normalized.replace(
        "%25%25",
        "200x200");

    if (
        normalized.startsWith(
            "http://") ||
        normalized.startsWith(
            "https://")
    ) {

        return normalized;
    }

    return QStringLiteral(
        "https://") +
        normalized;
}