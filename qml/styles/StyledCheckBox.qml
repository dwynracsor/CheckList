import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import ChecklistApp

CheckBox {
    id: check

    property color checkedColor: Theme.primary

    indicator: Rectangle {
        width: 18
        height: 18
        x: check.leftPadding
        y: parent.height / 2 - height / 2
        radius: Theme.radiusSmall
        border.color: check.checked
                      ? check.checkedColor
                      : Theme.borderDefault
        border.width: 1.5

        Rectangle {
            x: 4
            y: 4
            width: 10
            height: 10
            radius: 2
            color: check.checkedColor
            visible: check.checked
        }
    }

    contentItem: Text {
        text: check.text
        font.pixelSize: Theme.fontSizeMedium
        color: Theme.textSecondary
        verticalAlignment: Text.AlignVCenter
        leftPadding: check.indicator.width + check.spacing
    }
}
