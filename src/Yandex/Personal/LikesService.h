#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>

#include "../../Models/Track.h"


class YandexAuth;
class YandexClient;


class LikesService : public QObject
{
    Q_OBJECT

public:

    explicit LikesService(
        YandexAuth *auth,
        QObject *parent = nullptr);


    void loadLikedTracks(
        const QString &uid);


    signals:

        void tracksReceived(
            const QList<Track> &tracks);


    void loadingChanged(
        bool loading);


    void errorOccurred(
        const QString &message);


private:

    void loadTracksByIds(
        const QStringList &trackIds);


    YandexAuth *
        m_auth = nullptr;


    YandexClient *
        m_yandexClient = nullptr;


    bool m_loading =
        false;
};