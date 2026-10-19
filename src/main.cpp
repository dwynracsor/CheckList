#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "controllers/TaskControllerQml.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    // QQuickStyle::setStyle("Fluent");

    auto controller = std::make_shared<checklist::TaskControllerQml>();
    controller->load();

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("taskController", controller.get());

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}