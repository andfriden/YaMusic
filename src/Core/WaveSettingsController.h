#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVariantList>

// Настройки «Моей волны»: язык, характер подборки и настроение.
//
// Значения совпадают с тем, что принимает Яндекс в
// POST /rotor/station/user:onyourwave/settings3. Сервер хранит настройки
// станции сам, поэтому контроллер только помнит выбор между запусками и
// сообщает, что его нужно отправить перед следующей загрузкой волны.
class WaveSettingsController : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY settingsChanged)
  Q_PROPERTY(QString diversity READ diversity WRITE setDiversity NOTIFY settingsChanged)
  Q_PROPERTY(QString moodEnergy READ moodEnergy WRITE setMoodEnergy NOTIFY settingsChanged)

  // Варианты для интерфейса: [{"value": ..., "name": ...}].
  Q_PROPERTY(QVariantList languageOptions READ languageOptions CONSTANT)
  Q_PROPERTY(QVariantList diversityOptions READ diversityOptions CONSTANT)
  Q_PROPERTY(QVariantList moodEnergyOptions READ moodEnergyOptions CONSTANT)

public:
  explicit WaveSettingsController(QObject *parent = nullptr);

  QString language() const;
  void setLanguage(const QString &language);

  QString diversity() const;
  void setDiversity(const QString &diversity);

  QString moodEnergy() const;
  void setMoodEnergy(const QString &moodEnergy);

  QVariantList languageOptions() const;
  QVariantList diversityOptions() const;
  QVariantList moodEnergyOptions() const;

  // Тело запроса settings3.
  QJsonObject toJson() const;

  // Выбор менялся после последней отправки на сервер. Первый запуск
  // настройки не трогает: пока пользователь ничего не выбрал, волна
  // работает с тем, что уже сохранено в аккаунте.
  bool pendingApply() const;
  void markApplied();

signals:
  void settingsChanged();

private:
  bool setOption(QString &field, const QString &value, const QStringList &allowed,
                 const char *key);

  QString m_language;
  QString m_diversity;
  QString m_moodEnergy;
  bool m_pendingApply = false;
};
