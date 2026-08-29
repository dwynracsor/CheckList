import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

ColumnLayout {
    id: root

    property int taskIndex: 0
    property int correctionIndex: 0

    signal itemToggled(int taskIdx, int corrIdx, int itemIdx, bool done)
    signal itemRemoved(int taskIdx, int corrIdx, int itemIdx)
    signal nameEdited(int taskIdx, int corrIdx, string newName)
    signal itemAdded(int taskIdx, int corrIdx, string text)
    signal correctionDeleted(int taskIdx, int corrIdx)

    // Get correction from controller — re-evaluates when indices change
    property var correction: {
        var task = taskController.getTask(root.taskIndex)
        if (task && root.correctionIndex >= 0 && root.correctionIndex < task.correctionCount) {
            return task.getCorrection(root.correctionIndex)
        }
        return null
    }

    // Block background
    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: blockColumn.height + 32
        color: Theme.formBackground
        radius: Theme.radiusMedium
        border.color: "transparent"

        // Left blue border
        Rectangle {
            width: 3
            height: parent.height
            color: Theme.primary
            radius: 2
        }

        ColumnLayout {
            id: blockColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: Theme.spacingMedium
            anchors.leftMargin: Theme.spacingLarge
            spacing: Theme.spacingSmall

            // Header
            RowLayout {
                Layout.fillWidth: true

                InlineEditor {
                    id: corrTitleEditor
                    Layout.fillWidth: true
                    text: root.correction ? root.correction.name : ""
                    fontSize: Theme.fontSizeMedium
                    fontWeight: Theme.fontWeightBold
                    textColor: Theme.textPrimary
                    onEdited: function(newName) {
                        root.nameEdited(root.taskIndex, root.correctionIndex, newName)
                    }
                }

                Rectangle {
                    Layout.preferredWidth: corrDateText.implicitWidth + 24
                    Layout.preferredHeight: corrDateText.implicitHeight + 12
                    radius: 20
                    color: Theme.borderLight

                    Text {
                        id: corrDateText
                        anchors.centerIn: parent
                        text: "Creada: " + (root.correction ? root.correction.date : "")
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.textMuted
                    }
                }
            }

            // Checklist items
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0

                Repeater {
                    model: root.correction ? root.correction.itemCount : 0

                    ChecklistDelegate {
                        Layout.fillWidth: true
                        itemText: root.correction ? root.correction.getItem(index).text : ""
                        itemDone: root.correction ? root.correction.getItem(index).done : false
                        itemIndex: index

                        onToggled: function(done) {
                            root.itemToggled(root.taskIndex, root.correctionIndex, index, done)
                        }

                        onRemove: {
                            root.itemRemoved(root.taskIndex, root.correctionIndex, index)
                        }
                    }
                }

                Text {
                    visible: root.correction ? root.correction.itemCount === 0 : true
                    text: qsTr("Sin ítems")
                    font.pixelSize: Theme.fontSizeMedium
                    font.italic: true
                    color: Theme.textDisabled
                    Layout.topMargin: Theme.spacingSmall
                }
            }

            // Add item row
            AddItemRow {
                Layout.fillWidth: true
                placeholderText: qsTr("Agregar ítem...")
                onAddItem: function(text) {
                    root.itemAdded(root.taskIndex, root.correctionIndex, text)
                }
            }

            // Actions
            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: Theme.spacingSmall

                Item { Layout.fillWidth: true }

                DangerButton {
                    text: qsTr("Eliminar corrección")
                    onClicked: deleteCorrDialog.open()
                }
            }
        }
    }

    ConfirmationDialog {
        id: deleteCorrDialog
        titleText: "Eliminar corrección"
        messageText: "¿Estás seguro de que quieres eliminar esta corrección? Esta acción se puede deshacer."
        confirmText: "Eliminar"
        onConfirmed: root.correctionDeleted(root.taskIndex, root.correctionIndex)
    }
}
