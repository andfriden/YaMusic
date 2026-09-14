#include "MediaControlsMPRIS.h"

#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QVariantMap>

/*
 * MPRISv2 — интерфейсы org.mpris.MediaPlayer2 и
 * org.mpris.MediaPlayer2.Player на session bus.
 *
 * Спецификация:
 * https://specifications.freedesktop.org/mpris-spec/latest/
 */

namespace
{

constexpr auto kServiceName     = "org.mpris.MediaPlayer2.YaMusic";
constexpr auto kObjectPath      = "/org/mpris/MediaPlayer2";

constexpr auto kIntrospectionXml = R"(<node>
  <interface name="org.freedesktop.DBus.Introspectable">
    <method name="Introspect">
      <arg name="data" type="s" direction="out"/>
    </method>
  </interface>
  <interface name="org.mpris.MediaPlayer2">
    <method name="Raise"/>
    <method name="Quit"/>
    <property name="CanQuit" type="b" access="read"/>
    <property name="CanRaise" type="b" access="read"/>
    <property name="HasTrackList" type="b" access="read"/>
    <property name="Identity" type="s" access="read"/>
    <property name="DesktopEntry" type="s" access="read"/>
    <property name="SupportedUriSchemes" type="as" access="read"/>
    <property name="SupportedMimeTypes" type="as" access="read"/>
  </interface>
  <interface name="org.mpris.MediaPlayer2.Player">
    <method name="Next"/>
    <method name="Previous"/>
    <method name="Pause"/>
    <method name="PlayPause"/>
    <method name="Stop"/>
    <method name="Play"/>
    <method name="Seek">
      <arg name="Offset" type="x" direction="in"/>
    </method>
    <method name="SetPosition">
      <arg name="TrackId" type="o" direction="in"/>
      <arg name="Position" type="x" direction="in"/>
    </method>
    <method name="OpenUri">
      <arg name="Uri" type="s" direction="in"/>
    </method>
    <property name="PlaybackStatus" type="s" access="read"/>
    <property name="LoopStatus" type="s" access="read"/>
    <property name="Rate" type="d" access="read"/>
    <property name="Shuffle" type="b" access="read"/>
    <property name="Metadata" type="a{sv}" access="read"/>
    <property name="Volume" type="d" access="read"/>
    <property name="Position" type="x" access="read"/>
    <property name="MinimumRate" type="d" access="read"/>
    <property name="MaximumRate" type="d" access="read"/>
    <property name="CanGoNext" type="b" access="read"/>
    <property name="CanGoPrevious" type="b" access="read"/>
    <property name="CanPlay" type="b" access="read"/>
    <property name="CanPause" type="b" access="read"/>
    <property name="CanSeek" type="b" access="read"/>
    <property name="CanControl" type="b" access="read"/>
    <signal name="Seeked">
      <arg name="Position" type="x"/>
    </signal>
  </interface>
</node>)";

