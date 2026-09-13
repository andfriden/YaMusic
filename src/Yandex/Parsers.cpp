#include "Parsers.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>


QString parseId(
    const QJsonObject &object)
{
    const QJsonValue idValue =
        object.value("id");

    if (
        idValue.isString()
    ) {
        const QString id =
            idValue.toString();

        if (
            !id.isEmpty()
        ) {
            return id;
        }
    }

    if (
        idValue.isDouble()
    ) {
        const qint64 id =
            idValue.toInteger();

        if (id > 0) {
            return QString::number(
                id);
        }
    }

    const qint64 realId =
        object.value("realId")
            .toInteger();

    if (realId > 0) {
        return QString::number(
            realId);
    }

    return {};
}


QString parseCoverUri(
    const QJsonObject &object)
{
    QString uri =
        object.value("coverUri")
            .toString();

    if (
        !uri.isEmpty()
    ) {
        return uri;
    }

    const QJsonObject cover =
        object.value("cover")
            .toObject();

    uri =
        cover.value("uri")
            .toString();

    if (
        !uri.isEmpty()
    ) {
        return uri;
    }

    return object.value("ogImage")
        .toString();
}


Artist parseArtist(
    const QJsonObject &object)
{
    QJsonObject obj =
        object;

    if (
        obj.value("artist")
            .isObject()
    ) {
        obj =
            obj.value("artist")
                .toObject();
    }

    Artist artist;

    artist.id =
        parseId(obj);

    artist.name =
        obj.value("name")
            .toString();

    artist.coverUri =
        parseCoverUri(obj);

    return artist;
}


Album parseAlbum(
    const QJsonObject &object)
{
    QJsonObject obj =
        object;

    if (
        obj.value("album")
            .isObject()
    ) {
        obj =
            obj.value("album")
                .toObject();
    }

    Album album;

    album.id =
        parseId(obj);

    album.title =
        obj.value("title")
            .toString();

    album.coverUri =
        parseCoverUri(obj);

    album.year =
        obj.value("year")
            .toInt();

    return album;
}


Track parseTrack(
    const QJsonObject &object)
{
    QJsonObject obj =
        object;

    if (
        obj.value("track")
            .isObject()
    ) {
        obj =
            obj.value("track")
                .toObject();
    }

    Track track;

    track.id =
        parseId(obj);

    track.title =
        obj.value("title")
            .toString();

    track.coverUri =
        parseCoverUri(obj);

    track.durationMs =
        obj.value("durationMs")
            .toInt();

    const QJsonArray artists =
        obj.value("artists")
            .toArray();

    for (
        const QJsonValue &value :
        artists
    ) {
        if (
            !value.isObject()
        ) {
            continue;
        }

        const Artist artist =
            parseArtist(
                value.toObject());

        if (
            !artist.name.isEmpty()
        ) {
            track.artists.append(
                artist);
        }
    }

    const QJsonArray albums =
        obj.value("albums")
            .toArray();

    for (
        const QJsonValue &value :
        albums
    ) {
        if (
            !value.isObject()
        ) {
            continue;
        }

        const Album album =
            parseAlbum(
                value.toObject());

        if (
            !album.title.isEmpty()
        ) {
            track.albums.append(
                album);
        }
    }

    return track;
}


QList<Track> parseTrackArray(
    const QJsonArray &array)
{
    QList<Track> tracks;

    for (
        const QJsonValue &value :
        array
    ) {
        if (
            !value.isObject()
        ) {
            continue;
        }

        const Track track =
            parseTrack(
                value.toObject());

        if (
            track.id.isEmpty()
        ) {
            continue;
        }

        tracks.append(track);
    }

    return tracks;
}


QJsonObject unwrapResult(
    const QJsonDocument &document)
{
    if (
        !document.isObject()
    ) {
        return {};
    }

    const QJsonObject root =
        document.object();

    const QJsonValue result =
        root.value("result");

    if (
        result.isObject()
    ) {
        return result.toObject();
    }

    return root;
}