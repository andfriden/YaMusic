#pragma once

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>

#include "../../Models/Track.h"

class YandexAuth;
class YandexClient;

class YandexPersonal : public QObject
{
    Q_OBJECT

public:
    explicit YandexPersonal(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void loadMyWave();

    void loadMoreMyWave(
        const QString &queueTrackId);

    void sendMyWaveFeedback(
        const QString &event,
        const QString &trackId,
        const QString &batchId,
        qint64 totalPlayedSeconds = 0);

    signals:
        void myWaveReceived(
            const QList<Track> &tracks,
            const QString &batchId);

    void feedbackSent(
        const QString &event);

    void feedbackError(
        const QString &message);

    void errorOccurred(
        const QString &message);

private:
    void loadMyWaveInternal(
        const QString &queueTrackId);

    Track parseTrack(
        const QJsonObject &object) const;

    YandexAuth *m_auth = nullptr;

    YandexClient *m_yandexClient = nullptr;

    bool m_loading = false;
};