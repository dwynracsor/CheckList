import QtQuick
import QtQuick.Controls
import ChecklistApp

SystemTrayIcon {
    id: root

    property bool isVisible: true
    property string tooltip: "Correcciones"

    signal showWindow()
    signal hideWindow()
    signal quitApp()

    visible: root.isVisible
    iconSource: "qrc:/qt/qml/ChecklistApp/resources/icons/app.svg"
    tooltip: root.tooltip

    menu: Menu {
        MenuItem {
            text: "Mostrar ventana"
            onTriggered: root.showWindow()
        }

        MenuItem {
            text: "Ocultar ventana"
            onTriggered: root.hideWindow()
        }

        MenuSeparator {}

        MenuItem {
            text: "Salir"
            onTriggered: root.quitApp()
        }
    }

    onActivated: function(reason) {
        if (reason === SystemTrayIcon.Trigger) {
            root.showWindow()
        }
    }
}
