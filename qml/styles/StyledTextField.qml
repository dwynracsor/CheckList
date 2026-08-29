import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import ChecklistApp

TextField {
    id: field

    property real fieldRadius: Theme.radiusSmall

    placeholderTextColor: Theme.textDisabled
    color: Theme.textSecondary
    font.pixelSize: Theme.fontSizeMedium

    background: Rectangle {
        radius: field.fieldRadius
        border.color: field.activeFocus
                      ? Theme.borderFocus
                      : Theme.borderDefault
        border.width: 1
    }

    leftPadding: Theme.spacingMedium
    rightPadding: Theme.spacingMedium
    topPadding: Theme.spacingSmall
    bottomPadding: Theme.spacingSmall
}
