import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

RowLayout {
    id: root

    property string text: ""
    property real fontSize: Theme.fontSizeMedium
    property int fontWeight: Theme.fontWeightBold
    property color textColor: Theme.textPrimary

    signal edited(string newName)

    property bool editing: false

    spacing: Theme.spacingSmall

    // Display mode
    Text {
        id: displayText
        visible: !root.editing
        text: root.text
        font.pixelSize: root.fontSize
        font.weight: root.fontWeight
        color: root.textColor
        Layout.fillWidth: true
    }

    Button {
        id: editBtn
        visible: !root.editing
        text: "✏"
        font.pixelSize: Theme.fontSizeSmall
        background: Rectangle { color: "transparent" }
        contentItem: Text {
            text: parent.text
            color: Theme.textDisabled
        }
        onClicked: {
            root.editing = true
            editField.text = root.text
            editField.forceActiveFocus()
            editField.selectAll()
        }
    }

    // Edit mode
    StyledTextField {
        id: editField
        visible: root.editing
        Layout.fillWidth: true
        font.pixelSize: root.fontSize
        font.weight: root.fontWeight
        Keys.onReturnPressed: saveEdit()
        Keys.onEnterPressed: saveEdit()
        Keys.onEscapePressed: cancelEdit()
    }

    PrimaryButton {
        visible: root.editing
        text: "Guardar"
        onClicked: saveEdit()
    }

    function saveEdit() {
        var newName = editField.text.trim()
        if (newName !== "") {
            root.edited(newName)
            root.text = newName
        }
        root.editing = false
    }

    function cancelEdit() {
        root.editing = false
    }
}
