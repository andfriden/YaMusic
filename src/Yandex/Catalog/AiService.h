#pragma once

#include <QByteArray>
#include <QObject>
#include <QSet>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

// Возвращает, относится ли артист к ИИ-сгенерированному контенту.
// Список (id артистов) качается из открытой базы Slopless и кэшируется
// в памяти на сутки.
class AiService : public QObject {
  Q_OBJECT

public:
  explicit AiService(QObject *parent = nullptr);

  // true, если артист есть в базе ИИ-артистов. Инициирует загрузку
  // списка при первом вызове.
  bool isAiArtist(const QString &artistId);

  // Обновление при запуске: поднимает дисковый кэш (даже просроченный),
  // чтобы данные были сразу, и всегда тянет свежий список из сети.
  void refresh();

  // Список изменился (загрузился или обновился).
  Q_SIGNAL void listChanged();

private:
  void ensureLoaded();
  void fetchFromNetwork();

  // Разбор списка id из JSON. true, если список непустой и применился.
  bool applyJson(const QByteArray &data);
  // Дисковый кэш переживает перезапуск; TTL тот же, что у сетевого.
  bool loadFromDisk(bool allowStale);
  void saveToDisk(const QByteArray &data) const;
  QString cacheFilePath() const;

  QNetworkAccessManager *m_network = nullptr;
  QSet<QString> m_aiArtists;
  bool m_loaded = false;
  bool m_loading = false;
  qint64 m_loadedAt = 0;
};