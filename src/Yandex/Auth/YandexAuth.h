#pragma once

#include <QObject>
#include <QString>

class YandexAuth : public QObject
{
    Q_OBJECT

public:
    explicit YandexAuth(QObject *parent = nullptr);

    bool loadFromEnvironment();

    bool isAuthenticated() const;
    QString token() const;

signals:
    void authenticationChanged();

private:
    QString m_token;
};
