#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "../../Models/Genre.h"


class YandexAuth;
class YandexClient;


class GenreService : public QObject
{
    Q_OBJECT

public:

    explicit GenreService(
        YandexAuth *auth,
        QObject *parent = nullptr);


    void loadGenres();


    void loadTagPlaylistIds(
        const QString &tagId);


    signals:

        void genresReceived(
            const QList<Genre> &genres);


    void tagPlaylistIdsReceived(
        const QString &tagId,
        const QList<QPair<QString, int>> &playlists);


    void errorOccurred(
        const QString &message);


private:

    YandexAuth *
        m_auth = nullptr;


    YandexClient *
        m_yandexClient = nullptr;
};