#pragma once

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

  // Список изменился (загрузился или обновился).
  Q_SIGNAL void listChanged();

private:
  void ensureLoaded();

  QNetworkAccessManager *m_network = nullptr;
  QSet<QString> m_aiArtists;
  bool m_loaded = false;
  bool m_loading = false;
  qint64 m_loadedAt = 0;
};