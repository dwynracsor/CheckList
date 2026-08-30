import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

ColumnLayout {
    id: root

    property int taskIndex: 0
    property string listName: ""
    property string listDate: ""
    property bool isMinimized: false

    signal itemToggled(int taskIdx, int itemIdx, bool done)
    signal itemRemoved(int taskIdx, int itemIdx)
    signal itemAdded(int taskIdx, string text)
    signal nameEdited(int taskIdx, string newName)
    signal listDeleted(int taskIdx)

    property var task: (taskController.taskCount > 0 && root.taskIndex >= 0 && root.taskIndex < taskController.taskCount)
        ? taskController.getTask(root.taskIndex)
        : null

    spacing: 0

    // List header
    RowLayout {
        Layout.fillWidth: true
        spacing: Theme.spacingSmall

        // Edit mode: text field
        StyledTextField {
            id: nameEditField
            visible: false
            Layout.fillWidth: true
            text: root.listName
            font.pixelSize: Theme.fontSizeLarge
            font.weight: Theme.fontWeightBold
            Keys.onReturnPressed: saveName()
            Keys.onEnterPressed: saveName()
            Keys.onEscapePressed: cancelEdit()
        }

        // Display mode: list name + pencil
        RowLayout {
            visible: !nameEditField.visible
            Layout.fillWidth: true
            spacing: 6

            Text {
                text: root.listName
                font.pixelSize: Theme.fontSizeLarge
                font.weight: Theme.fontWeightBold
                color: Theme.textPrimary
                Layout.fillWidth: true
            }

            // Pencil edit button
            Text {
                text: "\u270E"
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.textDisabled
                Layout.alignment: Qt.AlignVCenter

                MouseArea {
                    anchors.fill: parent
                    anchors.margins: -4
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onEntered: parent.color = Theme.textSecondary
                    onExited: parent.color = Theme.textDisabled
                    onClicked: {
                        nameEditField.text = root.listName
                        nameEditField.visible = true
                        nameEditField.forceActiveFocus()
                        nameEditField.selectAll()
                    }
                }
            }
        }

        // Minimize button
        Text {
            text: "-"
            font.pixelSize: Theme.fontSizeLarge
            font.weight: Theme.fontWeightBold
            color: Theme.textMuted
            Layout.alignment: Qt.AlignVCenter

            MouseArea {
                anchors.fill: parent
                anchors.margins: -4
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
                onEntered: parent.color = Theme.textPrimary
                onExited: parent.color = Theme.textMuted
                onClicked: root.isMinimized = !root.isMinimized
            }
        }

        // Delete list button
        Text {
            text: "X"
            font.pixelSize: Theme.fontSizeMedium
            font.weight: Theme.fontWeightBold
            color: Theme.textMuted
            Layout.alignment: Qt.AlignVCenter

            MouseArea {
                anchors.fill: parent
                anchors.margins: -4
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
                onEntered: parent.color = Theme.danger
                onExited: parent.color = Theme.textMuted
                onClicked: deleteListDialog.open()
            }
        }
    }

    // Items area (hidden when minimized)
    ColumnLayout {
        Layout.fillWidth: true
        visible: !root.isMinimized
        spacing: 0
        Layout.topMargin: Theme.spacingTiny

        // Checklist items
        Repeater {
            model: root.task ? root.task.itemCount : 0

            ListItemDelegate {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                itemText: root.task ? root.task.getItem(index).text : ""
                itemDone: root.task ? root.task.getItem(index).done : false
                itemIndex: index

                onToggled: function(done) {
                    root.itemToggled(root.taskIndex, index, done)
                }

                onRemove: {
                    root.itemRemoved(root.taskIndex, index)
                }
            }
        }

        // Empty state
        Text {
            visible: root.task ? root.task.itemCount === 0 : true
            text: "No items yet"
            font.pixelSize: Theme.fontSizeMedium
            font.italic: true
            color: Theme.textDisabled
            Layout.topMargin: Theme.spacingSmall
        }

        // Progress bar and date row
        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: Theme.spacingSmall
            Layout.bottomMargin: Theme.spacingSmall
            spacing: Theme.spacingMedium

            // Progress bar
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 6
                radius: 3
                color: Theme.borderDefault

                Rectangle {
                    width: {
                        if (!root.task || root.task.itemCount === 0) return 0
                        var done = 0
                        for (var i = 0; i < root.task.itemCount; i++) {
                            if (root.task.getItem(i).done) done++
                        }
                        return (done / root.task.itemCount) * parent.width
                    }
                    height: parent.height
                    radius: 3
                    color: Theme.primary

                    Behavior on width { NumberAnimation { duration: 300; easing.type: Easing.OutQuad } }
                }
            }

            // Date
            Text {
                text: root.listDate
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.textDisabled
            }
        }

        // Add new item button
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            radius: Theme.radiusMedium
            color: "transparent"
            border.color: Theme.borderDefault
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: "+ New item"
                font.pixelSize: Theme.fontSizeMedium
                font.weight: Theme.fontWeightMedium
                color: Theme.textMuted
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    newItemField.visible = !newItemField.visible
                    if (newItemField.visible) newItemField.forceActiveFocus()
                }
            }

            // Inline new item input
            RowLayout {
                id: newItemField
                visible: false
                anchors.fill: parent
                anchors.margins: Theme.spacingSmall
                spacing: Theme.spacingSmall

                StyledTextField {
                    id: newItemInput
                    Layout.fillWidth: true
                    placeholderText: "Item description"
                    font.pixelSize: Theme.fontSizeSmall
                    Keys.onReturnPressed: addNewItem()
                    Keys.onEnterPressed: addNewItem()
                    Keys.onEscapePressed: {
                        newItemField.visible = false
                        newItemInput.text = ""
                    }
                }

                Text {
                    text: "+"
                    font.pixelSize: 18
                    font.weight: Theme.fontWeightBold
                    color: "#ffffff"

                    MouseArea {
                        anchors.fill: parent
                        anchors.margins: -4
                        cursorShape: Qt.PointingHandCursor
                        onClicked: addNewItem()
                    }
                }
            }
        }
    }

    // Confirmation dialog for deleting list
    Dialog {
        id: deleteListDialog
        title: "Delete list"
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Cancel

        ColumnLayout {
            spacing: Theme.spacingMedium

            Text {
                text: "Are you sure you want to delete this list?"
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.textSecondary
            }

            DangerButton {
                text: "Delete"
                onClicked: {
                    root.listDeleted(root.taskIndex)
                    deleteListDialog.close()
                }
                Layout.alignment: Qt.AlignRight
            }
        }
    }

    function saveName() {
        var newName = nameEditField.text.trim()
        if (newName !== "") {
            root.nameEdited(root.taskIndex, newName)
        }
        nameEditField.visible = false
    }

    function cancelEdit() {
        nameEditField.visible = false
    }

    function addNewItem() {
        var text = newItemInput.text.trim()
        if (text === "") return
        root.itemAdded(root.taskIndex, text)
        newItemInput.text = ""
        newItemField.visible = false
    }
}
