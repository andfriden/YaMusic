#pragma once

#include <QObject>
#include <QString>

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
    YandexClient *m_yandexClient = nullptr;
};
