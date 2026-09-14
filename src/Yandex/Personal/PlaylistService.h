#pragma once

#include <QList>
#include <QObject>
#include <QPair>
#include <QString>
#include "../../Models/PersonalPlaylist.h"
#include "../../Models/Playlist.h"
#include "../YandexServiceBase.h"


class PlaylistService : public YandexServiceBase
{
    Q_OBJECT

public:

    explicit PlaylistService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    // Single playlist

    void loadPlaylist(
        const QString &uid,
        int kind);

    // Similar / recommended playlists

    void loadSimilarPlaylists(const QString &uuid);

    // Multiple playlists

    void loadPlaylists(const QList<QPair<QString, int>> &playlists);

    // User playlists

    void loadUserPlaylists(
        const QString &uid);

signals:

    void playlistReceived(
        const Playlist &playlist);

    void playlistsReceived(
        const QList<Playlist> &playlists);

    void similarPlaylistsReceived(
        const QList<Playlist> &playlists);

    void userPlaylistsReceived(
        const QList<PersonalPlaylist> &playlists);

    void errorOccurred(
        const QString &message);

private:

    void startNextPlaylistBatchRequests();

    void finishPlaylistBatch();

private:

    // Batch state

    QList<QPair<QString, int>>
        m_playlistBatchQueue;

    QList<Playlist>
        m_playlistBatchResults;

    int m_playlistBatchActive = 0;

    int m_playlistBatchCompleted = 0;

    bool m_playlistBatchError = false;

    static constexpr int
        MaxConcurrentPlaylistRequests = 5;
};