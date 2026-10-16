#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

// Статистика прослушивания для раздела «Статистика».
//
// Считает прослушивания по трекам и артистам: каждый раз, когда начинает
// играть новый трек, AppController зовёт recordPlay(). Внутри агрегат ведёт
// счётчики проигрываний и суммарное время, которые хранятся между запусками
// в QSettings в виде JSON.
//
// Для интерфейса топы отдаются списками QVariantMap через топ-методы, а
// пересчёт привязок в QML идёт по сигналу statsChanged (того же образца, что
// podcasts у AppController).
class StatsController : public QObject {
  Q_OBJECT

  Q_PROPERTY(int totalPlays READ totalPlays NOTIFY statsChanged)
  Q_PROPERTY(int totalPlayTimeMs READ totalPlayTimeMs NOTIFY statsChanged)
  Q_PROPERTY(int trackCount READ trackCount NOTIFY statsChanged)
  Q_PROPERTY(int artistCount READ artistCount NOTIFY statsChanged)

public:
  explicit StatsController(QObject *parent = nullptr);

  int totalPlays() const;
  int totalPlayTimeMs() const;
  int trackCount() const;
  int artistCount() const;

  // Записывает одно прослушивание трека.
  Q_INVOKABLE void recordPlay(const QString &trackId, const QString &title,
                              const QString &artist, const QString &artistId,
                              const QString &coverUri, qint64 durationMs);

  // Топ-N треков и артистов по числу прослушиваний (массив QVariantMap).
  Q_INVOKABLE QVariantList topTracks(int limit = 20) const;
  Q_INVOKABLE QVariantList topArtists(int limit = 20) const;

  // Обнуляет всю статистику прослушиваний.
  Q_INVOKABLE void reset();

signals:
  void statsChanged();

private:
  void persist() const;
  void load();

  QVariantMap m_tracks;  // ключ — trackId
  QVariantMap m_artists; // ключ — artistId

  int m_totalPlays = 0;
  int m_totalPlayTimeMs = 0;
};