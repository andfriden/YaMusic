#pragma once

#include <QImage>
#include <QQuickAsyncImageProvider>
#include <QQuickImageResponse>
#include <QQuickTextureFactory>
#include <QSize>
#include <QString>

class CoverImageResponse : public QQuickImageResponse
{
    Q_OBJECT

public:
    explicit CoverImageResponse(
        const QString &url,
        const QSize &requestedSize,
        bool circular);

    QQuickTextureFactory *
    textureFactory() const override;

    void cancel() override;

private:
    void load();

    QImage makeCircular(
        const QImage &image) const;

    QString m_url;
    QSize m_requestedSize;
    bool m_circular = false;

    QImage m_image;
};


class CoverImageProvider :
    public QQuickAsyncImageProvider
{
public:
    explicit CoverImageProvider();

    QQuickImageResponse *
    requestImageResponse(
        const QString &id,
        const QSize &requestedSize) override;

private:
    QString createUrl(
        QString uri) const;

    bool isCircularRequest(
        const QString &id) const;

    QString stripRequestPrefix(
        QString id) const;
};