#include "StatsController.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

namespace {

constexpr auto kStatsTracksKey = "stats/tracks";
constexpr auto kStatsArtistsKey = "stats/artists";
constexpr auto kStatsTotalPlaysKey = "stats/totalPlays";
constexpr auto kStatsTotalTimeKey = "stats/totalTime";

} // namespace

StatsController::StatsController(QObject *parent) : QObject(parent) {
  load();
}

int StatsController::totalPlays() const {
  return m_totalPlays;
}

int StatsController::totalPlayTimeMs() const {
  return m_totalPlayTimeMs;
}

int StatsController::trackCount() const {
  return m_tracks.size();
}

int StatsController::artistCount() const {
  return m_artists.size();
}

void StatsController::recordPlay(const QString &trackId, const QString &title,
                                 const QString &artist, const QString &artistId,
                                 const QString &coverUri, qint64 durationMs) {
  if (trackId.isEmpty()) return;

  // Трек: накапливаем проигрывания и время.
  QVariantMap track = m_tracks.value(trackId).toMap();
  track.insert(QStringLiteral("id"), trackId);
  track.insert(QStringLiteral("title"), title);
  track.insert(QStringLiteral("artist"), artist);
  if (!artistId.isEmpty()) track.insert(QStringLiteral("artistId"), artistId);
  track.insert(QStringLiteral("coverUri"), coverUri);
  track.insert(QStringLiteral("durationMs"), static_cast<qlonglong>(durationMs));
  track.insert(QStringLiteral("plays"),
               track.value(QStringLiteral("plays"), 0).toInt() + 1);
  m_tracks.insert(trackId, track);

  // Артист: агрегируем по артисту, если он указан.
  if (!artistId.isEmpty()) {
    QVariantMap artistEntry = m_artists.value(artistId).toMap();
    artistEntry.insert(QStringLiteral("id"), artistId);
    artistEntry.insert(QStringLiteral("name"), artist);
    artistEntry.insert(QStringLiteral("coverUri"), coverUri);
    artistEntry.insert(QStringLiteral("plays"),
                       artistEntry.value(QStringLiteral("plays"), 0).toInt() + 1);
    m_artists.insert(artistId, artistEntry);
  }

  m_totalPlays += 1;
  if (durationMs > 0) m_totalPlayTimeMs += static_cast<int>(durationMs);

  persist();
  emit statsChanged();
}

QVariantList StatsController::topTracks(int limit) const {
  QList<QVariantMap> tracks;
  tracks.reserve(m_tracks.size());

  for (auto it = m_tracks.cbegin(); it != m_tracks.cend(); ++it) {
    tracks.append(it.value().toMap());
  }

  std::sort(tracks.begin(), tracks.end(),
            [](const QVariantMap &a, const QVariantMap &b) {
              return a.value(QStringLiteral("plays")).toInt() >
                     b.value(QStringLiteral("plays")).toInt();
            });

  if (limit > 0 && tracks.size() > limit) tracks.resize(limit);

  QVariantList result;
  result.reserve(tracks.size());
  for (const QVariantMap &map : tracks) result.append(map);
  return result;
}

QVariantList StatsController::topArtists(int limit) const {
  QList<QVariantMap> artists;
  artists.reserve(m_artists.size());

  for (auto it = m_artists.cbegin(); it != m_artists.cend(); ++it) {
    artists.append(it.value().toMap());
  }

  std::sort(artists.begin(), artists.end(),
            [](const QVariantMap &a, const QVariantMap &b) {
              return a.value(QStringLiteral("plays")).toInt() >
                     b.value(QStringLiteral("plays")).toInt();
            });

  if (limit > 0 && artists.size() > limit) artists.resize(limit);

  QVariantList result;
  result.reserve(artists.size());
  for (const QVariantMap &map : artists) result.append(map);
  return result;
}

void StatsController::reset() {
  m_tracks.clear();
  m_artists.clear();
  m_totalPlays = 0;
  m_totalPlayTimeMs = 0;
  persist();
  emit statsChanged();
}

void StatsController::persist() const {
  QSettings settings;
  settings.setValue(QLatin1String(kStatsTracksKey), m_tracks);
  settings.setValue(QLatin1String(kStatsArtistsKey), m_artists);
  settings.setValue(QLatin1String(kStatsTotalPlaysKey), m_totalPlays);
  settings.setValue(QLatin1String(kStatsTotalTimeKey), m_totalPlayTimeMs);
}

void StatsController::load() {
  QSettings settings;
  m_tracks = settings.value(QLatin1String(kStatsTracksKey)).toMap();
  m_artists = settings.value(QLatin1String(kStatsArtistsKey)).toMap();
  m_totalPlays = settings.value(QLatin1String(kStatsTotalPlaysKey), 0).toInt();
  m_totalPlayTimeMs = settings.value(QLatin1String(kStatsTotalTimeKey), 0).toInt();
}