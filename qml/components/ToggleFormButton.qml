import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

Button {
    id: root

    property bool isOpen: true

    text: isOpen ? "Ocultar formulario" : "Crear nueva corrección"

    contentItem: RowLayout {
        spacing: Theme.spacingSmall

        Text {
            text: root.isOpen ? "▲" : "▼"
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.textSecondary
        }

        Text {
            text: root.text
            font.pixelSize: Theme.fontSizeMedium
            font.weight: Theme.fontWeightMedium
            color: Theme.textSecondary
        }
    }

    background: Rectangle {
        radius: Theme.radiusMedium
        border.color: root.hovered
                      ? Theme.borderFocus
                      : Theme.borderDefault
        border.width: 1
        color: root.hovered ? Theme.borderLight : "#ffffff"
    }

    onClicked: {
        taskForm.visible = !taskForm.visible
    }

    implicitWidth: contentItem.implicitWidth + 32
    implicitHeight: contentItem.implicitHeight + 16
}
