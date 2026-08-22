#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "../../Models/Track.h"

struct ArtistDetails
{
    QString id;

    QString name;

    QString coverUri;

    QString description;

    QList<QString> genres;

    QList<Track> tracks;
};

class YandexAuth;
class YandexClient;

class ArtistService : public QObject
{
    Q_OBJECT

public:
    explicit ArtistService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void loadArtist(
        const QString &id);

    signals:
        void artistReceived(
            const ArtistDetails &artist);

    void errorOccurred(
        const QString &message);

private:
    YandexAuth *m_auth = nullptr;

    YandexClient *m_yandexClient = nullptr;
};