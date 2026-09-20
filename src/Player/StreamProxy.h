#pragma once

#include <QIODevice>
#include <QMutex>
#include <QPointer>
#include <QWaitCondition>

class QNetworkReply;

// Буферизованный сетевой поток для QMediaPlayer.
//
// FFmpeg на macOS не умеет надёжно играть HTTPS-аудио напрямую
// (SecureTransport падает с -9806, "IO Error"). Поэтому аудио качается
// через QNetworkAccessManager (штатный SSL-стек Qt), а демаксеру
// отдаётся QIODevice с уже расшифрованными байтами.
//
// Ключевые моменты совместимости с ffmpeg-бэкендом Qt:
//  - QIODevice::read() при pos() >= size() сразу возвращает 0 (ложный
//    EOF), поэтому size() всегда сообщает больше текущего размера
//    буфера, пока загрузка не завершена. При известном Content-Length
//    возвращается полный размер файла.
//  - atEnd() переопределён: признак конца только после фактического
//    завершения загрузки.
//  - readData() блокируется, пока данные не скачаны, поэтому демаксер
//    не видит «обрыва» потока.
//
// Устройство случайного доступа: перемотка работает в пределах уже
// скачанных данных; незагруженный участок дожидается сети внутри
// readData().
class StreamProxy : public QIODevice {
    Q_OBJECT
public:
    explicit StreamProxy(QObject *parent = nullptr);
    ~StreamProxy();

    void setReply(QNetworkReply *reply);
    void abort();

    bool isSequential() const override { return false; }
    bool atEnd() const override;
    bool seek(qint64 pos) override;
    qint64 size() const override;
    qint64 bytesAvailable() const override;

signals:
    void networkError(const QString &message);

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    QPointer<QNetworkReply> m_reply;
    QByteArray m_buffer;
    qint64 m_totalSize = -1;
    bool m_finished = false;
    bool m_error = false;
    bool m_destroyed = false;
    mutable QMutex m_mutex;
    QWaitCondition m_cond;
};