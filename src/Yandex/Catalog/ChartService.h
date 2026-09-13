#pragma once

#include <QList>
#include <QString>
#include "../../Models/Track.h"
#include "../YandexServiceBase.h"

class ChartService : public YandexServiceBase
{
    Q_OBJECT

public:

    explicit ChartService(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void loadChart(
        const QString &chartType);

    signals:

        void chartReceived(
            const QList<Track> &tracks,
            const QString &chartType);

    void errorOccurred(
        const QString &message);
};