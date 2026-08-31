#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "../../Models/Track.h"


class YandexAuth;
class YandexClient;


class ChartService : public QObject
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


private:

    YandexAuth *
        m_auth = nullptr;


    YandexClient *
        m_yandexClient = nullptr;
};