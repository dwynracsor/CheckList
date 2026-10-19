import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property string text: ""
    property bool editMode: false
    signal textChanged
    signal editingFinished
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    Rectangle {
        id: editor
        color: editMode ? theme.colors.surface : "transparent"
        border.color: editMode ? theme.colors.accent : theme.colors.border
        border.width: 1
        radius: 4
        Layout.fillWidth: true

        TextField {
            id: textField
            anchors.fill: parent
            anchors.margins: 4
            text: root.text
            visible: editMode
            focus: editMode
            onEditingFinished: {
                root.editingFinished()
            }
        }

        Text {
            id: displayText
            anchors.fill: parent
            anchors.margins: 4
            text: root.text
            visible: !editMode
            color: theme.colors.textPrimary
            elide: Text.ElideRight
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.editMode = true
    }
}