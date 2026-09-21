#include "WebSocketClient.h"

#include <QCryptographicHash>
#include <QHash>
#include <QHostAddress>
#include <QRandomGenerator>
#include <QSslSocket>
#include <QUrl>

#include <QtEndian>

namespace {

// RFC 6455 opcodes.
constexpr quint8 OpContinuation = 0x0;
constexpr quint8 OpText = 0x1;
constexpr quint8 OpBinary = 0x2;
constexpr quint8 OpClose = 0x8;
constexpr quint8 OpPing = 0x9;
constexpr quint8 OpPong = 0xA;

QByteArray randomMask() {
  QByteArray mask(4, Qt::Uninitialized);
  for (int i = 0; i < 4; ++i)
    mask[i] = static_cast<char>(QRandomGenerator::global()->generate() & 0xFF);
  return mask;
}

QByteArray applyMask(const QByteArray &payload, const QByteArray &mask) {
  QByteArray result = payload;
  for (int i = 0; i < result.size(); ++i)
    result[i] = result[i] ^ mask[i % 4];
  return result;
}

} // namespace

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent), m_socket(new QSslSocket(this)) {
  connect(m_socket, &QSslSocket::connected, this, [this]() {
    // Рукопожатие отправляется вручную после установления TCP+TLS,
    // потому что QSslSocket не знает про WebSocket Upgrade.
    const QUrl url(m_url);
    const QString host =
        url.host() + (url.port() > 0 ? QStringLiteral(":%1").arg(url.port()) : QString());

    // Случайный nonce для Sec-WebSocket-Accept.
    QByteArray nonce(16, Qt::Uninitialized);
    for (int i = 0; i < nonce.size(); ++i)
      nonce[i] = static_cast<char>(QRandomGenerator::global()->generate() & 0xFF);
    const QByteArray key = nonce.toBase64();

    QByteArray request;
    request += "GET " + url.path().toUtf8() + " HTTP/1.1\r\n";
    request += "Host: " + host.toUtf8() + "\r\n";
    request += "Upgrade: websocket\r\n";
    request += "Connection: Upgrade\r\n";
    request += "Sec-WebSocket-Key: " + key + "\r\n";
    request += "Sec-WebSocket-Version: 13\r\n";

    for (auto it = m_headers.constBegin(); it != m_headers.constEnd(); ++it) {
      request += it.key() + ": " + it.value() + "\r\n";
    }

    request += "\r\n";
    m_socket->write(request);
  });

  connect(m_socket, &QSslSocket::readyRead, this, &WebSocketClient::handleReadyRead);
  connect(m_socket, &QSslSocket::disconnected, this, [this]() {
    m_handshakeDone = false;
    emit disconnected();
  });

  connect(m_socket, &QSslSocket::errorOccurred, this, [this](QSslSocket::SocketError) {
    if (!m_handshakeDone) {
      emit errorOccurred(m_socket->errorString());
    }
  });
}

WebSocketClient::~WebSocketClient() {
  if (m_socket)
    m_socket->deleteLater();
}

