#pragma once

#include <QList>
#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>

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


    YandexAuth *m_auth = nullptr;
    YandexClient *m_yandexClient = nullptr;

    bool m_loading = false;

    QSet<QString> m_likedTrackIds;
};