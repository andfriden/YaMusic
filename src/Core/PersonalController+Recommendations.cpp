#include "PersonalController.h"

#include <QDebug>


void PersonalController::connectRecommendations()
{
    if (m_personalLanding == nullptr)
    {
        return;
    }


    connect(
        m_personalLanding,
        &PersonalLanding::personalPlaylistsReceived,
        this,
        [this](
            const QList<PersonalPlaylist> &playlists)
        {
            if (m_personalPlaylistsModel == nullptr)
            {
                return;
            }


            m_personalPlaylistsModel
                ->setPlaylists(
                    playlists);


            qDebug()
                << "Персональных плейлистов:"
                << playlists.size();


            for (
                const PersonalPlaylist &playlist :
                playlists)
            {
                qDebug()
                    << "Плейлист:"
                    << playlist.title
                    << "| uid:"
                    << playlist.uid
                    << "| kind:"
                    << playlist.kind
                    << "| tracks:"
                    << playlist.trackCount;
            }
        });


    connect(
        m_personalLanding,
        &PersonalLanding::loaded,
        this,
        [this](
            const QList<PersonalLandingSection> &sections)
        {
            m_loadingRecommendations = false;

            emit loadingRecommendationsChanged();


            if (m_personalPlaylistsModel != nullptr)
            {
                m_personalPlaylistsModel
                    ->setSections(
                        sections);
            }


            qDebug()
                << "Рекомендации: секций"
                << sections.size();


            for (
                const PersonalLandingSection &section :
                sections)
            {
                qDebug()
                    << "SECTION:"
                    << section.title
                    << "| type:"
                    << section.type
                    << "| items:"
                    << section.items.size();


                for (
                    const PersonalLandingItem &item :
                    section.items)
                {
                    qDebug()
                        << "  ITEM:"
                        << item.id
                        << "| type:"
                        << item.type;
                }
            }


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
            m_loadingRecommendations = false;

            emit loadingRecommendationsChanged();


            if (m_personalPlaylistsModel != nullptr)
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


void PersonalController::loadRecommendations()
{
    if (m_loadingRecommendations)
    {
        return;
    }


    if (m_personalLanding == nullptr)
    {
        return;
    }


    m_loadingRecommendations = true;

    emit loadingRecommendationsChanged();


    if (m_personalPlaylistsModel != nullptr)
    {
        m_personalPlaylistsModel
            ->clear();
    }


    emit statusChanged(
        "Загрузка рекомендаций...");


    m_personalLanding
        ->load();
}


void PersonalController::selectPersonalPlaylist(
    int index)
{
    if (m_personalPlaylistsModel == nullptr)
    {
        return;
    }


    const PersonalPlaylist playlist =
        m_personalPlaylistsModel
            ->playlistAt(index);


    if (
        playlist.uid.isEmpty() ||
        playlist.kind <= 0
    )
    {
        emit statusChanged(
            "Некорректный плейлист");

        return;
    }


    m_myWaveQueueActive = false;

    m_waitingForMoreMyWave = false;


    emit personalPlaylistSelected(
        playlist);
}