#pragma once

#include "SystemMediaControls.h"

#include <QImage>

class QNetworkAccessManager;
class QNetworkReply;

// macOS: MPRemoteCommandCenter + MPNowPlayingInfoCenter.
// Публикует метаданные и обрабатывает команды из Control Center.
class MediaControlsMacOS : public SystemMediaControls {
  Q_OBJECT

public:
  explicit MediaControlsMacOS(QObject *parent = nullptr);
  ~MediaControlsMacOS() override;

protected:
  void platformSetEnabled(bool enabled) override;
  void platformSetMetadata(const Metadata &metadata) override;
  void platformSetPlaybackStatus(PlaybackStatus status) override;
  void platformSetPosition(qint64 positionMs) override;
  void platformSetDuration(qint64 durationMs) override;

private:
  void updateNowPlayingInfo();
  void startArtworkFetch(const QString &coverUri);
  void handleArtworkReply(QNetworkReply *reply, const QString &requestedUri);

  static QString fullCoverUrl(const QString &coverUri);

  class Impl;
  Impl *d = nullptr;

  QNetworkAccessManager *m_network = nullptr;
  QString m_pendingArtworkUri;
  QImage m_artworkImage;
  QString m_lastArtworkTrackId;
};