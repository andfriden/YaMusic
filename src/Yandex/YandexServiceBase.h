#pragma once

#include <QObject>
#include <QString>

class YandexAuth;
class YandexClient;

/*
 * Общая база сервисов Яндекс Музыки.
 *
 * Держит auth-состояние и сетевой клиент, чтобы каждый сервис
 * не создавал их заново. Метод ensureAuthenticated() проверяет
 * авторизацию и подставляет токен в запросы.
 */
class YandexServiceBase : public QObject
{
    Q_OBJECT

public:

    explicit YandexServiceBase(
        YandexAuth *auth,
        QObject *parent = nullptr);

    /*
     * Возвращает true, если пользователь авторизован,
     * и токен уже выставлен в YandexClient.
     * Возвращает false, если авторизации нет — сервис
     * сам решает, как сообщить об ошибке через свои сигналы.
     */
    bool ensureAuthenticated();

protected:

    YandexAuth *m_auth = nullptr;

    YandexClient *m_yandexClient = nullptr;
};