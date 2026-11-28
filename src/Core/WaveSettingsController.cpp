#include "WaveSettingsController.h"
#include <QCoreApplication>

#include <QSettings>
#include <QVariantMap>

namespace {

constexpr auto kLanguageKey = "wave/language";
constexpr auto kDiversityKey = "wave/diversity";
constexpr auto kMoodEnergyKey = "wave/moodEnergy";
constexpr auto kPendingKey = "wave/pendingApply";

// Значения и подписи повторяют restrictions2 станции «Моя волна».
const QStringList kLanguages = {
    QStringLiteral("russian"),
    QStringLiteral("not-russian"),
    QStringLiteral("any"),
    QStringLiteral("without-words"),
};

const QStringList kDiversities = {
    QStringLiteral("favorite"),
    QStringLiteral("discover"),
    QStringLiteral("popular"),
    QStringLiteral("default"),
};

const QStringList kMoodEnergies = {
    QStringLiteral("active"),
    QStringLiteral("fun"),
    QStringLiteral("calm"),
    QStringLiteral("sad"),
    QStringLiteral("all"),
};

QVariantMap option(const QString &value, const QString &name) {
  QVariantMap item;
  item.insert(QStringLiteral("value"), value);
  item.insert(QStringLiteral("name"), name);
  return item;
}

QVariantList languageOptions() {
  return {
      option(QStringLiteral("russian"), QCoreApplication::translate("YaMusic", "Русский")),
      option(QStringLiteral("not-russian"), QCoreApplication::translate("YaMusic", "Иностранный")),
      option(QStringLiteral("any"), QCoreApplication::translate("YaMusic", "Любой")),
      option(QStringLiteral("without-words"), QCoreApplication::translate("YaMusic", "Без слов")),
  };
}

QVariantList diversityOptions() {
  return {
      option(QStringLiteral("favorite"), QCoreApplication::translate("YaMusic", "Любимое")),
      option(QStringLiteral("discover"), QCoreApplication::translate("YaMusic", "Незнакомое")),
      option(QStringLiteral("popular"), QCoreApplication::translate("YaMusic", "Популярное")),
      option(QStringLiteral("default"), QCoreApplication::translate("YaMusic", "Любое")),
  };
}

QVariantList moodEnergyOptions() {
  return {
      option(QStringLiteral("active"), QCoreApplication::translate("YaMusic", "Бодрое")),
      option(QStringLiteral("fun"), QCoreApplication::translate("YaMusic", "Весёлое")),
      option(QStringLiteral("calm"), QCoreApplication::translate("YaMusic", "Спокойное")),
      option(QStringLiteral("sad"), QCoreApplication::translate("YaMusic", "Грустное")),
      option(QStringLiteral("all"), QCoreApplication::translate("YaMusic", "Любое")),
  };
}

// Нейтральное значение: станция не ограничивает подборку.
QString storedOption(const QString &key, const QStringList &allowed,
                     const QString &fallback) {
  const QString value = QSettings().value(key, fallback).toString();
  return allowed.contains(value) ? value : fallback;
}

} // namespace

WaveSettingsController::WaveSettingsController(QObject *parent) : QObject(parent) {
  m_language = storedOption(kLanguageKey, kLanguages, QStringLiteral("any"));
  m_diversity = storedOption(kDiversityKey, kDiversities, QStringLiteral("default"));
  m_moodEnergy = storedOption(kMoodEnergyKey, kMoodEnergies, QStringLiteral("all"));

  // Выбор мог остаться неотправленным, если приложение закрыли до запуска
  // волны: тогда настройки применятся при следующем старте.
  m_pendingApply = QSettings().value(kPendingKey, false).toBool();
}

QString WaveSettingsController::language() const {
  return m_language;
}

void WaveSettingsController::setLanguage(const QString &language) {
  setOption(m_language, language, kLanguages, kLanguageKey);
}

QString WaveSettingsController::diversity() const {
  return m_diversity;
}

void WaveSettingsController::setDiversity(const QString &diversity) {
  setOption(m_diversity, diversity, kDiversities, kDiversityKey);
}

QString WaveSettingsController::moodEnergy() const {
  return m_moodEnergy;
}

void WaveSettingsController::setMoodEnergy(const QString &moodEnergy) {
  setOption(m_moodEnergy, moodEnergy, kMoodEnergies, kMoodEnergyKey);
}

QVariantList WaveSettingsController::languageOptions() const {
  return ::languageOptions();
}

QVariantList WaveSettingsController::diversityOptions() const {
  return ::diversityOptions();
}

QVariantList WaveSettingsController::moodEnergyOptions() const {
  return ::moodEnergyOptions();
}

QJsonObject WaveSettingsController::toJson() const {
  QJsonObject body;
  body.insert(QStringLiteral("moodEnergy"), m_moodEnergy);
  body.insert(QStringLiteral("diversity"), m_diversity);
  body.insert(QStringLiteral("type"), QStringLiteral("rotor"));
  body.insert(QStringLiteral("language"), m_language);
  return body;
}

bool WaveSettingsController::pendingApply() const {
  return m_pendingApply;
}

int WaveSettingsController::revision() const {
  return m_revision;
}

void WaveSettingsController::markApplied(int appliedRevision) {
  // Пока запрос летел, выбор могли поменять — тогда он ещё не отправлен.
  if (appliedRevision != m_revision) return;

  m_pendingApply = false;

  QSettings settings;
  settings.setValue(kPendingKey, false);
}

bool WaveSettingsController::setOption(QString &field, const QString &value,
                                       const QStringList &allowed, const char *key) {
  const QString trimmed = value.trimmed();

  if (!allowed.contains(trimmed) || field == trimmed) {
    return false;
  }

  field = trimmed;
  ++m_revision;

  QSettings settings;
  settings.setValue(key, trimmed);
  settings.setValue(kPendingKey, true);

  m_pendingApply = true;
  emit settingsChanged();
  return true;
}
