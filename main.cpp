#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "src/Core/AppController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<AppController>("YaMusic.Core", 1, 0, "AppController");

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() {
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.loadFromModule("YaMusic", "Main");

    return QGuiApplication::exec();
}
