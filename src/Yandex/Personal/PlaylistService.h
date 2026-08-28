#pragma once

#include <QObject>
#include <QList>
#include <QString>

#include "../../Models/PersonalPlaylist.h"
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


    // =============================================================
    // Single playlist
    // =============================================================

    void loadPlaylist(
        const QString &uid,
        int kind);


    // =============================================================
    // User playlists
    // =============================================================

    void loadUserPlaylists(
        const QString &uid);


    signals:

        void playlistReceived(
            const Playlist &playlist);


    void userPlaylistsReceived(
        const QList<PersonalPlaylist> &playlists);


    void errorOccurred(
        const QString &message);


private:

    YandexAuth *
        m_auth = nullptr;


    YandexClient *
        m_yandexClient = nullptr;
};