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

    // Checkbox
    Rectangle {
        width: 20
        height: 20
        radius: 3
        border.color: root.itemDone ? Theme.textPrimary : Theme.borderDefault
        border.width: 1.5
        color: root.itemDone ? Theme.textPrimary : "transparent"
        Layout.alignment: Qt.AlignVCenter

        Text {
            anchors.centerIn: parent
            text: "\u2713"
            font.pixelSize: 12
            font.weight: Theme.fontWeightBold
            color: "#ffffff"
            visible: root.itemDone
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.toggled(!root.itemDone)
        }
    }

    // Item text
    Text {
        text: root.itemText
        font.pixelSize: Theme.fontSizeMedium
        color: root.itemDone ? Theme.textDisabled : Theme.textSecondary
        font.strikeout: root.itemDone
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
    }

    // Delete button
    Text {
        id: deleteBtn
        text: "X"
        font.pixelSize: Theme.fontSizeSmall
        font.weight: Theme.fontWeightBold
        color: Theme.textDisabled
        Layout.alignment: Qt.AlignVCenter

        Behavior on color { ColorAnimation { duration: 150 } }

        MouseArea {
            anchors.fill: parent
            anchors.margins: -4
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onClicked: root.remove()
            onEntered: deleteBtn.color = Theme.danger
            onExited: deleteBtn.color = Theme.textDisabled
        }
    }
}
