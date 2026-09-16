#pragma once

#include <QList>
#include <QObject>
#include <QString>
#include "../../Models/Track.h"

class YandexAuth;
class YandexClient;

class StationService : public QObject
{
    Q_OBJECT

public:
    explicit StationService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void loadStationTracks(
        const QString &stationType,
        const QString &stationId,
        const QString &queueTrackId = {});

    void loadMoreStationTracks(
        const QString &stationType,
        const QString &stationId,
        const QString &queueTrackId);

    void sendFeedback(
        const QString &stationType,
        const QString &stationId,
        const QString &event,
        const QString &trackId,
        const QString &batchId,
        qint64 totalPlayedSeconds = 0);

    signals:
        void stationTracksReceived(
            const QList<Track> &tracks,
            const QString &batchId);

    void feedbackSent(
        const QString &event);

    void errorOccurred(
        const QString &message);

private:
    Track parseTrack(
        const QJsonObject &object) const;

    YandexAuth *m_auth = nullptr;

    YandexClient *m_yandexClient = nullptr;

    bool m_loading = false;
};