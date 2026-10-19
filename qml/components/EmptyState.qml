import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property string text: qsTr("No items yet")
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    RowLayout {
        Layout.alignment: Qt.AlignCenter
        spacing: theme.spacing.small

        Text {
            text: root.text
            color: theme.colors.textSecondary
            font.italic: true
            font.pixelSize: 14
        }
    }
}