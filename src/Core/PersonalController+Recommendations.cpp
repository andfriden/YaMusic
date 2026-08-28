#include "PersonalController.h"

#include <QDebug>


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


            for (
                const PersonalLandingSection &section :
                sections
            )
            {
                for (
                    const PersonalPlaylist &playlist :
                    section.playlists
                )
                {
                    playlists.append(
                        playlist);
                }
            }


            m_recommendationPlaylists =
                playlists;


            if (
                m_personalPlaylistsModel != nullptr
            )
            {
                m_personalPlaylistsModel
                    ->setSections(
                        sections);
            }


            qDebug()
                << "Рекомендации: секций"
                << sections.size();


            qDebug()
                << "Персональных плейлистов:"
                << playlists.size();


            emit recommendationsLoaded();


            emit statusChanged(
                QString(
                    "Загружено блоков рекомендаций: %1")
                .arg(
                    sections.size()));
        });


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


            qDebug()
                << "Recommendations error:"
                << message;


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


    emit statusChanged(
        "Загрузка рекомендаций...");


    m_personalLanding
        ->load();
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