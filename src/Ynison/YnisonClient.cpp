#include "YnisonClient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QUuid>

#include <algorithm>

namespace {

constexpr auto RedirectService =
    "https://ynison.music.yandex.ru/"
    "redirector.YnisonRedirectService/GetRedirectToYnison";
constexpr auto StateServicePath =
    "/ynison_state.YnisonStateService/PutYnisonState";
constexpr auto Origin = "https://music.yandex.ru";

// Keepalive ping на уровне WebSocket, как в yamuse.
constexpr int KeepaliveIntervalMs = 20 * 1000;

// Экспоненциальный backoff реконнекта 1 → 30 c, как в yamuse.
constexpr int MaxReconnectDelayMs = 30 * 1000;

QString uuid() {
  return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString nowMs() {
  return QString::number(
      QDateTime::currentMSecsSinceEpoch());
}

} // namespace

YnisonClient::YnisonClient(QObject *parent)
    : QObject(parent),
      m_redirectSocket(this),
      m_stateSocket(nullptr),
      m_reconnectTimer(this),
      m_keepaliveTimer(this) {
  m_reconnectTimer.setSingleShot(true);
  connect(&m_reconnectTimer, &QTimer::timeout, this,
          &YnisonClient::connectToYnison);

  m_keepaliveTimer.setInterval(KeepaliveIntervalMs);
  connect(&m_keepaliveTimer, &QTimer::timeout, this, [this]() {
    if (m_stateSocket)
      m_stateSocket->sendPing();
  });

  connect(&m_redirectSocket, &WebSocketClient::connected, this, [this]() {
    // Редиректор сразу пришлёт один JSON-фрейм с host/ticket.
  });

  connect(&m_redirectSocket, &WebSocketClient::textMessageReceived, this,
          &YnisonClient::handleRedirectMessage);

  connect(&m_redirectSocket, &WebSocketClient::errorOccurred, this, [this](const QString &) {
    scheduleReconnect();
  });
}

void YnisonClient::setToken(const QString &token) {
  m_token = token.trimmed();
}

void YnisonClient::setDeviceId(const QString &deviceId) {
  m_deviceId = deviceId.trimmed();
}

bool YnisonClient::isConnected() const {
  return m_connected;
}

QString YnisonClient::deviceId() const {
  return m_deviceId;
}

void YnisonClient::connectToYnison() {
  if (m_token.isEmpty())
    return;

  if (m_deviceId.isEmpty()) {
    // Стабильный device_id: 16 hex-символов, как в ma-provider.
    QByteArray bytes(8, Qt::Uninitialized);
    for (int i = 0; i < bytes.size(); ++i)
      bytes[i] = static_cast<char>(
          QRandomGenerator::global()->generate() & 0xFF);
    m_deviceId = QString::fromLatin1(bytes.toHex());
  }

  m_connected = false;
  emit connectedChanged(false);

  beginRedirect();
}

void YnisonClient::disconnectFromYnison() {
  m_reconnectTimer.stop();
  stopKeepalive();

  m_redirectSocket.abort();

  if (m_stateSocket) {
    m_stateSocket->disconnect(this);
    m_stateSocket->abort();
    m_stateSocket->deleteLater();
    m_stateSocket = nullptr;
  }

  if (m_connected) {
    m_connected = false;
    emit connectedChanged(false);
  }
}

QHash<QByteArray, QByteArray>
YnisonClient::buildHeaders(
    const QString &ticket,
    const QString &sessionId) const {
  QJsonObject deviceInfo;
  deviceInfo.insert("app_name", "YandexMusicAndroid/24023621");
  deviceInfo.insert("type", 2);

  QJsonObject proto;
  proto.insert("Ynison-Device-Id", m_deviceId);
  proto.insert("Ynison-Device-Info",
               QString::fromUtf8(
                   QJsonDocument(deviceInfo).toJson(
                       QJsonDocument::Compact)));

  if (!ticket.isEmpty())
    proto.insert("Ynison-Redirect-Ticket", ticket);
  if (!sessionId.isEmpty())
    proto.insert("Ynison-Session-Id", sessionId);

  const QByteArray protocol =
      "Bearer, v2, " +
      QJsonDocument(proto).toJson(
          QJsonDocument::Compact);

  QHash<QByteArray, QByteArray> headers;
  headers.insert("Authorization",
                 QByteArray("OAuth ") + m_token.toUtf8());
  headers.insert("Origin", Origin);
  headers.insert("Sec-WebSocket-Protocol", protocol);
  return headers;
}

void YnisonClient::beginRedirect() {
  m_redirectSocket.abort();
  m_redirectSocket.open(RedirectService, buildHeaders());
}

void YnisonClient::handleRedirectMessage(const QString &message) {
  const QJsonDocument doc =
      QJsonDocument::fromJson(message.toUtf8());
  if (!doc.isObject())
    return;

  const QJsonObject obj = doc.object();

  m_redirectHost =
      obj.value("host").toString().trimmed();
  m_redirectTicket =
      obj.value("redirect_ticket").toString().trimmed();
  m_sessionId =
      QString::number(
          obj.value("session_id").toVariant().toLongLong());

  if (m_redirectHost.isEmpty() ||
      m_redirectTicket.isEmpty()) {
    // Пустой тикет = невалидный токен, как в ma-provider.
    scheduleReconnect();
    return;
  }

  beginState(m_redirectHost, m_redirectTicket, m_sessionId);
}

void YnisonClient::beginState(
    const QString &host,
    const QString &ticket,
    const QString &sessionId) {
  if (m_stateSocket) {
    m_stateSocket->disconnect(this);
    m_stateSocket->deleteLater();
    m_stateSocket = nullptr;
  }

  auto *socket = new WebSocketClient(this);
  m_stateSocket = socket;

  connect(socket, &WebSocketClient::connected, this,
          &YnisonClient::onStateSocketConnected);

  connect(socket, &WebSocketClient::textMessageReceived, this,
          &YnisonClient::handleStateMessage);

  connect(socket, &WebSocketClient::disconnected, this, [this]() {
    if (m_connected) {
      m_connected = false;
      emit connectedChanged(false);
    }
    scheduleReconnect();
  });

  connect(socket, &WebSocketClient::errorOccurred, this,
          [this](const QString &) {
            if (m_connected) {
              m_connected = false;
              emit connectedChanged(false);
            }
            scheduleReconnect();
          });

  const QString url =
      QStringLiteral("wss://%1%2")
          .arg(host)
          .arg(StateServicePath);

  socket->open(url, buildHeaders(ticket, sessionId));
}

void YnisonClient::onStateSocketConnected() {
  m_connected = true;
  m_reconnectAttempt = 0;
  emit connectedChanged(true);

  restartKeepalive();

  // Регистрируем устройство как плеер (can_be_player). Активность
  // захватывается отдельно через update_active_device при старте
  // реального воспроизведения — сервер отклоняет регистрацию
  // активного устройства с пустой очередью (400030001).
  sendFullState(buildInitialState());
}

void YnisonClient::handleStateMessage(const QString &message) {
  const QJsonDocument doc =
      QJsonDocument::fromJson(message.toUtf8());
  if (!doc.isObject())
    return;

  const QJsonObject obj = doc.object();

  if (obj.contains("error")) {
    scheduleReconnect();
    return;
  }

  emit stateReceived(obj);
}

void YnisonClient::sendFullState(const QJsonObject &playerState) {
  if (!m_stateSocket || !m_connected)
    return;

  QJsonObject update;
  update.insert("update_full_state", QJsonObject{
      {"player_state", playerState},
      {"device", buildDevice()},
      {"is_currently_active", false},
  });

  QJsonObject envelope = update;
  envelope.insert("rid", uuid());
  envelope.insert("player_action_timestamp_ms", nowMs());
  envelope.insert("activity_interception_type", "DO_NOT_INTERCEPT_BY_DEFAULT");

  m_stateSocket->sendText(
      QString::fromUtf8(
          QJsonDocument(envelope).toJson(
              QJsonDocument::Compact)));
}

void YnisonClient::sendPlayerState(const QJsonObject &playerState) {
  if (!m_stateSocket || !m_connected)
    return;

  QJsonObject envelope = QJsonObject{
      {"update_player_state", QJsonObject{
          {"player_state", playerState},
      }},
  };
  envelope.insert("rid", uuid());
  envelope.insert("player_action_timestamp_ms", nowMs());
  envelope.insert("activity_interception_type", "DO_NOT_INTERCEPT_BY_DEFAULT");

  m_stateSocket->sendText(
      QString::fromUtf8(
          QJsonDocument(envelope).toJson(
              QJsonDocument::Compact)));
}

void YnisonClient::sendPlayingStatus(
    qint64 progressMs,
    qint64 durationMs,
    bool paused) {
  if (!m_stateSocket || !m_connected)
    return;

  progressMs = std::clamp<qint64>(progressMs, 0, durationMs);
  durationMs = std::max<qint64>(0, durationMs);

  const QJsonObject playingStatus{
      {"progress_ms", QString::number(progressMs)},
      {"duration_ms", QString::number(durationMs)},
      {"paused", paused},
      {"playback_speed", 1.0},
  };

  const QJsonObject envelope{
      {"update_playing_status", QJsonObject{
          {"playing_status", playingStatus},
      }},
  };

  m_stateSocket->sendText(
      QString::fromUtf8(
          QJsonDocument(envelope).toJson(
              QJsonDocument::Compact)));
}

void YnisonClient::sendActiveDevice() {
  if (!m_stateSocket || !m_connected)
    return;

  const QJsonObject envelope{
      {"update_active_device", QJsonObject{
          {"device_id_optional", m_deviceId},
      }},
  };

  m_stateSocket->sendText(
      QString::fromUtf8(
          QJsonDocument(envelope).toJson(
              QJsonDocument::Compact)));
}

QJsonObject YnisonClient::buildDevice() const {
  return QJsonObject{
      {"info", QJsonObject{
          {"device_id", m_deviceId},
          {"title", "Яндекс Музыка"},
          {"type", "ANDROID"},
          {"app_name", "ru.yandex.music"},
          {"app_version", "2026.08.3"},
      }},
      {"capabilities", QJsonObject{
          {"can_be_player", true},
          {"can_be_remote_controller", false},
      }},
      {"is_shadow", false},
  };
}

QJsonObject YnisonClient::makeVersion() const {
  return QJsonObject{
      {"device_id", m_deviceId},
      {"version", QString::number(
           QDateTime::currentMSecsSinceEpoch() * 1000)},
      {"timestamp_ms", "0"},
  };
}

QJsonObject YnisonClient::buildInitialState() const {
  return QJsonObject{
      {"status", QJsonObject{
          {"paused", true},
          {"duration_ms", "0"},
          {"progress_ms", "0"},
          {"playback_speed", 1},
          {"version", makeVersion()},
      }},
      {"player_queue", QJsonObject{
          {"current_playable_index", -1},
          {"entity_id", ""},
          {"entity_type", "VARIOUS"},
          {"playable_list", QJsonArray()},
          {"options", QJsonObject{
              {"repeat_mode", "NONE"},
          }},
          {"entity_context", "BASED_ON_ENTITY_BY_DEFAULT"},
          {"version", makeVersion()},
          {"from_optional", ""},
      }},
  };
}

void YnisonClient::scheduleReconnect() {
  if (!m_stateSocket && m_token.isEmpty()) {
    // Не пытаемся переподключаться без токена (дисконнект).
    return;
  }

  const int baseMs =
      std::min(1000 << m_reconnectAttempt, MaxReconnectDelayMs);
  m_reconnectAttempt =
      std::min(m_reconnectAttempt + 1, 6);

  // Лёгкий джиттер, как в ma-provider (±20%).
  const int jitter =
      static_cast<int>(baseMs * 0.2 *
                       (QRandomGenerator::global()->generateDouble() * 2 - 1));
  const int delayMs = std::max(500, baseMs + jitter);

  m_reconnectTimer.start(delayMs);
}

void YnisonClient::restartKeepalive() {
  m_keepaliveTimer.start();
}

void YnisonClient::stopKeepalive() {
  m_keepaliveTimer.stop();
}