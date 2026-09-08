#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "src/Core/AppController.h"
#include "src/Yandex/Catalog/CoverImageProvider.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    // =============================================================
    // Stable application identity
    //
    // This must be set before any QSettings object is created.
    // =============================================================

    QCoreApplication::setOrganizationName(
        "YaMusic"
    );

    QCoreApplication::setOrganizationDomain(
        "yandex.ru"
    );

    QCoreApplication::setApplicationName(
        "YaMusic"
    );


    // =============================================================
    // QML registration
    // =============================================================

    qmlRegisterType<AppController>(
        "YaMusic.Core",
        1,
        0,
        "AppController"
    );


    // =============================================================
    // QML engine
    // =============================================================

    QQmlApplicationEngine engine;


    // =============================================================
    // Yandex artwork provider
    // =============================================================

    engine.addImageProvider(
        "yandex",
        new CoverImageProvider()
    );


    // =============================================================
    // QML creation failure
    // =============================================================

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() {
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );


    // =============================================================
    // Main QML module
    // =============================================================

    engine.loadFromModule(
        "YaMusic",
        "Main"
    );


    return QGuiApplication::exec();
}