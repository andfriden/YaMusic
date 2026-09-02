#include "PersonalController.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>


// =============================================================
// Recommendations connections
// =============================================================

void PersonalController::connectRecommendations()
{
    if (
        m_personalLanding == nullptr
    )
    {
        return;
    }


    connect(
        m_personalLanding,
        &PersonalLanding::loaded,
        this,
        [this](
            const QList<PersonalLandingSection> &sections)
        {
            m_loadingRecommendations =
                false;

            emit loadingRecommendationsChanged();


            m_recommendationSections =
                sections;


            QList<PersonalPlaylist>
                playlists;


            QList<PersonalChartItem>
                chartItems;


            // =====================================================
            // Sections
            // =====================================================

            for (
                const PersonalLandingSection &section :
                sections
            )
            {
                // -------------------------------------------------
                // Playlists
                // -------------------------------------------------

                for (
                    const PersonalPlaylist &playlist :
                    section.playlists
                )
                {
                    if (
                        playlist.uid.isEmpty()
                    )
                    {
                        continue;
                    }


                    playlists.append(
                        playlist);
                }


                // -------------------------------------------------
                // Chart
                // -------------------------------------------------

                if (
                    section.type != "chart"
                )
                {
                    continue;
                }


                for (
                    const PersonalLandingItem &landingItem :
                    section.items
                )
                {
                    if (
                        landingItem.type !=
                            "chart-item"
                    )
                    {
                        continue;
                    }


                    const QJsonObject data =
                        landingItem.data;


                    const QJsonObject chart =
                        data
                            .value("chart")
                            .toObject();


                    const QJsonObject track =
                        data
                            .value("track")
                            .toObject();


                    if (
                        track.isEmpty()
                    )
                    {
                        continue;
                    }


                    PersonalChartItem item;


                    item.position =
                        chart
                            .value("position")
                            .toInt();


                    item.listeners =
                        chart
                            .value("listeners")
                            .toInt();


                    item.progress =
                        chart
                            .value("progress")
                            .toString();


                    item.shift =
                        chart
                            .value("shift")
                            .toInt();


                    item.id =
                        track
                            .value("id")
                            .toString();


                    if (
                        item.id.isEmpty()
                    )
                    {
                        item.id =
                            track
                                .value("realId")
                                .toString();
                    }


                    if (
                        item.id.isEmpty()
                    )
                    {
                        item.id =
                            landingItem.id;
                    }


                    item.title =
                        track
                            .value("title")
                            .toString();


                    item.coverUri =
                        track
                            .value("coverUri")
                            .toString();


                    item.durationMs =
                        track
                            .value("durationMs")
                            .toInteger();


                    const QJsonArray artists =
                        track
                            .value("artists")
                            .toArray();


                    for (
                        const QJsonValue &artistValue :
                        artists
                    )
                    {
                        if (
                            !artistValue.isObject()
                        )
                        {
                            continue;
                        }


                        const QString artistName =
                            artistValue
                                .toObject()
                                .value("name")
                                .toString();


                        if (
                            !artistName.isEmpty()
                        )
                        {
                            item.artists.append(
                                artistName);
                        }
                    }


                    if (
                        item.id.isEmpty() ||
                        item.title.isEmpty()
                    )
                    {
                        continue;
                    }


                    chartItems.append(
                        item);
                }
            }


            // =====================================================
            // Store playlists
            // =====================================================

            m_recommendationPlaylists =
                playlists;


            // =====================================================
            // Playlists model
            // =====================================================

            if (
                m_personalPlaylistsModel != nullptr
            )
            {
                m_personalPlaylistsModel
                    ->setSections(
                        sections);
            }


            // =====================================================
            // Chart model
            // =====================================================

            if (
                m_chartModel != nullptr
            )
            {
                m_chartModel
                    ->setItems(
                        chartItems);
            }


            emit recommendationsLoaded();


            emit statusChanged(
                QString(
                    "Загружено блоков рекомендаций: %1")
                .arg(
                    sections.size()));
        });


    // =============================================================
    // Error
    // =============================================================

    connect(
        m_personalLanding,
        &PersonalLanding::errorOccurred,
        this,
        [this](
            const QString &message)
        {
            m_loadingRecommendations =
                false;

            emit loadingRecommendationsChanged();


            m_recommendationSections.clear();

            m_recommendationPlaylists.clear();


            if (
                m_personalPlaylistsModel != nullptr
            )
            {
                m_personalPlaylistsModel
                    ->clear();
            }


            if (
                m_chartModel != nullptr
            )
            {
                m_chartModel
                    ->clear();
            }


            emit statusChanged(
                QString(
                    "Ошибка загрузки рекомендаций: %1")
                .arg(
                    message));
        });
}


// =============================================================
// Load recommendations
// =============================================================

void PersonalController::loadRecommendations()
{
    if (
        m_loadingRecommendations
    )
    {
        return;
    }


    if (
        m_personalLanding == nullptr
    )
    {
        return;
    }


    m_loadingRecommendations =
        true;

    emit loadingRecommendationsChanged();


    m_recommendationSections.clear();

    m_recommendationPlaylists.clear();


    if (
        m_personalPlaylistsModel != nullptr
    )
    {
        m_personalPlaylistsModel
            ->clear();
    }


    if (
        m_chartModel != nullptr
    )
    {
        m_chartModel
            ->clear();
    }


    emit statusChanged(
        "Загрузка рекомендаций...");


    m_personalLanding
        ->load();


    // =============================================================
    // New playlists
    // =============================================================

    if (
        m_newPlaylistsService != nullptr
    )
    {
        qDebug()
            << "PersonalController:"
            << "loading new playlists";


        m_newPlaylistsService
            ->load();
    }
}


// =============================================================
// Select personal playlist
// =============================================================

void PersonalController::selectPersonalPlaylist(
    const QString &uid,
    int kind)
{
    const QString playlistUid =
        uid.trimmed();


    if (
        playlistUid.isEmpty() ||
        kind <= 0
    )
    {
        emit statusChanged(
            "Некорректный плейлист");

        return;
    }


    PersonalPlaylist selectedPlaylist;


    for (
        const PersonalPlaylist &playlist :
        m_recommendationPlaylists
    )
    {
        if (
            playlist.uid == playlistUid &&
            playlist.kind == kind
        )
        {
            selectedPlaylist =
                playlist;

            break;
        }
    }


    if (
        selectedPlaylist.uid.isEmpty()
    )
    {
        for (
            const PersonalLandingSection &section :
            m_recommendationSections
        )
        {
            for (
                const PersonalPlaylist &playlist :
                section.playlists
            )
            {
                if (
                    playlist.uid == playlistUid &&
                    playlist.kind == kind
                )
                {
                    selectedPlaylist =
                        playlist;

                    break;
                }
            }


            if (
                !selectedPlaylist.uid.isEmpty()
            )
            {
                break;
            }
        }
    }


    if (
        selectedPlaylist.uid.isEmpty()
    )
    {
        emit statusChanged(
            "Плейлист не найден");

        return;
    }


    m_myWaveQueueActive =
        false;

    m_waitingForMoreMyWave =
        false;


    emit personalPlaylistSelected(
        selectedPlaylist);
}