#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QString>
#include "../Models/Track.h"

QString parseId(
    const QJsonObject &object);

QString parseCoverUri(
    const QJsonObject &object);

Artist parseArtist(
    const QJsonObject &object);

Album parseAlbum(
    const QJsonObject &object);

Track parseTrack(
    const QJsonObject &object);

QList<Track> parseTrackArray(
    const QJsonArray &array);

QJsonObject unwrapResult(
    const QJsonDocument &document);