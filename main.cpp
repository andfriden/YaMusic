#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "src/Core/AppController.h"
#include "src/Core/AuthController.h"

#include "src/Yandex/Account/AccountService.h"
#include "src/Yandex/Auth/YandexAuth.h"
#include "src/Yandex/Catalog/CoverImageProvider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("YaMusic");
    QCoreApplication::setOrganizationDomain("yandex.ru");
    QCoreApplication::setApplicationName("YaMusic");

    YandexAuth auth;

    AccountService accountService(
        &auth
    );

    AuthController authController(
        &auth,
        &accountService
    );

    AppController appController(
        &auth,
        &accountService
    );

    QQmlApplicationEngine engine;

    engine.addImageProvider(
        "yandex",
        new CoverImageProvider()
    );

    engine.rootContext()->setContextProperty(
        "authController",
        &authController
    );

    engine.rootContext()->setContextProperty(
        "appController",
        &appController
    );

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() {
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );

    engine.loadFromModule(
        "YaMusic",
        "Main"
    );

    return QGuiApplication::exec();
}