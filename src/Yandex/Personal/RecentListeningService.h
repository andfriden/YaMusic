#pragma once

#include <QDateTime>
#include <QHash>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>

#include "../../Models/Track.h"

class YandexAuth;
class YandexClient;

struct RecentListeningTrack
{
    QString trackId;
    QDateTime timestamp;
};

class RecentListeningService : public QObject
{
    Q_OBJECT

public:
    explicit RecentListeningService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void setUserId(
        const QString &userId);

    void load(
        int trackCount = 50,
        int contextCount = 10);

    signals:
        void loadingChanged(
            bool loading);

    void tracksReceived(
        const QList<Track> &tracks);

    void errorOccurred(
        const QString &message);

private:
    QList<RecentListeningTrack>
    parseResponse(
        const QByteArray &data) const;

    RecentListeningTrack
    parseListenedTrack(
        const QJsonObject &object) const;

    void resolveTracks(
        const QList<RecentListeningTrack> &references);

    YandexAuth *
        m_auth = nullptr;

    YandexClient *
        m_yandexClient = nullptr;

    QString
        m_userId;

    QList<RecentListeningTrack>
        m_pendingReferences;

    bool
        m_loading = false;
};