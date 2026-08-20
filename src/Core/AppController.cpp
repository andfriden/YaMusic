#include "AppController.h"

#include <QDebug>

AppController::AppController(QObject *parent)
    : QObject(parent)
{
}

void AppController::testConnection()
{
    qDebug() << "AppController is working";
    emit statusChanged("AppController is working");
}