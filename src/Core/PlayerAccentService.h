#pragma once

#include <QColor>
#include <QHash>
#include <QObject>
#include <QString>

class QNetworkAccessManager;

class PlayerAccentService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QColor accentColor
        READ accentColor
        NOTIFY accentColorChanged)

public:

    explicit PlayerAccentService(
        QObject *parent = nullptr);

    QColor accentColor() const;

    Q_INVOKABLE void updateForCover(
        const QString &coverUri);


    signals:

        void accentColorChanged();


private:

    QColor calculateDominantColor(
        const QByteArray &data) const;

    QString createUrl(
        QString uri) const;


private:

    QNetworkAccessManager *
        m_networkManager = nullptr;

    QHash<QString, QColor>
        m_cache;

    QColor m_accentColor;

    QString m_currentCoverUri;
};