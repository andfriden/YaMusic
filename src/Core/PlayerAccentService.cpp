#include "PlayerAccentService.h"

#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <algorithm>


// =============================================================
// Constructor
// =============================================================

PlayerAccentService::PlayerAccentService(
    QObject *parent)
    : QObject(parent)
    , m_networkManager(
          new QNetworkAccessManager(this))
    , m_accentColor(
          QColor("#7C4DFF"))
{
}


// =============================================================
// Accent color
// =============================================================

QColor
PlayerAccentService::accentColor() const
{
    return m_accentColor;
}


// =============================================================
// Update accent
// =============================================================

void
PlayerAccentService::updateForCover(
    const QString &coverUri)
{
    const QString uri =
        coverUri.trimmed();


    if (
        uri.isEmpty()
    ) {
        return;
    }


    if (
        uri == m_currentCoverUri
    ) {
        return;
    }


    m_currentCoverUri =
        uri;


    // ---------------------------------------------------------
    // Cache
    // ---------------------------------------------------------

    if (
        m_cache.contains(uri)
    ) {
        const QColor cachedColor =
            m_cache.value(uri);


        if (
            cachedColor.isValid()
        ) {
            if (
                cachedColor !=
                m_accentColor
            ) {
                m_accentColor =
                    cachedColor;

                emit accentColorChanged();
            }
        }


        return;
    }


    const QString urlString =
        createUrl(uri);


    if (
        urlString.isEmpty()
    ) {
        return;
    }


    const QUrl url(
        urlString);


    if (
        !url.isValid()
    ) {
        return;
    }


    QNetworkRequest request(
        url);


    request.setAttribute(
        QNetworkRequest::RedirectPolicyAttribute,
        QNetworkRequest::NoLessSafeRedirectPolicy);


    QNetworkReply *reply =
        m_networkManager->get(
            request);


    if (
        reply == nullptr
    ) {
        return;
    }


    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this, reply, uri]()
        {
            const QByteArray data =
                reply->readAll();


            const bool successful =
                reply->error()
                == QNetworkReply::NoError;


            reply->deleteLater();


            if (
                uri != m_currentCoverUri
            ) {
                return;
            }


            if (
                !successful ||
                data.isEmpty()
            ) {
                return;
            }


            const QColor color =
                calculateDominantColor(
                    data);


            if (
                !color.isValid()
            ) {
                return;
            }


            m_cache.insert(
                uri,
                color);


            if (
                color ==
                m_accentColor
            ) {
                return;
            }


            m_accentColor =
                color;


            emit accentColorChanged();
        });
}


// =============================================================
// Dominant color
// =============================================================

QColor
PlayerAccentService::calculateDominantColor(
    const QByteArray &data) const
{
    QImage image;


    if (
        !image.loadFromData(
            data)
    ) {
        return {};
    }


    if (
        image.isNull()
    ) {
        return {};
    }


    image =
        image.scaled(
            32,
            32,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation);


    image =
        image.convertToFormat(
            QImage::Format_RGB32);


    quint64 red = 0;
    quint64 green = 0;
    quint64 blue = 0;
    quint64 count = 0;


    for (
        int y = 0;
        y < image.height();
        ++y
    ) {
        for (
            int x = 0;
        x < image.width();
        ++x
        ) {
            const QColor pixel =
                image.pixelColor(
                    x,
                    y);


            if (
                !pixel.isValid()
            ) {
                continue;
            }


            const int maxChannel =
                std::max({
                    pixel.red(),
                    pixel.green(),
                    pixel.blue()
                });


            const int minChannel =
                std::min({
                    pixel.red(),
                    pixel.green(),
                    pixel.blue()
                });


            // Ignore almost black pixels.

            if (
                maxChannel < 20
            ) {
                continue;
            }


            // Ignore almost white pixels.

            if (
                minChannel > 238
            ) {
                continue;
            }


            red +=
                static_cast<quint64>(
                    pixel.red());

            green +=
                static_cast<quint64>(
                    pixel.green());

            blue +=
                static_cast<quint64>(
                    pixel.blue());

            ++count;
        }
    }


    if (
        count == 0
    ) {
        return {};
    }


    QColor result(
        static_cast<int>(
            red / count),

        static_cast<int>(
            green / count),

        static_cast<int>(
            blue / count));


    if (
        !result.isValid()
    ) {
        return {};
    }


    // Prevent a too-dark accent.

    result =
        result.lighter(
            115);


    return result;
}


// =============================================================
// Cover URL
// =============================================================

QString
PlayerAccentService::createUrl(
    QString uri) const
{
    uri =
        uri.trimmed();


    if (
        uri.isEmpty()
    ) {
        return {};
    }


    // ---------------------------------------------------------
    // Already complete URL.
    // ---------------------------------------------------------

    if (
        uri.startsWith(
            "https://",
            Qt::CaseInsensitive)
        ||
        uri.startsWith(
            "http://",
            Qt::CaseInsensitive)
    ) {
        return uri;
    }


    // ---------------------------------------------------------
    // Protocol-relative URL.
    // ---------------------------------------------------------

    if (
        uri.startsWith(
            "//")
    ) {
        return
            QString(
                "https:%1")
                .arg(
                    uri);
    }


    // ---------------------------------------------------------
    // Yandex coverUri usually contains %% as the size marker.
    //
    // Example:
    //
    // avatars.yandex.net/get-music-content/.../%%
    //
    // Replace it with an actual image size.
    // ---------------------------------------------------------

    uri.replace(
        "%%",
        "600x600");


    // ---------------------------------------------------------
    // Common Yandex hosts.
    // ---------------------------------------------------------

    if (
        uri.startsWith(
            "avatars.yandex.net/",
            Qt::CaseInsensitive)
        ||
        uri.startsWith(
            "avatars.mds.yandex.net/",
            Qt::CaseInsensitive)
    ) {
        return
            QString(
                "https://%1")
                .arg(
                    uri);
    }


    // ---------------------------------------------------------
    // Relative Yandex path.
    // ---------------------------------------------------------

    return
        QString(
            "https://avatars.mds.yandex.net/%1")
            .arg(
                uri);
}