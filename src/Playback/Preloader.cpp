#include "Preloader.h"
#include "../Player/TrackCache.h"
#include "../Yandex/Catalog/TrackService.h"
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <memory>

namespace Playback {

namespace {

// Докачка считается зависшей, если столько времени не пришло ни одного
// байта: медленный трек не должен держать окно целиком.
constexpr int kStallTimeoutMs = 20000;

} // namespace

Preloader::Preloader(TrackCache *cache, TrackService *trackService, QObject *parent)
    : QObject(parent), m_cache(cache), m_trackService(trackService),
      m_network(new QNetworkAccessManager(this)) {
  Q_ASSERT(m_cache != nullptr);
  Q_ASSERT(m_trackService != nullptr);

  m_stallTimer.setSingleShot(true);
  m_stallTimer.setInterval(kStallTimeoutMs);

  connect(&m_stallTimer, &QTimer::timeout, this, [this]() {
    if (m_reply != nullptr && m_reply->isRunning()) m_reply->abort();
  });

  connect(m_trackService, &TrackService::streamUrlReceived, this,
          [this](const QString &trackId, const QString &url) {
            handleStreamUrl(trackId, url);
          });

  // Кодек приходит вслед за URL: он нужен для расширения файла и для
  // показа качества, когда трек заиграет из кэша.
  connect(m_trackService, &TrackService::streamQualityReceived, this,
          [this](const QString &trackId, const QString &codec, int bitrateKbps) {
            if (trackId != m_resolvingTrackId && trackId != m_downloadingTrackId) return;

            m_codec = codec;
            m_bitrate = bitrateKbps;
          });

  // Ошибку запроса URL TrackService не связывает с конкретным треком, а
  // запрос здесь всегда один: если он ждал ответа — виноват он. Ошибки
  // воспроизведения сюда тоже попадают, но худшее, что будет, — этот
  // трек не попадёт в кэш.
  connect(m_trackService, &TrackService::errorOccurred, this, [this](const QString &) {
    if (m_resolvingTrackId.isEmpty()) return;

    const QString trackId = m_resolvingTrackId;
    m_resolvingTrackId.clear();
    markFailed(trackId);
    step();
  });
}

void Preloader::setEnabled(bool enabled) {
  if (m_enabled == enabled) return;

  m_enabled = enabled;

  if (m_enabled)
    step();
  else
    cancelPending();
}

bool Preloader::isEnabled() const {
  return m_enabled;
}

void Preloader::setAheadCount(int count) {
  const int bounded = qBound(0, count, 20);

  if (m_aheadCount == bounded) return;

  m_aheadCount = bounded;
  emit readyChanged();
  step();
}

int Preloader::aheadCount() const {
  return m_aheadCount;
}

void Preloader::setUpcoming(const QList<Track> &tracks) {
  m_upcoming = tracks;

  // Трек ушёл из окна — незавершённая докачка больше не нужна.
  if (!m_downloadingTrackId.isEmpty() && !wanted(m_downloadingTrackId)) {
    cancelPending();
  } else if (!m_resolvingTrackId.isEmpty() && !wanted(m_resolvingTrackId)) {
    m_resolvingTrackId.clear();
  }

  emit readyChanged();
  step();
}

QString Preloader::cachedPath(const Track &track) const {
  if (track.id.isEmpty()) return {};

  return m_cache->pathFor(TrackCache::identityFor(track.id));
}

int Preloader::windowCount() const {
  return wantedTracks().size();
}

int Preloader::readyCount() const {
  int ready = 0;

  for (const Track &track : wantedTracks()) {
    if (!cachedPath(track).isEmpty()) ++ready;
  }

  return ready;
}

bool Preloader::wanted(const QString &trackId) const {
  if (trackId.isEmpty()) return false;

  for (const Track &track : wantedTracks()) {
    if (track.id == trackId) return true;
  }

  return false;
}

QList<Track> Preloader::wantedTracks() const {
  QList<Track> result;

  for (const Track &track : m_upcoming) {
    if (result.size() >= m_aheadCount) break;
    if (track.id.isEmpty()) continue;
    // Без прав на воспроизведение качать нечего: сервер ответит 403.
    if (!track.available) continue;

    result.append(track);
  }

  return result;
}

void Preloader::step() {
  if (!m_enabled) return;
  if (m_reply != nullptr) return;
  if (!m_resolvingTrackId.isEmpty()) return;

  for (const Track &track : wantedTracks()) {
    if (m_failedTrackIds.contains(track.id)) continue;
    if (m_cache->contains(TrackCache::identityFor(track.id))) continue;

    m_resolvingTrackId = track.id;
    m_trackService->loadStreamInfo(track.id);
    return;
  }
}

void Preloader::handleStreamUrl(const QString &trackId, const QString &url) {
  if (trackId.isEmpty() || trackId != m_resolvingTrackId) return;

  startDownload(trackId, url);
}

void Preloader::startDownload(const QString &trackId, const QString &url) {
  m_resolvingTrackId.clear();

  if (!wanted(trackId)) return;

  if (url.trimmed().isEmpty()) {
    markFailed(trackId);
    step();
    return;
  }

  const QString identity = TrackCache::identityFor(trackId);
  const QString writePath = m_cache->writePath(identity, m_codec);

  auto file = std::make_shared<QFile>(writePath);

  if (!file->open(QIODevice::WriteOnly)) {
    markFailed(trackId);
    step();
    return;
  }

  m_downloadingTrackId = trackId;
  m_writePath = writePath;

  QNetworkRequest request{QUrl(url)};

  request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("YaMusic/1.0 (Qt)"));
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                       QNetworkRequest::NoLessSafeRedirectPolicy);
  // Долгие аудио-потоки надёжнее по HTTP/1.1 — как и у играющего стрима.
  request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);

  QNetworkReply *reply = m_network->get(request);

  m_reply = reply;
  m_stallTimer.start();

  connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
    if (file->isOpen()) file->write(reply->readAll());
  });

  connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64, qint64) {
    m_stallTimer.start();
  });

  connect(reply, &QNetworkReply::finished, this,
          [this, reply, file, writePath, identity, trackId]() {
            m_stallTimer.stop();
            reply->deleteLater();
            file->close();

            m_reply = nullptr;
            m_downloadingTrackId.clear();
            m_writePath.clear();

            if (reply->error() != QNetworkReply::NoError) {
              QFile::remove(writePath);
              markFailed(trackId);
              step();
              return;
            }

            QString error;

            if (!m_cache->finishWrite(writePath, identity, m_codec, &error)) {
              markFailed(trackId);
              step();
              return;
            }

            m_cache->saveMeta(identity, m_codec, m_bitrate);
            m_cache->evictIfNeeded();
            m_codec.clear();
            m_bitrate = 0;

            emit cached(trackId);
            emit readyChanged();
            step();
          });
}

void Preloader::cancelPending() {
  m_resolvingTrackId.clear();

  if (m_reply != nullptr) {
    // Недописанный файл не нужен: сначала отвязываем обработчики, чтобы
    // оборванная загрузка не считалась завершённой.
    disconnect(m_reply, nullptr, this, nullptr);
    m_reply->abort();
    m_reply->deleteLater();
    m_reply = nullptr;
  }

  if (!m_writePath.isEmpty()) {
    QFile::remove(m_writePath);
    m_writePath.clear();
  }

  m_stallTimer.stop();
  m_downloadingTrackId.clear();
  m_codec.clear();
  m_bitrate = 0;
}

void Preloader::markFailed(const QString &trackId) {
  if (trackId.isEmpty()) return;

  m_failedTrackIds.insert(trackId);
  emit readyChanged();
}

} // namespace Playback
