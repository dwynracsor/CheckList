import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

ColumnLayout {
    id: root

    property int taskIndex: 0
    property string taskName: ""
    property string taskDate: ""
    property int taskItemCount: 0
    property int taskCorrectionCount: 0

    signal itemToggled(int taskIdx, int corrIdx, int itemIdx, bool done)
    signal itemRemoved(int taskIdx, int corrIdx, int itemIdx)
    signal nameEdited(int taskIdx, int corrIdx, string newName)
    signal itemAdded(int taskIdx, int corrIdx, string text)
    signal taskDeleted(int taskIdx)
    signal correctionDeleted(int taskIdx, int corrIdx)
    signal correctionAdded(int taskIdx, string name)

    // Get task from controller
    property var task: taskController.getTask(root.taskIndex)

    // Card background
    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: contentColumn.height + 32
        color: Theme.cardBackground
        radius: Theme.radiusCard
        border.color: Theme.borderDefault
        border.width: 1

        ColumnLayout {
            id: contentColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: Theme.spacingLarge
            spacing: Theme.spacingSmall

            // Header
            RowLayout {
                id: headerRow
                Layout.fillWidth: true

                InlineEditor {
                    id: titleEditor
                    Layout.fillWidth: true
                    text: root.taskName
                    fontSize: Theme.fontSizeLarge
                    fontWeight: Theme.fontWeightBold
                    textColor: Theme.textPrimary
                    onEdited: function(newName) {
                        root.nameEdited(root.taskIndex, -1, newName)
                    }
                }

                Rectangle {
                    Layout.preferredWidth: dateText.implicitWidth + 24
                    Layout.preferredHeight: dateText.implicitHeight + 12
                    radius: 20
                    color: Theme.borderLight

                    Text {
                        id: dateText
                        anchors.centerIn: parent
                        text: "Creada: " + root.taskDate
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.textMuted
                    }
                }
            }

            // Main checklist items
            ColumnLayout {
                id: mainChecklist
                Layout.fillWidth: true
                spacing: 0

                Repeater {
                    model: root.task ? root.task.itemCount : 0

                    ChecklistDelegate {
                        Layout.fillWidth: true
                        itemText: root.task ? root.task.getItem(index).text : ""
                        itemDone: root.task ? root.task.getItem(index).done : false
                        itemIndex: index

                        onToggled: function(done) {
                            root.itemToggled(root.taskIndex, -1, index, done)
                        }

                        onRemove: {
                            root.itemRemoved(root.taskIndex, -1, index)
                        }
                    }
                }

                // Empty state for items
                Text {
                    visible: root.taskItemCount === 0
                    text: "Sin ítems"
                    font.pixelSize: Theme.fontSizeMedium
                    font.italic: true
                    color: Theme.textDisabled
                    Layout.topMargin: Theme.spacingSmall
                }
            }

            // Add item row for mother task
            AddItemRow {
                Layout.fillWidth: true
                placeholderText: "Agregar ítem a esta corrección..."
                onAddItem: function(text) {
                    root.itemAdded(root.taskIndex, -1, text)
                }
            }

            // Corrections section
            ColumnLayout {
                id: correctionsSection
                Layout.fillWidth: true
                spacing: Theme.spacingSmall

                Repeater {
                    model: root.task ? root.task.correctionCount : 0

                    CorrectionBlock {
                        Layout.fillWidth: true
                        correctionIndex: index
                        taskIndex: root.taskIndex

                        onItemToggled: function(taskIdx, corrIdx, itemIdx, done) {
                            root.itemToggled(taskIdx, corrIdx, itemIdx, done)
                        }

                        onItemRemoved: function(taskIdx, corrIdx, itemIdx) {
                            root.itemRemoved(taskIdx, corrIdx, itemIdx)
                        }

                        onNameEdited: function(taskIdx, corrIdx, newName) {
                            root.nameEdited(taskIdx, corrIdx, newName)
                        }

                        onItemAdded: function(taskIdx, corrIdx, text) {
                            root.itemAdded(taskIdx, corrIdx, text)
                        }

                        onCorrectionDeleted: function(taskIdx, corrIdx) {
                            root.correctionDeleted(taskIdx, corrIdx)
                        }
                    }
                }
            }

            // Add correction button
            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: Theme.spacingSmall

                Item { Layout.fillWidth: true }

                AppControls.PrimaryButton {
                    text: "+ Agregar corrección"
                    onClicked: {
                        // Show inline input for correction name
                        correctionNameInput.visible = true
                        correctionNameInput.forceActiveFocus()
                    }
                }
            }

            // Inline correction name input (hidden by default)
            RowLayout {
                id: correctionNameInput
                visible: false
                Layout.fillWidth: true
                spacing: Theme.spacingSmall

                AppControls.StyledTextField {
                    id: newCorrNameField
                    Layout.fillWidth: true
                    placeholderText: "Nombre de la corrección..."
                    onAccepted: addCorrectionBtn.clicked()
                }

                AppControls.PrimaryButton {
                    id: addCorrectionBtn
                    text: "Agregar"
                    onClicked: {
                        var name = newCorrNameField.text.trim()
                        if (name !== "") {
                            root.correctionAdded(root.taskIndex, name)
                            newCorrNameField.text = ""
                            correctionNameInput.visible = false
                        }
                    }
                }

                AppControls.ButtonStyle {
                    text: "Cancelar"
                    onClicked: {
                        newCorrNameField.text = ""
                        correctionNameInput.visible = false
                    }
                }
            }

            // Actions row
            RowLayout {
                id: actionsRow
                Layout.fillWidth: true
                Layout.topMargin: Theme.spacingSmall

                Item { Layout.fillWidth: true }

                AppControls.DangerButton {
                    text: "Eliminar"
                    onClicked: root.taskDeleted(root.taskIndex)
                }
            }
        }
    }
}