QString playbackStatusToString(SystemMediaControls::PlaybackStatus status)
{
    switch (status)
    {
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

/*
 * Impl — QObject, регистрируемый на шине.
 * Вложенный класс имеет доступ к protected-членам владельца,
 * поэтому свойства читают актуальное состояние напрямую.
 */
class MediaControlsMPRIS::Impl : public QObject
{
    Q_OBJECT

    // ---- org.mpris.MediaPlayer2 ----

    Q_PROPERTY(bool CanQuit READ canQuit CONSTANT)
    Q_PROPERTY(bool CanRaise READ canRaise CONSTANT)
    Q_PROPERTY(bool HasTrackList READ hasTrackList CONSTANT)
    Q_PROPERTY(QString Identity READ identity CONSTANT)
    Q_PROPERTY(QString DesktopEntry READ desktopEntry CONSTANT)
    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes CONSTANT)
    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes CONSTANT)

    // ---- org.mpris.MediaPlayer2.Player ----

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
    explicit Impl(MediaControlsMPRIS *owner)
        : QObject(owner)
        , m_owner(owner)
    {
    }

    bool registerService()
    {
        QDBusConnection &bus = QDBusConnection::sessionBus();

        if (!bus.registerService(kServiceName))
        {
            qWarning("MPRIS: cannot register service %s", kServiceName);
            return false;
        }

        const QDBusConnection::RegisterOptions options =
            QDBusConnection::ExportAllSlots |
            QDBusConnection::ExportAllSignals |
            QDBusConnection::ExportAllProperties;

        if (!bus.registerObject(kObjectPath, kIntrospectionXml, this, options))
        {
            qWarning("MPRIS: cannot register object %s", kObjectPath);
            bus.unregisterService(kServiceName);
            return false;
        }

        return true;
    }

    void unregisterService()
    {
        QDBusConnection &bus = QDBusConnection::sessionBus();
        bus.unregisterObject(kObjectPath);
        bus.unregisterService(kServiceName);
    }

    // ---- Методы org.mpris.MediaPlayer2 ----

public slots:
    void Raise() {}
    void Quit() {}

    // ---- Методы org.mpris.MediaPlayer2.Player ----

    void Next()
    {
        Q_EMIT m_owner->nextRequested();
    }

    void Previous()
    {
        Q_EMIT m_owner->previousRequested();
    }

    void Pause()
    {
        Q_EMIT m_owner->pauseRequested();
    }

    void PlayPause()
    {
        Q_EMIT m_owner->togglePlayPauseRequested();
    }

    void Stop()
    {
        Q_EMIT m_owner->pauseRequested();
    }

    void Play()
    {
        Q_EMIT m_owner->playRequested();
    }

    void Seek(qint64 offsetMicroseconds)
    {
        const qint64 currentUs = m_owner->mediaPositionMs() * 1000;
        const qint64 targetUs = qMax<qint64>(0, currentUs + offsetMicroseconds);
        Q_EMIT m_owner->seekRequested(targetUs / 1000);
    }

    void SetPosition(const QDBusObjectPath &trackId, qint64 positionMicroseconds)
    {
        Q_UNUSED(trackId);
        Q_EMIT m_owner->seekRequested(
            qMax<qint64>(0, positionMicroseconds) / 1000);
    }

    void OpenUri(const QString &uri)
    {
        Q_UNUSED(uri);
    }

    // ---- Геттеры org.mpris.MediaPlayer2 ----

    bool canQuit() const { return false; }
    bool canRaise() const { return false; }
    bool hasTrackList() const { return false; }
    QString identity() const { return QStringLiteral("YaMusic"); }
    QString desktopEntry() const { return QStringLiteral("yamusic"); }

    QStringList supportedUriSchemes() const
    {
        return {QStringLiteral("https")};
    }

    QStringList supportedMimeTypes() const
    {
        return {QStringLiteral("audio/mpeg")};
    }

    // ---- Геттеры org.mpris.MediaPlayer2.Player ----

    QString playbackStatus() const
    {
        return playbackStatusToString(m_owner->mediaPlaybackStatus());
    }

    QString loopStatus() const
    {
        return m_owner->mediaLoopStatus();
    }

    double rate() const { return 1.0; }

    bool shuffle() const
    {
        return m_owner->mediaShuffle();
    }

    QVariantMap metadata() const
    {
        QVariantMap md;
        const auto &t = m_owner->mediaMetadata();

        if (!t.trackId.isEmpty())
        {
            md[QStringLiteral("mpris:trackid")] =
                QVariant::fromValue(QDBusObjectPath(
                    QStringLiteral("/org/mpris/MediaPlayer2/YaMusic/%1")
                        .arg(t.trackId)));
        }

        if (!t.title.isEmpty())
            md[QStringLiteral("xesam:title")] = t.title;

        if (!t.artist.isEmpty())
            md[QStringLiteral("xesam:artist")] = QStringList{t.artist};

        if (!t.album.isEmpty())
            md[QStringLiteral("xesam:album")] = t.album;

        if (t.durationMs > 0)
            md[QStringLiteral("mpris:length")] = t.durationMs * 1000;

        if (!t.coverUrl.isEmpty())
            md[QStringLiteral("mpris:artUrl")] = t.coverUrl;

        return md;
    }

    double volume() const { return 1.0; }

    void setVolume(double)
    {
        // Громкость не управляется через MPRIS.
    }

    qint64 position() const
    {
        return m_owner->mediaPositionMs() * 1000;
    }

    double minimumRate() const { return 1.0; }
    double maximumRate() const { return 1.0; }

    bool canGoNext() const { return true; }
    bool canGoPrevious() const { return true; }
    bool canPlay() const { return true; }
    bool canPause() const { return true; }
    bool canSeek() const { return true; }
    bool canControl() const { return true; }

    // ---- Нотификации для D-Bus ----

    void notifyAllChanged()
    {
        Q_EMIT playbackStatusChanged();
        Q_EMIT loopStatusChanged();
        Q_EMIT shuffleChanged();
        Q_EMIT metadataChanged();
    }

    void notifySeeked(qint64 positionMs)
    {
        if (qAbs(positionMs - m_lastSeekedMs) >= 1000)
            Q_EMIT Seeked(positionMs * 1000);
        m_lastSeekedMs = positionMs;
    }

signals:
    void playbackStatusChanged();
    void loopStatusChanged();
    void shuffleChanged();
    void metadataChanged();
    void volumeChanged();
    void Seeked(qint64 positionMicroseconds);

private:
    MediaControlsMPRIS *m_owner = nullptr;
    qint64 m_lastSeekedMs = 0;
};

// ---------- C++ интерфейс ----------

MediaControlsMPRIS::MediaControlsMPRIS(QObject *parent)
    : SystemMediaControls(parent)
    , d(new Impl(this))
{
}

MediaControlsMPRIS::~MediaControlsMPRIS()
{
    setEnabled(false);
    delete d;
}

void MediaControlsMPRIS::platformSetEnabled(bool enabled)
{
    if (enabled)
        d->registerService();
    else
        d->unregisterService();
}

void MediaControlsMPRIS::platformSetMetadata(const Metadata &)
{
    if (d)
        d->notifyAllChanged();
}

void MediaControlsMPRIS::platformSetPlaybackStatus(PlaybackStatus)
{
    if (d)
        d->notifyAllChanged();
}

void MediaControlsMPRIS::platformSetPosition(qint64 positionMs)
{
    if (d)
        d->notifySeeked(positionMs);
}

void MediaControlsMPRIS::platformSetDuration(qint64)
{
}

void MediaControlsMPRIS::platformSetLoopStatus(const QString &)
{
    if (d)
        d->notifyAllChanged();
}

void MediaControlsMPRIS::platformSetShuffle(bool)
{
    if (d)
        d->notifyAllChanged();
}

#include "MediaControlsMPRIS.moc"