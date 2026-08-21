#pragma once

#include <QQuickAsyncImageProvider>
#include <QQuickImageResponse>
#include <QQuickTextureFactory>
#include <QImage>

// Loads Yandex Music artwork asynchronously for QML.
class CoverImageResponse : public QQuickImageResponse
{
    Q_OBJECT

public:
    explicit CoverImageResponse(
        const QString &url,
        const QSize &requestedSize);

    QQuickTextureFactory *textureFactory() const override;

    void cancel() override;

private:
    void load();

    QString m_url;
    QSize m_requestedSize;
    QImage m_image;
};

// Provides asynchronous Yandex Music artwork to QML.
class CoverImageProvider : public QQuickAsyncImageProvider
{
public:
    explicit CoverImageProvider();

    QQuickImageResponse *requestImageResponse(
        const QString &id,
        const QSize &requestedSize) override;

private:
    QString createUrl(const QString &uri) const;
};
