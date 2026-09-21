#pragma once

#include "../../Models/Track.h"
#include "../YandexServiceBase.h"

#include <QList>
#include <QString>

class RecentListeningService : public YandexServiceBase {
    Q_OBJECT

  public:
    explicit RecentListeningService(YandexAuth *auth,
                                    QObject *parent = nullptr);

    void setUserId(const QString &userId);

    void load(int trackCount = 50, int contextCount = 10);

    signals:
      void loadingChanged(bool loading);
    void tracksReceived(const QList<Track> &tracks);
    void errorOccurred(const QString &message);

private:
    QList<Track> parseHistory(const QByteArray &data,
                              int trackCount) const;

    QString m_userId;
    bool m_loading = false;
};