#include "ChartController.h"

#include "../Playback/PlaybackController.h"
#include "../Yandex/Catalog/ChartService.h"


ChartController::ChartController(
    ChartService *chartService,
    PlaybackController *playbackController,
    QObject *parent)
    : QObject(parent)
    , m_chartService(chartService)
    , m_playbackController(playbackController)
    , m_russiaModel(new ChartModel(this))
    , m_worldModel(new ChartModel(this))
{
    if (
        m_chartService == nullptr
    )
    {
        return;
    }


    connect(
        m_chartService,
        &ChartService::chartReceived,
        this,
        [this](
            const QList<Track> &tracks,
            const QString &type)
        {
            if (
                type == "russia"
            )
            {
                m_russiaModel->setTracks(
                    tracks);
            }
            else if (
                type == "world"
            )
            {
                m_worldModel->setTracks(
                    tracks);
            }


            if (
                m_pendingCharts > 0
            )
            {
                --m_pendingCharts;
            }


            if (
                m_pendingCharts == 0
            )
            {
                m_loading = false;

                emit loadingChanged();
                emit chartsChanged();
            }
        });


    connect(
        m_chartService,
        &ChartService::errorOccurred,
        this,
        [this](
            const QString &message)
        {
            m_loading = false;
            m_pendingCharts = 0;

            emit loadingChanged();

            emit statusChanged(
                QString(
                    "Ошибка чарта: %1")
                    .arg(message));
        });
}


void ChartController::loadChart(
    const QString &chartType)
{
    if (
        m_chartService == nullptr
    )
    {
        return;
    }


    const QString type =
        chartType
            .trimmed()
            .toLower();


    if (
        type != "russia" &&
        type != "world"
    )
    {
        emit statusChanged(
            "Некорректный тип чарта");

        return;
    }


    m_loading = true;
    m_pendingCharts = 1;

    emit loadingChanged();


    if (
        type == "russia"
    )
    {
        m_russiaModel->clear();
    }
    else
    {
        m_worldModel->clear();
    }


    m_chartService->loadChart(
        type);
}


void ChartController::loadCharts()
{
    if (
        m_chartService == nullptr
    )
    {
        return;
    }


    m_loading = true;
    m_pendingCharts = 2;

    emit loadingChanged();


    m_russiaModel->clear();
    m_worldModel->clear();


    m_chartService->loadChart(
        "russia");

    m_chartService->loadChart(
        "world");
}


void ChartController::selectTrack(
    const QString &chartType,
    int index)
{
    if (
        m_playbackController == nullptr
    )
    {
        return;
    }


    ChartModel *model = nullptr;


    if (
        chartType == "russia"
    )
    {
        model =
            m_russiaModel;
    }
    else if (
        chartType == "world"
    )
    {
        model =
            m_worldModel;
    }


    if (
        model == nullptr
    )
    {
        return;
    }


    const Track track =
        model->trackAt(index);


    if (
        track.id.isEmpty()
    )
    {
        return;
    }


    m_playbackController->playTrack(
        track);
}


ChartModel *
ChartController::russiaModel() const
{
    return m_russiaModel;
}


ChartModel *
ChartController::worldModel() const
{
    return m_worldModel;
}


bool
ChartController::isLoading() const
{
    return m_loading;
}