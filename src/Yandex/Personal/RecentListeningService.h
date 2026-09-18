#pragma once

#include "../../Models/Track.h"
#include "../YandexServiceBase.h"
#include <QDateTime>
#include <QHash>
#include <QJsonObject>
#include <QList>
#include <QString>

struct RecentListeningTrack {
  QString trackId;
  QDateTime timestamp;
};

class RecentListeningService : public YandexServiceBase {
  Q_OBJECT

public:
  explicit RecentListeningService(YandexAuth *auth, QObject *parent = nullptr);
  void setUserId(const QString &userId);
  void load(int trackCount = 50, int contextCount = 10);

signals:
  void loadingChanged(bool loading);
  void tracksReceived(const QList<Track> &tracks);
  void errorOccurred(const QString &message);

private:
  QList<RecentListeningTrack> parseResponse(const QByteArray &data) const;
  RecentListeningTrack parseListenedTrack(const QJsonObject &object) const;
  void resolveTracks(const QList<RecentListeningTrack> &references);

  QString m_userId;
  QList<RecentListeningTrack> m_pendingReferences;
  bool m_loading = false;
};