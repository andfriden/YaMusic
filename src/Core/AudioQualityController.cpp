#include "AudioQualityController.h"

namespace {
const char *kQualityKey = "audio/quality";
} // namespace

AudioQualityController::AudioQualityController(QObject *parent) : QObject(parent) {
  QSettings settings;
  const QString stored =
      settings.value(QString::fromLatin1(kQualityKey), "high").toString();
  m_quality = normalize(stored);
}

QString AudioQualityController::quality() const {
  return m_quality;
}

void AudioQualityController::setQuality(const QString &quality) {
  const QString normalized = normalize(quality);
  if (m_quality == normalized) {
    return;
  }
  m_quality = normalized;
  QSettings settings;
  settings.setValue(QString::fromLatin1(kQualityKey), m_quality);
  emit qualityChanged();
}

QString AudioQualityController::normalize(const QString &quality) {
  const QString q = quality.trimmed().toLower();
  if (q == "low" || q == "normal" || q == "high" || q == "lossless") {
    return q;
  }
  return "high";
}