#include "AuthController.h"

#include "../Yandex/Account/AccountService.h"
#include "../Yandex/Auth/YandexAuth.h"

AuthController::AuthController(
    YandexAuth *auth,
    AccountService *accountService,
    QObject *parent)
    : QObject(parent)
    , m_auth(auth)
    , m_accountService(accountService)
{
    if (m_accountService == nullptr)
        return;

    connect(
        m_accountService,
        &AccountService::accountReceived,
        this,
        [this](const Account &account) {

            m_account = account;

            const bool wasAuthenticated =
                m_authenticated;

            m_authenticated = true;

            emit accountChanged();

            if (wasAuthenticated != m_authenticated)
                emit authenticatedChanged();
        });

    connect(
        m_accountService,
        &AccountService::errorOccurred,
        this,
        [this](const QString &message) {

            const bool wasAuthenticated =
                m_authenticated;

            m_authenticated = false;

            if (wasAuthenticated != m_authenticated)
                emit authenticatedChanged();

            emit errorOccurred(message);
        });

    if (m_auth != nullptr &&
        m_auth->isAuthenticated()) {

        loadAccount();
    }
}

bool AuthController::authenticated() const
{
    return m_authenticated;
}

QString AuthController::displayName() const
{
    return m_account.displayName;
}

QString AuthController::login() const
{
    return m_account.login;
}

QString AuthController::extractToken(
    const QString &input) const
{
    const QString value = input.trimmed();

    const QString marker = "access_token=";

    const int start =
        value.indexOf(marker);

    // Обычный токен — оставляем как есть.
    if (start < 0)
        return value;

    const int tokenStart =
        start + marker.length();

    const int tokenEnd =
        value.indexOf('&', tokenStart);

    if (tokenEnd < 0)
        return value.mid(tokenStart).trimmed();

    return value
        .mid(
            tokenStart,
            tokenEnd - tokenStart
        )
        .trimmed();
}

void AuthController::loginWithToken(
    const QString &token)
{
    if (m_auth == nullptr) {
        emit errorOccurred(
            "Сервис авторизации недоступен");

        return;
    }

    const QString extractedToken =
        extractToken(token);

    if (extractedToken.isEmpty()) {
        emit errorOccurred(
            "Не удалось найти OAuth-токен");

        return;
    }

    if (!m_auth->setToken(extractedToken)) {
        emit errorOccurred(
            "Не удалось сохранить токен");

        return;
    }

    loadAccount();
}

void AuthController::logout()
{
    if (m_auth == nullptr)
        return;

    if (!m_auth->clearToken()) {
        emit errorOccurred(
            "Не удалось удалить токен");

        return;
    }

    m_account = Account{};

    const bool wasAuthenticated =
        m_authenticated;

    m_authenticated = false;

    emit accountChanged();

    if (wasAuthenticated != m_authenticated)
        emit authenticatedChanged();
}

void AuthController::loadAccount()
{
    if (m_accountService == nullptr)
        return;

    m_accountService->loadAccount();
}