bool WebSocketClient::isConnected() const {
  return m_handshakeDone && m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void WebSocketClient::open(const QString &url,
                           const QHash<QByteArray, QByteArray> &headers) {
  if (isConnected())
    return;

  m_url = url;
  m_headers = headers;
  m_incoming.clear();
  m_handshakeDone = false;
  m_closing = false;
  m_fragment.clear();
  m_fragmentOpcode = 0;

  const QUrl parsed(url);
  m_socket->connectToHostEncrypted(parsed.host(), parsed.port(443));
}

void WebSocketClient::close() {
  if (!isConnected()) {
    m_socket->abort();
    return;
  }
  m_closing = true;
  sendFrame(OpClose, QByteArray());
  m_socket->flush();
}

void WebSocketClient::abort() {
  m_socket->abort();
}

void WebSocketClient::sendText(const QString &message) {
  if (!isConnected())
    return;
  sendFrame(OpText, message.toUtf8());
}

void WebSocketClient::sendPing() {
  if (!isConnected())
    return;
  sendFrame(OpPing, QByteArray());
}

void WebSocketClient::handleReadyRead() {
  m_incoming.append(m_socket->readAll());

  if (!m_handshakeDone) {
    const int headerEnd = m_incoming.indexOf("\r\n\r\n");
    if (headerEnd < 0) {
      // Ждём полный заголовок (эвристический лимит против мусора).
      if (m_incoming.size() > 16 * 1024)
        fail("WebSocket handshake header too large");
      return;
    }

    const QByteArray header = m_incoming.left(headerEnd);
    m_incoming.remove(0, headerEnd + 4);

    if (!header.startsWith("HTTP/1.1 101")) {
      fail("WebSocket handshake rejected: " + QString::fromUtf8(header.left(200)));
      return;
    }
    m_handshakeDone = true;
    emit connected();
  }

  processFrames();
}

void WebSocketClient::processFrames() {
  while (m_incoming.size() >= 2) {
    const quint8 b0 = static_cast<quint8>(m_incoming.at(0));
    const quint8 b1 = static_cast<quint8>(m_incoming.at(1));

    const bool fin = (b0 & 0x80) != 0;
    const quint8 opcode = b0 & 0x0F;
    const bool masked = (b1 & 0x80) != 0;
    quint64 payloadLen = b1 & 0x7F;
    int offset = 2;

    if (payloadLen == 126) {
      if (m_incoming.size() < offset + 2)
        return;
      payloadLen = qFromBigEndian<quint16>(reinterpret_cast<const uchar *>(m_incoming.constData() + offset));
      offset += 2;
    } else if (payloadLen == 127) {
      if (m_incoming.size() < offset + 8)
        return;
      payloadLen = qFromBigEndian<quint64>(reinterpret_cast<const uchar *>(m_incoming.constData() + offset));
      offset += 8;
    }

    QByteArray maskKey;
    if (masked) {
      if (m_incoming.size() < offset + 4)
        return;
      maskKey = m_incoming.mid(offset, 4);
      offset += 4;
    }

    if (m_incoming.size() < static_cast<int>(offset + payloadLen))
      return;

    QByteArray payload = m_incoming.mid(offset, static_cast<int>(payloadLen));
    m_incoming.remove(0, offset + static_cast<int>(payloadLen));

    if (masked)
      payload = applyMask(payload, maskKey);

    switch (opcode) {
    case OpText:
    case OpBinary:
      if (fin) {
        emit textMessageReceived(QString::fromUtf8(payload));
      } else {
        m_fragment = payload;
        m_fragmentOpcode = opcode;
      }
      break;

    case OpContinuation:
      m_fragment.append(payload);
      if (fin) {
        emit textMessageReceived(QString::fromUtf8(m_fragment));
        m_fragment.clear();
        m_fragmentOpcode = 0;
      }
      break;

    case OpPing:
      // Отвечаем pong'ом с тем же payload.
      sendFrame(OpPong, payload);
      break;

    case OpPong:
      emit pongReceived();
      break;

    case OpClose:
      m_closing = true;
      // Эхо close-кадра, затем закрываем TCP.
      if (fin)
        sendFrame(OpClose, payload);
      m_socket->disconnectFromHost();
      break;

    default:
      fail("Unsupported WebSocket opcode");
      return;
    }
  }
}

void WebSocketClient::sendFrame(quint8 opcode, const QByteArray &payload) {
  if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
    return;

  QByteArray frame;
  frame.append(static_cast<char>(0x80 | opcode));

  const int len = payload.size();
  if (len < 126) {
    frame.append(static_cast<char>(0x80 | len));
  } else if (len < 65536) {
    frame.append(static_cast<char>(0x80 | 126));
    const quint16 big = qToBigEndian<quint16>(static_cast<quint16>(len));
    frame.append(reinterpret_cast<const char *>(&big), 2);
  } else {
    frame.append(static_cast<char>(0x80 | 127));
    const quint64 big = qToBigEndian<quint64>(static_cast<quint64>(len));
    frame.append(reinterpret_cast<const char *>(&big), 8);
  }

  const QByteArray mask = randomMask();
  frame.append(mask);
  frame.append(applyMask(payload, mask));

  m_socket->write(frame);
}

void WebSocketClient::fail(const QString &message) {
  m_handshakeDone = false;
  emit errorOccurred(message);
  m_socket->abort();
}