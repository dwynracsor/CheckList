import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: qsTr("Checklist")

    ColumnLayout {
        anchors.fill: true
        spacing: 20
        padding: 20

        Text {
            text: qsTr("Checklist Application")
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignCenter
        }

        Text {
            text: qsTr("C++ con Qt6 - Funcionando correctamente")
            font.pixelSize: 16
            Layout.alignment: Qt.AlignCenter
        }

        Button {
            text: qsTr("Prueba")
            Layout.alignment: Qt.AlignCenter
        }
    }
}