#pragma once

#include <QObject>
#include <QString>

class TrackCache;

namespace Playback {
class Preloader;
}

// Настройки предзагрузки для интерфейса: включена ли докачка следующих
// треков, сколько их держать готовыми и сколько места отдан кэш.
//
// Сами значения живут в QSettings, а работают с ними кэш и предзагрузчик:
// контроллер только хранит настройку и показывает состояние.
class PreloadController : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(int aheadCount READ aheadCount WRITE setAheadCount NOTIFY aheadCountChanged)
  Q_PROPERTY(int limitMb READ limitMb WRITE setLimitMb NOTIFY limitMbChanged)
  Q_PROPERTY(double usedMb READ usedMb NOTIFY cacheChanged)
  Q_PROPERTY(int windowCount READ windowCount NOTIFY cacheChanged)
  Q_PROPERTY(int readyCount READ readyCount NOTIFY cacheChanged)

public:
  explicit PreloadController(TrackCache *cache, Playback::Preloader *preloader,
                             QObject *parent = nullptr);

  bool enabled() const;
  int aheadCount() const;
  int limitMb() const;
  double usedMb() const;
  int windowCount() const;
  int readyCount() const;

  void setEnabled(bool enabled);
  void setAheadCount(int count);
  void setLimitMb(int megabytes);

  // Удаляет скачанное: следующий трек придётся снова тянуть из сети.
  Q_INVOKABLE void clearCache();

signals:
  void enabledChanged();
  void aheadCountChanged();
  void limitMbChanged();
  void cacheChanged();

private:
  void applyLimit();

  TrackCache *m_cache = nullptr;
  Playback::Preloader *m_preloader = nullptr;

  bool m_enabled = true;
  int m_aheadCount = 3;
  int m_limitMb = 500;
};
