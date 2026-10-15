#pragma once

#include <QHash>
#include <QObject>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

// Возвращает нецензурные замены треков по открытой базе FckCensorData.
// Список (trackId -> url) кэшируется в памяти на сутки.
class CensorService : public QObject {
  Q_OBJECT

public:
  explicit CensorService(QObject *parent = nullptr);

  // URL нецензурной версии трека, или пустая строка, если такой нет.
  // Инициирует загрузку списка при первом вызове.
  QString replacementUrl(const QString &trackId);

private:
  void ensureLoaded();

  QNetworkAccessManager *m_network = nullptr;
  QHash<QString, QString> m_replacements;
  bool m_loaded = false;
  bool m_loading = false;
  qint64 m_loadedAt = 0;
};