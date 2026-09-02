#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "../../Models/Playlist.h"


class YandexAuth;
class YandexClient;
class PlaylistService;


class NewPlaylistsService : public QObject
{
    Q_OBJECT

public:

    explicit NewPlaylistsService(
        YandexAuth *auth,
        PlaylistService *playlistService,
        QObject *parent = nullptr);


    void load();


    signals:

        void playlistsReceived(
            const QList<Playlist> &playlists);


    void errorOccurred(
        const QString &message);


private:

    YandexAuth *
        m_auth = nullptr;

    YandexClient *
        m_yandexClient = nullptr;

    PlaylistService *
        m_playlistService = nullptr;
};