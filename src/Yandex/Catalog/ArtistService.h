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

    /*
     * Уже используем для блока
     * популярных композиций.
     */
    QList<Track> tracks;

    /*
     * Популярные альбомы.
     *
     * Получаются через:
     * /artists/{id}/direct-albums
     * sort-by=rating
     */
    QList<Album> popularAlbums;

    /*
     * Самый свежий релиз.
     *
     * Получается через:
     * /artists/{id}/direct-albums
     * sort-by=year
     */
    Album newRelease;

    /*
     * Похожие исполнители.
     */
    QList<Artist> similarArtists;
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
    void loadArtistTracks(
        const ArtistDetails &artist);

    void loadArtistAlbums(
        const ArtistDetails &artist);

    void loadArtistSimilar(
        const ArtistDetails &artist);

    YandexAuth *m_auth = nullptr;

    YandexClient *m_yandexClient = nullptr;
};