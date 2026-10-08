#pragma once

#include <QObject>
#include <QSettings>
#include <QString>

// Выбор качества аудио при воспроизведении.
// Значения: "low", "normal", "high", "lossless".
// Сохраняется в QSettings (ключ "audio/quality").
class AudioQualityController : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString quality READ quality WRITE setQuality NOTIFY qualityChanged)

public:
  explicit AudioQualityController(QObject *parent = nullptr);

  QString quality() const;

  Q_INVOKABLE void setQuality(const QString &quality);

  // Нормализованное значение (пустая строка, если некорректное).
  static QString normalize(const QString &quality);

signals:
  void qualityChanged();

private:
  QString m_quality = "high";
};