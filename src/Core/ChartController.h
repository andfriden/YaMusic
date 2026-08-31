#pragma once

#include <QObject>
#include <QString>

#include "../Yandex/Catalog/ChartModel.h"


class ChartService;
class PlaybackController;


class ChartController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool loading
        READ isLoading
        NOTIFY loadingChanged)

    Q_PROPERTY(
        ChartModel *russiaModel
        READ russiaModel
        CONSTANT)

    Q_PROPERTY(
        ChartModel *worldModel
        READ worldModel
        CONSTANT)


public:

    explicit ChartController(
        ChartService *chartService,
        PlaybackController *playbackController,
        QObject *parent = nullptr);


    void loadChart(
        const QString &chartType);


    Q_INVOKABLE void loadCharts();


    Q_INVOKABLE void selectTrack(
        const QString &chartType,
        int index);


    ChartModel *
    russiaModel() const;


    ChartModel *
    worldModel() const;


    bool
    isLoading() const;


    signals:

        void loadingChanged();

    void chartsChanged();

    void statusChanged(
        const QString &message);


private:

    ChartService *
        m_chartService = nullptr;


    PlaybackController *
        m_playbackController = nullptr;


    ChartModel *
        m_russiaModel = nullptr;


    ChartModel *
        m_worldModel = nullptr;


    bool m_loading = false;

    int m_pendingCharts = 0;
};