#include "SearchParser.h"

#include <QJsonArray>
#include <QJsonObject>

// Parses the Yandex Music search response.
SearchResults SearchParser::parse(
    const QJsonObject &object)
{
    SearchResults searchResults;

    const QJsonObject resultObject =
        object.value("result").toObject();

    if (resultObject.isEmpty()) {
        return searchResults;
    }

    searchResults.query =
        resultObject.value("text").toString();

    const QJsonObject tracksObject =
        resultObject.value("tracks").toObject();

    if (tracksObject.isEmpty()) {
        return searchResults;
    }

    searchResults.page =
        tracksObject.value("page").toInt();

    searchResults.perPage =
        tracksObject.value("perPage").toInt();

    searchResults.total =
        tracksObject.value("total").toInt();

    const QJsonArray trackArray =
        tracksObject.value("results").toArray();

    for (const QJsonValue &trackValue : trackArray) {

        const QJsonObject trackObject =
            trackValue.toObject();

        Track track;

        track.id =
            QString::number(
                trackObject.value("id").toInteger());

        track.title =
            trackObject.value("title").toString();

        track.coverUri =
            trackObject.value("coverUri").toString();

        track.durationMs =
            trackObject.value("durationMs").toInt();

        const QJsonArray artistArray =
            trackObject.value("artists").toArray();

        for (const QJsonValue &artistValue : artistArray) {

            const QJsonObject artistObject =
                artistValue.toObject();

            Artist artist;

            artist.id =
                QString::number(
                    artistObject.value("id").toInteger());

            artist.name =
                artistObject.value("name").toString();

            track.artists.append(
                artist);
        }

        const QJsonArray albumArray =
            trackObject.value("albums").toArray();

        for (const QJsonValue &albumValue : albumArray) {

            const QJsonObject albumObject =
                albumValue.toObject();

            Album album;

            album.id =
                QString::number(
                    albumObject.value("id").toInteger());

            album.title =
                albumObject.value("title").toString();

            album.coverUri =
                albumObject.value("coverUri").toString();

            album.year =
                albumObject.value("year").toInt();

            track.albums.append(
                album);
        }

        searchResults.tracks.append(
            track);
    }

    return searchResults;
}