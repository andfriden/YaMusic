#pragma once

#include <QHash>
#include <QObject>
#include <QSettings>
#include <QString>

// Выбор качества аудио при воспроизведении.
// Значения: "low", "normal", "high", "lossless".
// Сохраняется в QSettings (ключ "audio/quality").
class AudioQualityController : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString quality READ quality WRITE setQuality NOTIFY qualityChanged)

  // Включает подмену цензурной версии трека на нецензурную
  // (по базе FckCensorData), если для трека нет явного исключения.
  Q_PROPERTY(bool censorBypass READ censorBypass WRITE setCensorBypass NOTIFY censorPreferencesChanged)

public:
  explicit AudioQualityController(QObject *parent = nullptr);

  QString quality() const;

  Q_INVOKABLE void setQuality(const QString &quality);

  bool censorBypass() const;

  Q_INVOKABLE void setCensorBypass(bool enabled);

  // Предпочитать оригинальную (цензурную) версию для конкретного трека.
  Q_INVOKABLE bool prefersOriginal(const QString &trackId) const;

  Q_INVOKABLE void setPrefersOriginal(const QString &trackId, bool original);

  // Нормализованное значение (пустая строка, если некорректное).
  static QString normalize(const QString &quality);

signals:
  void qualityChanged();

  void censorPreferencesChanged();

private:
  QString m_quality = "high";
  bool m_censorBypass = false;
  QHash<QString, bool> m_preferOriginal;
};