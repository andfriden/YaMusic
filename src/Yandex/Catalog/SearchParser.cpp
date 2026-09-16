#include "SearchParser.h"
#include "../Parsers.h"
#include <QJsonArray>
#include <QJsonObject>

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

    // --- Tracks ---

    const QJsonObject tracksObject =
        resultObject.value("tracks").toObject();

    if (!tracksObject.isEmpty()) {
        searchResults.total =
            tracksObject.value("total").toInt();
        searchResults.page =
            tracksObject.value("page").toInt();
        searchResults.perPage =
            tracksObject.value("perPage").toInt();

        const QJsonArray trackArray =
            tracksObject.value("results").toArray();

        for (const QJsonValue &trackValue : trackArray) {
            const QJsonObject trackObject =
                trackValue.toObject();

            Track track = parseTrack(trackObject);
            searchResults.tracks.append(track);
        }
    }

    // --- Artists ---

    const QJsonObject artistsObject =
        resultObject.value("artists").toObject();

    if (!artistsObject.isEmpty()) {
        const QJsonArray artistArray =
            artistsObject.value("results").toArray();

        for (const QJsonValue &artistValue : artistArray) {
            const QJsonObject artistObject =
                artistValue.toObject();

            Artist artist = parseArtist(artistObject);
            searchResults.artists.append(artist);
        }
    }

    // --- Albums ---

    const QJsonObject albumsObject =
        resultObject.value("albums").toObject();

    if (!albumsObject.isEmpty()) {
        const QJsonArray albumArray =
            albumsObject.value("results").toArray();

        for (const QJsonValue &albumValue : albumArray) {
            const QJsonObject albumObject =
                albumValue.toObject();

            Album album = parseAlbum(albumObject);
            searchResults.albums.append(album);
        }
    }

    // --- Playlists ---

    const QJsonObject playlistsObject =
        resultObject.value("playlists").toObject();

    if (!playlistsObject.isEmpty()) {
        const QJsonArray playlistArray =
            playlistsObject.value("results").toArray();

        for (const QJsonValue &playlistValue : playlistArray) {
            const QJsonObject playlistObject =
                playlistValue.toObject();

            PersonalPlaylist playlist;
            playlist.id = QString::number(
                playlistObject.value("uid").toInt());
            playlist.uid = playlist.id;
            playlist.kind =
                playlistObject.value("kind").toInt();
            playlist.title =
                playlistObject.value("title").toString();
            playlist.description =
                playlistObject.value("description").toString();
            playlist.coverUri =
                playlistObject.value("coverUri").toString();
            playlist.trackCount =
                playlistObject.value("trackCount").toInt();

            searchResults.playlists.append(playlist);
        }
    }

    return searchResults;
}