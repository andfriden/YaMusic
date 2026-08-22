#pragma once

#include <QObject>
#include <QList>
#include <QString>

#include "../../Models/Track.h"

struct AlbumDetails
{
    Album album;

    QString description;

    int trackCount = 0;

    QList<Track> tracks;
};

class YandexAuth;
class YandexClient;

class AlbumService : public QObject
{
    Q_OBJECT

public:
    explicit AlbumService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void loadAlbum(
        const QString &id);

    signals:
        void albumReceived(
            const AlbumDetails &album);

    void errorOccurred(
        const QString &message);

private:
    YandexAuth *m_auth = nullptr;
    YandexClient *m_yandexClient = nullptr;
};