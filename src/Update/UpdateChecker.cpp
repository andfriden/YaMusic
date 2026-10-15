#include "UpdateChecker.h"
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSysInfo>
#include <QUrlQuery>

namespace Update {

namespace {

constexpr int kMaxReleases = 30;

// Разрядность этого компьютера в том виде, как она стоит в имени файла
// релиза: "arm64" или "x64".
QString archSuffix() {
  const QString arch = QSysInfo::currentCpuArchitecture();

  if (arch.contains(QStringLiteral("arm")) || arch.contains(QStringLiteral("aarch64"))) {
    return QStringLiteral("arm64");
  }

  return QStringLiteral("x64");
}

} // namespace

UpdateChecker::UpdateChecker(QObject *parent)
    : QObject(parent), m_network(new QNetworkAccessManager(this)),
      m_currentVersion(QCoreApplication::applicationVersion()) {}

void UpdateChecker::setCurrentVersion(const QString &version) {
  m_currentVersion = version.trimmed();
}

QString UpdateChecker::currentVersion() const {
  return m_currentVersion;
}

bool UpdateChecker::isChecking() const {
  return m_checking;
}

const ReleaseInfo &UpdateChecker::availableRelease() const {
  return m_release;
}

QString UpdateChecker::expectedAssetName(const QString &version) {
#if defined(Q_OS_MACOS) || defined(Q_OS_MAC)
  return QStringLiteral("YaMusic-%1-macos-%2.dmg").arg(version, archSuffix());
#elif defined(Q_OS_WIN)
  return QStringLiteral("YaMusic-%1-windows-x64-portable.zip").arg(version);
#else
  return QStringLiteral("YaMusic-%1-linux-x86_64.AppImage").arg(version);
#endif
}

ReleaseAsset UpdateChecker::assetForThisSystem() const {
  if (!m_release.isValid()) return {};

  const QString expected = expectedAssetName(m_release.version);

  for (const ReleaseAsset &asset : m_release.assets) {
    if (asset.name == expected) return asset;
  }

  // Имя могло отличаться (например, сборка сделана вручную) — тогда
  // подойдёт любой файл релиза для этой системы.
  for (const ReleaseAsset &asset : m_release.assets) {
    const QString name = asset.name.toLower();

    if (!name.endsWith(QStringLiteral(".dmg")) && !name.endsWith(QStringLiteral(".zip")) &&
        !name.endsWith(QStringLiteral(".appimage"))) {
      continue;
    }

    if (name.contains(QStringLiteral("macos")) && name.contains(archSuffix())) return asset;
  }

  return {};
}

void UpdateChecker::check() {
  if (m_checking) return;

  if (m_currentVersion.trimmed().isEmpty()) {
    emit checkFailed(QStringLiteral("Версия приложения не задана"));
    return;
  }

  m_checking = true;
  emit checkingChanged();

  QUrlQuery query;
  query.addQueryItem(QStringLiteral("per_page"), QString::number(kMaxReleases));
  const QUrl url(QStringLiteral("https://api.github.com/repos/%1/releases?%2")
                     .arg(m_repository, query.toString(QUrl::FullyEncoded)));

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::UserAgentHeader,
                    QStringLiteral("YaMusic/%1").arg(m_currentVersion));
  request.setRawHeader("Accept", "application/vnd.github+json");

  QNetworkReply *reply = m_network->get(request);
  m_reply = reply;
  connect(reply, &QNetworkReply::finished, this, [this, reply]() { handleReply(reply); });
}

void UpdateChecker::handleReply(QNetworkReply *reply) {
  m_checking = false;
  reply->deleteLater();

  if (reply->error() != QNetworkReply::NoError) {
    emit checkingChanged();
    emit checkFailed(reply->errorString());
    return;
  }

  const QByteArray data = reply->readAll();
  QJsonParseError parseError;
  const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError || !document.isArray()) {
    emit checkingChanged();
    emit checkFailed(QStringLiteral("Не удалось разобрать ответ GitHub"));
    return;
  }

  ReleaseInfo best;

  for (const QJsonValue &value : document.array()) {
    if (!value.isObject()) continue;
    const QJsonObject object = value.toObject();
    if (object.value(QStringLiteral("draft")).toBool()) continue;

    const ReleaseInfo release = releaseFromJson(object);

    if (!release.isValid()) continue;
    if (!isNewerVersion(release.version, m_currentVersion)) continue;
    if (best.isValid() && !isNewerVersion(release.version, best.version)) continue;
    best = release;
  }

  if (!best.isValid()) {
    m_release = {};
    emit checkingChanged();
    emit upToDate(m_currentVersion);
    return;
  }

  m_release = best;
  emit checkingChanged();
  emit updateAvailable(best);
}

ReleaseInfo UpdateChecker::releaseFromJson(const QJsonObject &object) const {
  ReleaseInfo release;
  release.tag = object.value(QStringLiteral("tag_name")).toString().trimmed();
  release.version = release.tag;

  if (release.version.startsWith(QLatin1Char('v')) ||
      release.version.startsWith(QLatin1Char('V'))) {
    release.version.remove(0, 1);
  }

  release.title = object.value(QStringLiteral("name")).toString();
  release.notes = object.value(QStringLiteral("body")).toString();
  release.pageUrl = object.value(QStringLiteral("html_url")).toString();
  release.publishedAt =
      QDateTime::fromString(object.value(QStringLiteral("published_at")).toString(), Qt::ISODate);

  for (const QJsonValue &value : object.value(QStringLiteral("assets")).toArray()) {
    if (!value.isObject()) continue;
    const QJsonObject assetObject = value.toObject();
    ReleaseAsset asset;
    asset.name = assetObject.value(QStringLiteral("name")).toString();
    asset.url = assetObject.value(QStringLiteral("browser_download_url")).toString();
    asset.size = static_cast<qint64>(assetObject.value(QStringLiteral("size")).toDouble());

    if (asset.name.isEmpty() || !asset.url.isValid()) continue;
    release.assets.append(asset);
  }

  // Версия без чисел (тег вида "nightly") сравниваться не может.
  if (versionParts(release.version).isEmpty()) release.version.clear();
  return release;
}

} // namespace Update
