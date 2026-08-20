#pragma once

#include <QObject>
#include <QString>

class YandexAuth;
class YandexClient;

class AppController : public QObject
{
    Q_OBJECT

public:
    explicit AppController(QObject *parent = nullptr);

    Q_INVOKABLE void testConnection();
    Q_INVOKABLE void testYandexApi();

signals:
    void statusChanged(const QString &message);

private:
    YandexAuth *m_yandexAuth = nullptr;
    YandexClient *m_yandexClient = nullptr;
};
