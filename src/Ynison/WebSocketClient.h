#pragma once

#include <QByteArray>
#include <QHash>
#include <QObject>
#include <QString>

class QSslSocket;

// Минимальный RFC 6455 WebSocket-клиент поверх QSslSocket.
//
// Нужен, потому что QtWebSockets не входит в поставку Qt, а протокол
// Ynison требует обычный WebSocket (wss). Реализуем только то, что
// нужно для Ynison: text-фреймы, ping/pong, close, фрагментация,
// маскирование исходящих фреймов (обязательно для клиента).
class WebSocketClient : public QObject {
  Q_OBJECT

public:
  explicit WebSocketClient(QObject *parent = nullptr);
  ~WebSocketClient() override;

  bool isConnected() const;

  // url вида wss://host/path. Дополнительные HTTP-заголовки
  // (Authorization, Origin, Sec-WebSocket-Protocol) передаются в headers.
  void open(
      const QString &url,
      const QHash<QByteArray, QByteArray> &headers = {});
  void close();
  void abort();

  void sendText(const QString &message);
  void sendPing();

signals:
  void connected();
  void textMessageReceived(const QString &message);
  void pongReceived();
  void disconnected();
  void errorOccurred(const QString &message);

private:
  void handleReadyRead();
  void processFrames();
  void sendFrame(quint8 opcode, const QByteArray &payload);
  void fail(const QString &message);

  QSslSocket *m_socket = nullptr;
  QByteArray m_incoming;
  QString m_url;
  QHash<QByteArray, QByteArray> m_headers;
  bool m_handshakeDone = false;
  bool m_closing = false;

  // Фрагментированное сообщение, собираемое из continuation-фреймов.
  QByteArray m_fragment;
  quint8 m_fragmentOpcode = 0;
};