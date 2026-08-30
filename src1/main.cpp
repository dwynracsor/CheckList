#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("CheckList");
    app.setStyle("Fusion");

    // Fuente monoespaciada global (con fallback si no está instalada).
    QFont mono("JetBrains Mono");
    if (!QFontInfo(mono).exactMatch()) {
        mono = QFont("Cascadia Mono");
        if (!QFontInfo(mono).exactMatch()) {
            mono.setFamily("Monospace");
            mono.setStyleHint(QFont::Monospace);
        }
    }
    mono.setPointSize(10);
    app.setFont(mono);

    MainWindow window;
    window.show();

    return app.exec();
}
