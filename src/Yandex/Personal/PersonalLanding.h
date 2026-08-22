#pragma once

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>

#include "../../Models/PersonalPlaylist.h"
#include "../../Models/RecentListeningItem.h"

struct PersonalLandingItem
{
    QString id;
    QString type;

    QJsonObject data;
};

struct PersonalLandingSection
{
    QString id;
    QString title;
    QString type;
    QString typeForFrom;
    QString description;

    QList<PersonalLandingItem> items;
};

class YandexAuth;
class YandexClient;

class PersonalLanding : public QObject
{
    Q_OBJECT

public:
    explicit PersonalLanding(
        YandexAuth *auth,
        QObject *parent = nullptr);

    void load();

    signals:
        void loaded(
            const QList<PersonalLandingSection> &sections);

    void personalPlaylistsReceived(
        const QList<PersonalPlaylist> &playlists);

    void recentListeningReceived(
        const QList<RecentListeningItem> &items);

    void errorOccurred(
        const QString &message);

private:
    PersonalLandingItem parseItem(
        const QJsonObject &object) const;

    PersonalLandingSection parseSection(
        const QJsonObject &object) const;

    PersonalPlaylist parsePersonalPlaylist(
        const PersonalLandingItem &item) const;

    RecentListeningItem parseRecentListeningItem(
        const PersonalLandingItem &item) const;

private:
    YandexAuth *m_auth = nullptr;

    YandexClient *m_yandexClient = nullptr;
};