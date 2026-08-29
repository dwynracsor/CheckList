#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QDebug>

#include "domain/ChecklistItem.h"
#include "domain/Correction.h"
#include "domain/Task.h"
#include "persistence/JsonStorage.h"
#include "controllers/TaskController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Checklist");
    app.setOrganizationName("ChecklistApp");
    app.setApplicationVersion("1.0.0");

    QQuickStyle::setStyle("Basic");

    qmlRegisterType<ChecklistItem>("ChecklistApp", 1, 0, "ChecklistItem");
    qmlRegisterType<Correction>("ChecklistApp", 1, 0, "Correction");
    qmlRegisterType<Task>("ChecklistApp", 1, 0, "Task");
    qmlRegisterType<JsonStorage>("ChecklistApp", 1, 0, "JsonStorage");
    qmlRegisterType<TaskController>("ChecklistApp", 1, 0, "TaskController");

    QQmlApplicationEngine engine;

    static TaskController taskController;
    engine.rootContext()->setContextProperty("taskController", &taskController);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() {
            qCritical() << "QML object creation failed!";
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::warnings,
        &app,
        [](const QList<QQmlError> &warnings) {
            for (const QQmlError &error : warnings)
                qWarning() << "QML Warning:" << error.toString();
        }
    );

    engine.loadFromModule("ChecklistApp", "Main");

    return app.exec();
}
