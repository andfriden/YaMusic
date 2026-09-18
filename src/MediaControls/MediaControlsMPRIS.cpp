#include "MediaControlsMPRIS.h"

#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QVariantMap>

namespace {
constexpr auto kServiceName = "org.mpris.MediaPlayer2.YaMusic";
constexpr auto kObjectPath = "/org/mpris/MediaPlayer2";

QString playbackStatusToString(SystemMediaControls::PlaybackStatus status) {
  switch (status) {
  case SystemMediaControls::PlaybackStatus::Playing:
    return QStringLiteral("Playing");
  case SystemMediaControls::PlaybackStatus::Paused:
    return QStringLiteral("Paused");
  case SystemMediaControls::PlaybackStatus::Stopped:
    return QStringLiteral("Stopped");
  }
  return QStringLiteral("Stopped");
}

} // anonymous namespace

// Импл — основной QObject на шине; адаптеры — его дочерние объекты.
class Impl : public QObject {
  Q_OBJECT

public:
  explicit Impl(MediaControlsMPRIS *owner) : QObject(owner), m_owner(owner) {}

  bool registerService();
  void unregisterService();

  SystemMediaControls::PlaybackStatus playbackStatus() const {
    return m_owner->mediaPlaybackStatus();
  }
  QString loopStatus() const { return m_owner->mediaLoopStatus(); }
  bool shuffle() const { return m_owner->mediaShuffle(); }
  const SystemMediaControls::Metadata &metadata() const { return m_owner->mediaMetadata(); }
  qint64 positionMs() const { return m_owner->mediaPositionMs(); }

  void requestPlay() { Q_EMIT m_owner->playRequested(); }
  void requestPause() { Q_EMIT m_owner->pauseRequested(); }
  void requestToggle() { Q_EMIT m_owner->togglePlayPauseRequested(); }
  void requestNext() { Q_EMIT m_owner->nextRequested(); }
  void requestPrevious() { Q_EMIT m_owner->previousRequested(); }
  void requestSeek(qint64 positionMs) { Q_EMIT m_owner->seekRequested(positionMs); }

  void notifyAllChanged();
  void notifySeeked(qint64 positionMs);

private:
  MediaControlsMPRIS *m_owner = nullptr;
  qint64 m_lastSeekedMs = 0;
};

// Адаптеры — по одному на D-Bus интерфейс.
namespace {
class RootAdaptor : public QDBusAbstractAdaptor {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")

  Q_PROPERTY(bool CanQuit READ canQuit CONSTANT)
  Q_PROPERTY(bool CanRaise READ canRaise CONSTANT)
  Q_PROPERTY(bool HasTrackList READ hasTrackList CONSTANT)
  Q_PROPERTY(QString Identity READ identity CONSTANT)
  Q_PROPERTY(QString DesktopEntry READ desktopEntry CONSTANT)
  Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes CONSTANT)
  Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes CONSTANT)

public:
  explicit RootAdaptor(Impl *parent) : QDBusAbstractAdaptor(parent) {}

public slots:
  void Raise() {}
  void Quit() {}

  bool canQuit() const { return false; }
  bool canRaise() const { return false; }
  bool hasTrackList() const { return false; }
  QString identity() const { return QStringLiteral("YaMusic"); }
  QString desktopEntry() const { return QStringLiteral("yamusic"); }
  QStringList supportedUriSchemes() const { return {QStringLiteral("https")}; }
  QStringList supportedMimeTypes() const { return {QStringLiteral("audio/mpeg")}; }
};

class PlayerAdaptor : public QDBusAbstractAdaptor {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")

  Q_PROPERTY(QString PlaybackStatus READ playbackStatus NOTIFY playbackStatusChanged)
  Q_PROPERTY(QString LoopStatus READ loopStatus NOTIFY loopStatusChanged)
  Q_PROPERTY(double Rate READ rate CONSTANT)
  Q_PROPERTY(bool Shuffle READ shuffle NOTIFY shuffleChanged)
  Q_PROPERTY(QVariantMap Metadata READ metadata NOTIFY metadataChanged)
  Q_PROPERTY(double Volume READ volume WRITE setVolume NOTIFY volumeChanged)
  Q_PROPERTY(qint64 Position READ position)
  Q_PROPERTY(double MinimumRate READ minimumRate CONSTANT)
  Q_PROPERTY(double MaximumRate READ maximumRate CONSTANT)
  Q_PROPERTY(bool CanGoNext READ canGoNext CONSTANT)
  Q_PROPERTY(bool CanGoPrevious READ canGoPrevious CONSTANT)
  Q_PROPERTY(bool CanPlay READ canPlay CONSTANT)
  Q_PROPERTY(bool CanPause READ canPause CONSTANT)
  Q_PROPERTY(bool CanSeek READ canSeek CONSTANT)
  Q_PROPERTY(bool CanControl READ canControl CONSTANT)

public:
  explicit PlayerAdaptor(Impl *parent) : QDBusAbstractAdaptor(parent), m_impl(parent) {}

public slots:
  void Next() { m_impl->requestNext(); }
  void Previous() { m_impl->requestPrevious(); }
  void Pause() { m_impl->requestPause(); }
  void PlayPause() { m_impl->requestToggle(); }
  void Stop() { m_impl->requestPause(); }
  void Play() { m_impl->requestPlay(); }

  void Seek(qint64 offsetMicroseconds) {
    const qint64 targetUs = qMax<qint64>(0, m_impl->positionMs() * 1000 + offsetMicroseconds);
    m_impl->requestSeek(targetUs / 1000);
  }

