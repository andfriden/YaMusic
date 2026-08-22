#pragma once

#include <QObject>
#include <QString>

#include "../../Models/Playlist.h"

class YandexAuth;
class YandexClient;

class PlaylistService : public QObject
{
    Q_OBJECT

public:
    explicit PlaylistService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void loadPlaylist(
        const QString &uid,
        int kind);

    signals:
        void playlistReceived(
            const Playlist &playlist);

    void errorOccurred(
        const QString &message);

private:
    YandexAuth *m_auth = nullptr;
    YandexClient *m_yandexClient = nullptr;
};