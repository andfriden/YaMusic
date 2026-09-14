#pragma once

#include <QList>
#include <QString>
#include "../../Models/Track.h"
#include "../YandexServiceBase.h"

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

class TrackService : public YandexServiceBase
{
    Q_OBJECT

public:
    explicit TrackService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void loadStreamInfo(
        const QString &trackId);

    /*
     * Загружает дополнительную информацию о треке
     * (текст песни) через /tracks/{id}/supplement.
     */
    void loadSupplementary(
        const QString &trackId);

    signals:
        void streamInfoReceived(
            const QList<TrackStreamInfo> &streams);

    void streamUrlReceived(
        const QString &trackId,
        const QString &url);

    void supplementReceived(
        const TrackSupplementary &supplement);

    void errorOccurred(
        const QString &message);

private:
    TrackStreamInfo selectBestStream(
        const QList<TrackStreamInfo> &streams) const;

    void resolveStream(
        const QString &trackId,
        const TrackStreamInfo &stream);
};