import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

ColumnLayout {
    id: root

    signal taskSaved(string name, var items)

    // Animated visibility
    implicitHeight: visible ? formColumn.implicitHeight : 0
    clip: true
    Behavior on implicitHeight {
        NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
    }

    ColumnLayout {
        id: formColumn
        width: parent.width
        spacing: Theme.spacingMedium
        opacity: root.visible ? 1 : 0
        Behavior on opacity {
            NumberAnimation { duration: 250 }
        }

        // Name field
        Text {
            text: "Nombre de la corrección"
            font.pixelSize: Theme.fontSizeSmall
            font.weight: Theme.fontWeightMedium
            color: Theme.textSecondary
        }

        AppControls.StyledTextField {
            id: nameField
            Layout.fillWidth: true
            placeholderText: "Ej: Preparar presentación"
            onAccepted: saveTaskBtn.clicked()
        }

        // Checklist builder
        ChecklistBuilder {
            id: checklistBuilder
            Layout.fillWidth: true
            Layout.topMargin: Theme.spacingSmall
        }

        // Save button
        AppControls.SuccessButton {
            id: saveTaskBtn
            text: "Guardar corrección"
            Layout.fillWidth: true
            Layout.topMargin: Theme.spacingSmall
            onClicked: {
                if (nameField.text.trim() === "") {
                    nameField.forceActiveFocus()
                    return
                }

                var items = checklistBuilder.getItems()
                root.taskSaved(nameField.text.trim(), items)

                // Reset form
                nameField.text = ""
                checklistBuilder.clear()
                nameField.forceActiveFocus()
            }
        }
    }
}
