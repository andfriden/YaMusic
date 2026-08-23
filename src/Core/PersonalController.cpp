#include "PersonalController.h"

#include "../Playback/PlaybackController.h"
#include "../Player/PlayerService.h"
#include "../Yandex/Personal/YandexPersonal.h"

#include <QDebug>

PersonalController::PersonalController(
    YandexPersonal *yandexPersonal,
    PersonalLanding *personalLanding,
    RecentListeningService *recentListeningService,
    PlaybackController *playbackController,
    PlayerService *playerService,
    QObject *parent)
    : QObject(parent)
    , m_yandexPersonal(
          yandexPersonal)
    , m_personalLanding(
          personalLanding)
    , m_recentListeningService(
          recentListeningService)
    , m_playbackController(
          playbackController)
    , m_playerService(
          playerService)
    , m_myWaveModel(
          new MyWaveModel(this))
    , m_personalPlaylistsModel(
          new PersonalPlaylistsModel(this))
    , m_recentListeningModel(
          new RecentListeningModel(this))
{
    /*
     * My Wave
     */

    if (
        m_yandexPersonal != nullptr
    ) {

        connect(
            m_yandexPersonal,
            &YandexPersonal::myWaveReceived,
            this,
            [this](
                const QList<Track> &tracks,
                const QString &batchId) {

                const QString trimmedBatchId =
                    batchId.trimmed();

                for (
                    const Track &track :
                    tracks
                ) {

                    if (
                        !track.id.isEmpty() &&
                        !trimmedBatchId.isEmpty()
                    ) {

                        m_myWaveTrackBatches.insert(
                            track.id,
                            trimmedBatchId);
                    }
                }

                const bool firstLoad =
                    !m_loadingMoreMyWave;

                if (
                    m_loadingMoreMyWave
                ) {

                    m_loadingMoreMyWave =
                        false;

                    emit loadingMoreMyWaveChanged();
                }

                if (firstLoad) {

                    m_loadingMyWave =
                        false;

                    emit loadingMyWaveChanged();

                    m_myWaveModel
                        ->setTracks(
                            tracks);

                    qDebug()
                        << "My Wave tracks:"
                        << tracks.size()
                        << "batch:"
                        << trimmedBatchId;

                    emit statusChanged(
                        QString(
                            "Моя волна: %1 треков")
                            .arg(
                                m_myWaveModel
                                    ->count()));

                } else {

                    m_myWaveModel
                        ->appendTracks(
                            tracks);

                    qDebug()
                        << "More My Wave tracks:"
                        << tracks.size()
                        << "batch:"
                        << trimmedBatchId;

                    emit statusChanged(
                        QString(
                            "Загружено ещё %1 треков")
                            .arg(
                                tracks.size()));
                }
            });

        connect(
            m_yandexPersonal,
            &YandexPersonal::errorOccurred,
            this,
            [this](
                const QString &message) {

                if (
                    m_loadingMyWave
                ) {

                    m_loadingMyWave =
                        false;

                    emit loadingMyWaveChanged();
                }

                if (
                    m_loadingMoreMyWave
                ) {

                    m_loadingMoreMyWave =
                        false;

                    emit loadingMoreMyWaveChanged();
                }

                if (
                    m_loadingRecommendations
                ) {

                    m_loadingRecommendations =
                        false;

                    emit loadingRecommendationsChanged();
                }

                qDebug()
                    << "Personal error:"
                    << message;

                emit statusChanged(
                    message);
            });

        connect(
            m_yandexPersonal,
            &YandexPersonal::feedbackSent,
            this,
            [this](
                const QString &event) {

                qDebug()
                    << "My Wave feedback sent:"
                    << event;
            });

        connect(
            m_yandexPersonal,
            &YandexPersonal::feedbackError,
            this,
            [this](
                const QString &message) {

                qDebug()
                    << "My Wave feedback error:"
                    << message;
            });
    }

    /*
     * Personal Landing
     */

    if (
        m_personalLanding != nullptr
    ) {

        connect(
            m_personalLanding,
            &PersonalLanding::personalPlaylistsReceived,
            this,
            [this](
                const QList<PersonalPlaylist>
                    &playlists) {

                m_personalPlaylistsModel
                    ->setPlaylists(
                        playlists);

                qDebug()
                    << "Персональных плейлистов:"
                    << playlists.size();

                for (
                    const PersonalPlaylist &playlist :
                    playlists
                ) {

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
                const QList<PersonalLandingSection>
                    &sections) {

                m_loadingRecommendations =
                    false;

                emit loadingRecommendationsChanged();

                qDebug()
                    << "Рекомендации: секций"
                    << sections.size();

                for (
                    const PersonalLandingSection &section :
                    sections
                ) {

                    qDebug()
                        << "Секция:"
                        << section.title
                        << "| type:"
                        << section.type
                        << "| items:"
                        << section.items.size();
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
                const QString &message) {

                m_loadingRecommendations =
                    false;

                emit loadingRecommendationsChanged();

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

    /*
     * Recently Played
     */

    if (
        m_recentListeningService != nullptr
    ) {

        connect(
            m_recentListeningService,
            &RecentListeningService::loadingChanged,
            this,
            [this](
                bool loading) {

                Q_UNUSED(this)

                qDebug()
                    << "Recent listening loading:"
                    << loading;
            });

        connect(
            m_recentListeningService,
            &RecentListeningService::tracksReceived,
            this,
            [this](
                const QList<Track> &tracks) {

                m_recentListeningModel
                    ->setTracks(
                        tracks);

                qDebug()
                    << "Recently listened tracks:"
                    << tracks.size();

                for (
                    const Track &track :
                    tracks
                ) {

                    QString artistName;

                    if (
                        !track.artists.isEmpty()
                    ) {

                        artistName =
                            track.artists
                                .first()
                                .name;
                    }

                    qDebug()
                        << "Recently:"
                        << track.title
                        << "| artist:"
                        << artistName
                        << "| id:"
                        << track.id;
                }

                emit statusChanged(
                    QString(
                        "Недавно слушали: %1 треков")
                        .arg(
                            tracks.size()));
            });

        connect(
            m_recentListeningService,
            &RecentListeningService::errorOccurred,
            this,
            [this](
                const QString &message) {

                qDebug()
                    << "Recent listening error:"
                    << message;

                emit statusChanged(
                    QString(
                        "Ошибка истории прослушивания: %1")
                        .arg(
                            message));
            });
    }

    /*
     * My Wave feedback
     */

    if (
        m_playerService != nullptr
    ) {

        connect(
            m_playerService,
            &PlayerService::playbackStarted,
            this,
            [this]() {

                if (
                    m_currentMyWaveTrackId
                        .isEmpty()
                ) {
                    return;
                }

                if (
                    m_myWaveTrackStarted
                ) {
                    return;
                }

                if (
                    batchIdForTrack(
                        m_currentMyWaveTrackId)
                        .isEmpty()
                ) {
                    return;
                }

                m_myWaveTrackStarted =
                    true;

                sendMyWaveFeedback(
                    "trackStarted",
                    m_currentMyWaveTrackId);
            });

        connect(
            m_playerService,
            &PlayerService::playbackFinished,
            this,
            [this]() {

                if (
                    m_currentMyWaveTrackId
                        .isEmpty() ||
                    !m_myWaveTrackStarted
                ) {
                    return;
                }

                const qint64 playedSeconds =
                    m_playerService
                        ->position() /
                    1000;

                sendMyWaveFeedback(
                    "trackFinished",
                    m_currentMyWaveTrackId,
                    playedSeconds);

                m_currentMyWaveTrackId
                    .clear();

                m_myWaveTrackStarted =
                    false;
            });
    }
}

void PersonalController::loadMyWave()
{
    if (
        m_loadingMyWave ||
        m_loadingMoreMyWave
    ) {
        return;
    }

    if (
        m_yandexPersonal == nullptr
    ) {
        return;
    }

    m_loadingMyWave =
        true;

    emit loadingMyWaveChanged();

    m_myWaveTrackBatches.clear();

    emit statusChanged(
        "Загрузка моей волны...");

    m_yandexPersonal
        ->loadMyWave();
}

void PersonalController::loadMoreMyWave()
{
    if (
        m_loadingMyWave ||
        m_loadingMoreMyWave
    ) {
        return;
    }

    if (
        m_yandexPersonal == nullptr
    ) {
        return;
    }

    const Track lastTrack =
        m_myWaveModel
            ->lastTrack();

    if (
        lastTrack.id.isEmpty()
    ) {
        return;
    }

    m_loadingMoreMyWave =
        true;

    emit loadingMoreMyWaveChanged();

    emit statusChanged(
        "Загрузка следующей части моей волны...");

    m_yandexPersonal
        ->loadMoreMyWave(
            lastTrack.id);
}

void PersonalController::loadRecommendations()
{
    if (
        m_loadingRecommendations
    ) {
        return;
    }

    if (
        m_personalLanding == nullptr
    ) {
        return;
    }

    m_loadingRecommendations =
        true;

    emit loadingRecommendationsChanged();

    m_personalPlaylistsModel
        ->clear();

    /*
     * История грузится отдельно через
     * RecentListeningService после получения UID.
     */

    emit statusChanged(
        "Загрузка рекомендаций...");

    m_personalLanding
        ->load();
}

void PersonalController::selectMyWaveTrack(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    const Track track =
        m_myWaveModel
            ->trackAt(
                index);

    if (
        track.id.isEmpty()
    ) {

        emit statusChanged(
            "Некорректный трек моей волны");

        return;
    }

    if (
        !m_currentMyWaveTrackId.isEmpty() &&
        m_myWaveTrackStarted
    ) {

        const qint64 playedSeconds =
            m_playerService != nullptr
                ? m_playerService
                      ->position() /
                  1000
                : 0;

        sendMyWaveFeedback(
            "skip",
            m_currentMyWaveTrackId,
            playedSeconds);
    }

    m_currentMyWaveTrackId =
        track.id;

    m_myWaveTrackStarted =
        false;

    emit myWaveTrackSelected(
        track);

    m_playbackController
        ->playTrack(
            track);
}

void PersonalController::selectPersonalPlaylist(
    int index)
{
    const PersonalPlaylist playlist =
        m_personalPlaylistsModel
            ->playlistAt(
                index);

    if (
        playlist.uid.isEmpty() ||
        playlist.kind <= 0
    ) {

        emit statusChanged(
            "Некорректный плейлист");

        return;
    }

    emit personalPlaylistSelected(
        playlist);
}

void PersonalController::selectRecentListening(
    int index)
{
    if (
        m_playbackController == nullptr
    ) {
        return;
    }

    const Track track =
        m_recentListeningModel
            ->trackAt(
                index);

    if (
        track.id.isEmpty()
    ) {

        emit statusChanged(
            "Некорректный трек истории");

        return;
    }

    emit statusChanged(
        QString(
            "Выбран трек: %1")
            .arg(
                track.title));

    m_playbackController
        ->playTrack(
            track);
}

MyWaveModel *
PersonalController::myWaveModel() const
{
    return m_myWaveModel;
}

PersonalPlaylistsModel *
PersonalController::personalPlaylistsModel() const
{
    return m_personalPlaylistsModel;
}

RecentListeningModel *
PersonalController::recentListeningModel() const
{
    return m_recentListeningModel;
}

bool PersonalController::isLoadingMyWave() const
{
    return m_loadingMyWave;
}

bool PersonalController::isLoadingMoreMyWave() const
{
    return m_loadingMoreMyWave;
}

bool PersonalController::isLoadingRecommendations() const
{
    return m_loadingRecommendations;
}

QString PersonalController::batchIdForTrack(
    const QString &trackId) const
{
    return m_myWaveTrackBatches
        .value(
            trackId);
}

void PersonalController::sendMyWaveFeedback(
    const QString &event,
    const QString &trackId,
    qint64 totalPlayedSeconds)
{
    if (
        m_yandexPersonal == nullptr
    ) {
        return;
    }

    const QString batchId =
        batchIdForTrack(
            trackId);

    if (
        batchId.isEmpty()
    ) {

        qDebug()
            << "No batch ID for Wave track:"
            << trackId;

        return;
    }

    m_yandexPersonal
        ->sendMyWaveFeedback(
            event,
            trackId,
            batchId,
            totalPlayedSeconds);
}