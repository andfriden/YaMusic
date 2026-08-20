#pragma once

#include <QObject>
#include <QString>

class AccountService;
class YandexAuth;

// Coordinates application-level actions exposed to the QML interface.
class AppController : public QObject
{
    Q_OBJECT

public:
    explicit AppController(QObject *parent = nullptr);

    // Simple application connectivity test.
    Q_INVOKABLE void testConnection();

    // Requests the authenticated Yandex Music account.
    Q_INVOKABLE void testYandexApi();

signals:
    // Updates the status message displayed by the QML interface.
    void statusChanged(const QString &message);

private:
    YandexAuth *m_yandexAuth = nullptr;
    AccountService *m_accountService = nullptr;
};
