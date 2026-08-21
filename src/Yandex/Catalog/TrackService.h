#pragma once

#include <QList>
#include <QObject>
#include <QString>

class YandexAuth;
class YandexClient;

struct TrackStreamInfo
{
    QString codec;
    QString downloadInfoUrl;

    bool gain = false;
    bool preview = false;
    bool direct = false;

    int bitrateInKbps = 0;
};

class TrackService : public QObject
{
    Q_OBJECT

public:
    explicit TrackService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void loadStreamInfo(
        const QString &trackId);

    signals:
        void streamInfoReceived(
            const QList<TrackStreamInfo> &streams);

    void streamUrlReceived(
        const QString &trackId,
        const QString &url);

    void errorOccurred(
        const QString &message);

private:
    TrackStreamInfo selectBestStream(
        const QList<TrackStreamInfo> &streams) const;

    void resolveStream(
        const QString &trackId,
        const TrackStreamInfo &stream);

    YandexAuth *m_auth = nullptr;
    YandexClient *m_yandexClient = nullptr;
};