  void SetPosition(const QDBusObjectPath &, qint64 posUs) {
    m_impl->requestSeek(qMax<qint64>(0, posUs) / 1000);
  }

  void OpenUri(const QString &) {}

  QString playbackStatus() const { return playbackStatusToString(m_impl->playbackStatus()); }
  QString loopStatus() const { return m_impl->loopStatus(); }
  double rate() const { return 1.0; }
  bool shuffle() const { return m_impl->shuffle(); }

  QVariantMap metadata() const {
    QVariantMap md;
    const auto &t = m_impl->metadata();

    if (!t.trackId.isEmpty())
      md[QStringLiteral("mpris:trackid")] = QVariant::fromValue(
          QDBusObjectPath(QStringLiteral("/org/mpris/MediaPlayer2/YaMusic/%1").arg(t.trackId)));
    if (!t.title.isEmpty()) md[QStringLiteral("xesam:title")] = t.title;
    if (!t.artist.isEmpty()) md[QStringLiteral("xesam:artist")] = QStringList{t.artist};
    if (!t.album.isEmpty()) md[QStringLiteral("xesam:album")] = t.album;
    if (t.durationMs > 0) md[QStringLiteral("mpris:length")] = t.durationMs * 1000;
    if (!t.coverUrl.isEmpty()) md[QStringLiteral("mpris:artUrl")] = t.coverUrl;
    return md;
  }

  double volume() const { return 1.0; }
  void setVolume(double) {}
  qint64 position() const { return m_impl->positionMs() * 1000; }
  double minimumRate() const { return 1.0; }
  double maximumRate() const { return 1.0; }

  bool canGoNext() const { return true; }
  bool canGoPrevious() const { return true; }
  bool canPlay() const { return true; }
  bool canPause() const { return true; }
  bool canSeek() const { return true; }
  bool canControl() const { return true; }

  void emitAllChanged() {
    Q_EMIT playbackStatusChanged();
    Q_EMIT loopStatusChanged();
    Q_EMIT shuffleChanged();
    Q_EMIT metadataChanged();
    sendPropertiesChanged();
  }

  void emitSeeked(qint64 positionMs) { Q_EMIT Seeked(positionMs * 1000); }

  void sendPropertiesChanged() {
    QVariantMap changed;
    changed.insert(QStringLiteral("PlaybackStatus"), playbackStatus());
    changed.insert(QStringLiteral("LoopStatus"), loopStatus());
    changed.insert(QStringLiteral("Shuffle"), shuffle());
    changed.insert(QStringLiteral("Metadata"), metadata());

    QDBusMessage msg = QDBusMessage::createSignal(QStringLiteral("/org/mpris/MediaPlayer2"),
                                                  QStringLiteral("org.freedesktop.DBus.Properties"),
                                                  QStringLiteral("PropertiesChanged"));
    msg << QStringLiteral("org.mpris.MediaPlayer2.Player") << changed << QStringList();
    QDBusConnection::sessionBus().send(msg);
  }

signals:
  void playbackStatusChanged();
  void loopStatusChanged();
  void shuffleChanged();
  void metadataChanged();
  void volumeChanged();
  void Seeked(qint64 positionMicroseconds);

private:
  Impl *m_impl = nullptr;
};

} // anonymous namespace

bool Impl::registerService() {
  QDBusConnection bus = QDBusConnection::sessionBus();

  if (!bus.registerService(kServiceName)) {
    qWarning("MPRIS: cannot register service %s", kServiceName);
    return false;
  }

  new RootAdaptor(this);
  new PlayerAdaptor(this);

  if (!bus.registerObject(kObjectPath, this, QDBusConnection::ExportAdaptors)) {
    qWarning("MPRIS: cannot register object %s", kObjectPath);
    bus.unregisterService(kServiceName);
    return false;
  }
  return true;
}

void Impl::unregisterService() {
  QDBusConnection bus = QDBusConnection::sessionBus();
  bus.unregisterObject(kObjectPath);
  bus.unregisterService(kServiceName);
}

void Impl::notifyAllChanged() {
  auto *pa = findChild<PlayerAdaptor *>(QString(), Qt::FindDirectChildrenOnly);
  if (pa) pa->emitAllChanged();
}

void Impl::notifySeeked(qint64 positionMs) {
  if (qAbs(positionMs - m_lastSeekedMs) < 1000) return;
  auto *pa = findChild<PlayerAdaptor *>(QString(), Qt::FindDirectChildrenOnly);
  if (pa) pa->emitSeeked(positionMs);
  m_lastSeekedMs = positionMs;
}

MediaControlsMPRIS::MediaControlsMPRIS(QObject *parent)
    : SystemMediaControls(parent), d(new Impl(this)) {}

MediaControlsMPRIS::~MediaControlsMPRIS() {
  setEnabled(false);
  delete d;
}

void MediaControlsMPRIS::platformSetEnabled(bool enabled) {
  if (enabled)
    d->registerService();
  else
    d->unregisterService();
}

void MediaControlsMPRIS::platformSetMetadata(const Metadata &) {
  if (d) d->notifyAllChanged();
}

void MediaControlsMPRIS::platformSetPlaybackStatus(PlaybackStatus) {
  if (d) d->notifyAllChanged();
}

void MediaControlsMPRIS::platformSetPosition(qint64 positionMs) {
  if (d) d->notifySeeked(positionMs);
}

void MediaControlsMPRIS::platformSetDuration(qint64) {}

void MediaControlsMPRIS::platformSetLoopStatus(const QString &) {
  if (d) d->notifyAllChanged();
}

void MediaControlsMPRIS::platformSetShuffle(bool) {
  if (d) d->notifyAllChanged();
}

#include "MediaControlsMPRIS.moc"