#include "AudioQualityController.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace {
const char *kQualityKey = "audio/quality";
const char *kCensorBypassKey = "audio/censorBypass";
const char *kCensorPreferKey = "audio/censorPreferOriginal";
} // namespace

AudioQualityController::AudioQualityController(QObject *parent) : QObject(parent) {
  QSettings settings;
  const QString stored =
      settings.value(QString::fromLatin1(kQualityKey), "high").toString();
  m_quality = normalize(stored);

  m_censorBypass =
      settings.value(QString::fromLatin1(kCensorBypassKey), false).toBool();

  const QString preferJson =
      settings.value(QString::fromLatin1(kCensorPreferKey)).toString();
  if (!preferJson.isEmpty()) {
    const QJsonDocument doc = QJsonDocument::fromJson(preferJson.toUtf8());
    if (doc.isObject()) {
      const QJsonObject obj = doc.object();
      for (auto it = obj.begin(); it != obj.end(); ++it) {
        m_preferOriginal.insert(it.key(), it.value().toBool());
      }
    }
  }
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
bool AudioQualityController::censorBypass() const {
  return m_censorBypass;
}

void AudioQualityController::setCensorBypass(bool enabled) {
  if (m_censorBypass == enabled) {
    return;
  }
  m_censorBypass = enabled;
  QSettings settings;
  settings.setValue(QString::fromLatin1(kCensorBypassKey), m_censorBypass);
  emit censorPreferencesChanged();
}

bool AudioQualityController::prefersOriginal(const QString &trackId) const {
  const QString id = trackId.trimmed();
  if (id.isEmpty()) {
    return false;
  }
  return m_preferOriginal.value(id, false);
}

void AudioQualityController::setPrefersOriginal(const QString &trackId, bool original) {
  const QString id = trackId.trimmed();
  if (id.isEmpty()) {
    return;
  }
  if (original) {
    m_preferOriginal.insert(id, true);
  } else {
    m_preferOriginal.remove(id);
  }
  QSettings settings;
  QJsonObject obj;
  for (auto it = m_preferOriginal.cbegin(); it != m_preferOriginal.cend(); ++it) {
    obj.insert(it.key(), it.value());
  }
  settings.setValue(QString::fromLatin1(kCensorPreferKey),
                    QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
  emit censorPreferencesChanged();
}
