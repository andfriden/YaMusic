#include "StreamProxy.h"
#include <QNetworkReply>
#include <limits>

StreamProxy::StreamProxy(QObject *parent) : QIODevice(parent) {
    open(ReadOnly | Unbuffered);
}

StreamProxy::~StreamProxy() {
    m_destroyed = true;
    if (m_reply) {
        m_reply->disconnect(this);
        m_reply->abort();
        m_reply->deleteLater();
    }
}

void StreamProxy::setReply(QNetworkReply *reply) {
    m_reply = reply;

    const QVariant contentLength = reply->header(QNetworkRequest::ContentLengthHeader);
    bool ok = false;
    const qint64 length = contentLength.toLongLong(&ok);
    if (ok && length > 0)
        m_totalSize = length;

    connect(reply, &QNetworkReply::readyRead, this, [this]() {
        QMutexLocker lock(&m_mutex);
        m_buffer.append(m_reply->readAll());
        m_cond.wakeAll();
    });

    connect(reply, &QNetworkReply::finished, this, [this]() {
        QMutexLocker lock(&m_mutex);
        if (!m_finished) {
            m_finished = true;
            m_buffer.append(m_reply->readAll());
            m_cond.wakeAll();
        }
        if (m_reply)
            m_reply->deleteLater();
    });

    connect(reply, &QNetworkReply::errorOccurred, this,
            [this](QNetworkReply::NetworkError) {
                if (m_destroyed) return;
                QMutexLocker lock(&m_mutex);
                if (!m_finished) {
                    m_error = true;
                    m_finished = true;
                    m_cond.wakeAll();
                }
                lock.unlock();
                emit networkError(m_reply ? m_reply->errorString() : QString());
            });
}

// Остановка потока: сеть обрывается, ожидающие чтения пробуждаются
// и завершаются (EOF без ошибки), чтобы демаксер вышел штатно.
// Буфер очищается — освобождаем память; объект остаётся живым,
// пока ffmpeg-демаксер не закончит с ним работу.
void StreamProxy::abort() {
    m_destroyed = true;
    if (m_reply) {
        m_reply->disconnect(this);
        m_reply->abort();
        m_reply->deleteLater();
    }
    QMutexLocker lock(&m_mutex);
    m_finished = true;
    m_buffer.clear();
    m_cond.wakeAll();
}

bool StreamProxy::atEnd() const {
    QMutexLocker lock(&m_mutex);
    return m_finished && pos() >= m_buffer.size();
}

qint64 StreamProxy::readData(char *data, qint64 maxSize) {
    QMutexLocker lock(&m_mutex);
    while (pos() >= m_buffer.size() && !m_finished) {
        m_cond.wait(&m_mutex);
    }
    if (m_error) return -1;
    if (pos() >= m_buffer.size()) return 0; // честный EOF после завершения загрузки

    const qint64 bytes = qMin(maxSize, m_buffer.size() - pos());
    memcpy(data, m_buffer.constData() + pos(), static_cast<size_t>(bytes));
    return bytes; // базовый QIODevice сам сдвинет pos на прочитанное
}

qint64 StreamProxy::writeData(const char *, qint64) {
    return -1;
}

bool StreamProxy::seek(qint64 pos) {
    QMutexLocker lock(&m_mutex);
    // Не даём seek в область за буфером: при файле размера max()
    // ffmpeg может рассчитать гигантский byte offset для SEEK_END.
    // Капаем до актуально скачанного объёма, чтобы readData() не
    // завис в вечном ожидании непрочитанных байт.
    return QIODevice::seek(qBound(qint64(0), pos, m_buffer.size()));
}

qint64 StreamProxy::size() const {
    QMutexLocker lock(&m_mutex);
    if (m_totalSize > 0)
        return m_totalSize;
    // Возвращаем максимально возможное значение, чтобы FFmpeg НЕ оценивал
    // длительность трека через (file_size / bitrate) — иначе для маленького
    // начального буфера (первые килобайты) получится длительность ~0.3 с.
    // С таким sentinel FFmpeg полагается на реальные MPEG-заголовки (XING/VBRI
    // для VBR или длительность по накопленным кадрам), что даёт верную цифру.
    return std::numeric_limits<qint64>::max();
}

qint64 StreamProxy::bytesAvailable() const {
    QMutexLocker lock(&m_mutex);
    return qMax<qint64>(0, m_buffer.size() - pos()) + QIODevice::bytesAvailable();
}