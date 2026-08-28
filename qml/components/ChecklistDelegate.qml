import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

RowLayout {
    id: root

    property string itemText: ""
    property bool itemDone: false
    property int itemIndex: 0

    signal toggled(bool done)
    signal remove()

    spacing: Theme.spacingSmall

    AppControls.StyledCheckBox {
        id: checkbox
        checked: root.itemDone
        onCheckedChanged: root.toggled(checked)
    }

    Text {
        text: root.itemText
        font.pixelSize: Theme.fontSizeMedium
        color: root.itemDone ? Theme.textDisabled : Theme.textSecondary
        font.strikeout: root.itemDone
        Layout.fillWidth: true
    }

    Button {
        text: "✕"
        font.pixelSize: Theme.fontSizeSmall
        background: Rectangle { color: "transparent" }
        contentItem: Text {
            text: parent.text
            color: Theme.textDisabled
        }
        onClicked: root.remove()
    }
}
