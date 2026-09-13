#pragma once

#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>
#include "../../Models/Track.h"
#include "../YandexServiceBase.h"

class LikesService : public YandexServiceBase
{
    Q_OBJECT

public:

    explicit LikesService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void loadLikedTracks(
        const QString &uid);

    void loadLikedAlbums(
        const QString &uid);

    void loadLikedArtists(
        const QString &uid);

    void addLike(
        const QString &uid,
        const QString &trackId);

    void removeLike(
        const QString &uid,
        const QString &trackId);

    bool isLiked(
        const QString &trackId) const;

    signals:

        void tracksReceived(
            const QList<Track> &tracks);

    void albumsReceived(
        const QList<Album> &albums);

    void artistsReceived(
        const QList<Artist> &artists);

    void loadingChanged(
        bool loading);

    void errorOccurred(
        const QString &message);

    void likeChanged(
        const QString &trackId,
        bool liked);

private:

    void loadTracksByIds(
        const QStringList &trackIds);

    void changeLike(
        const QString &uid,
        const QString &trackId,
        bool liked);

    bool m_loadingTracks = false;
    bool m_loadingAlbums = false;
    bool m_loadingArtists = false;

    QSet<QString> m_likedTrackIds;
};