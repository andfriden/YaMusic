#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include "../Models/Account.h"

class YandexClient : public QObject
{
    Q_OBJECT

public:
    explicit YandexClient(QObject *parent = nullptr);

    void setToken(const QString &token);

    bool hasToken() const;

    QNetworkReply *get(const QString &path);

    void getAccountStatus();


signals:
    void requestError(const QString &message);
    void accountReceived(const Account &account); 

private:
    QNetworkRequest createRequest(const QString &path) const;

    QNetworkAccessManager m_networkManager;
    QString m_token;
};
