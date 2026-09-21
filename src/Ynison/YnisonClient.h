#pragma once

#include "WebSocketClient.h"

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QTimer>

// Ynison-клиент: двухфазный WebSocket (redirector → state service).
//
// Протокол исследован по реализациям:
//  - MarshalX/yandex-music-api (ynison)
//  - trudenboy/ma-provider-yandex-ynison
//  - flxxxxddd/yamuse
//
// Отправка истории прослушивания:
//  - сервер пишет историю из потока update_playing_status (не-paused,
//    растущий progress) от активного устройства-плеера по долгоживущему
//    соединению; в конце трека — финальный progress=duration и
//    update_player_state со сдвигом индекса.
//  - все числовые поля (progress_ms/duration_ms/version/timestamp_ms)
//    сериализуются строками, иначе сервер отвечает 500 и рвёт сокет.
//  - progress не должен превышать duration (иначе 400030001 + разрыв).
//  - device_id стабильный между реконнектами; keepalive — WS ping 20с;
//    реконнект с экспоненциальным backoff.
class YnisonClient : public QObject {
  Q_OBJECT

public:
  explicit YnisonClient(QObject *parent = nullptr);

  void setToken(const QString &token);
  void setDeviceId(const QString &deviceId);

  bool isConnected() const;
  QString deviceId() const;

public slots:
  void connectToYnison();
  void disconnectFromYnison();

  // Внешние апдейты состояния (вызываются из YnisonReporter).
  void sendFullState(const QJsonObject &playerState);
  void sendPlayerState(const QJsonObject &playerState);
  void sendPlayingStatus(
      qint64 progressMs,
      qint64 durationMs,
      bool paused);
  void sendActiveDevice();

signals:
  void connectedChanged(bool connected);
  void stateReceived(const QJsonObject &state);

private:
  void beginRedirect();
  void beginState(const QString &host,
                  const QString &ticket,
                  const QString &sessionId);
  void handleRedirectMessage(const QString &message);
  void handleStateMessage(const QString &message);
  void onStateSocketConnected();
  void scheduleReconnect();
  void restartKeepalive();
  void stopKeepalive();

  QHash<QByteArray, QByteArray> buildHeaders(
      const QString &ticket = {},
      const QString &sessionId = {}) const;

  QJsonObject buildDevice() const;
  QJsonObject makeVersion() const;
  QJsonObject buildInitialState() const;

  WebSocketClient m_redirectSocket;
  WebSocketClient *m_stateSocket = nullptr;

  QString m_token;
  QString m_deviceId;
  QString m_redirectHost;
  QString m_redirectTicket;
  QString m_sessionId;

  bool m_connected = false;
  int m_reconnectAttempt = 0;
  QTimer m_reconnectTimer;
  QTimer m_keepaliveTimer;
};