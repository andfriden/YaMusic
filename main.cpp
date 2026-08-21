#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "src/Core/AppController.h"
#include "src/Yandex/Catalog/CoverImageProvider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Registers the application controller for use from QML.
    qmlRegisterType<AppController>(
        "YaMusic.Core",
        1,
        0,
        "AppController");

    QQmlApplicationEngine engine;

    // Registers the Yandex Music artwork provider.
    engine.addImageProvider(
        "yandex",
        new CoverImageProvider());

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() {
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.loadFromModule(
        "YaMusic",
        "Main");

    return QGuiApplication::exec();
}
