import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import ChecklistApp

ButtonStyle {
    id: control

    buttonPadding: Theme.spacingSmall
    contentItem: Text {
        text: control.text
        font.pixelSize: Theme.fontSizeSmall
        font.weight: Theme.fontWeightNormal
        color: control.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    implicitWidth: contentItem.implicitWidth + control.buttonPadding * 2
    implicitHeight: contentItem.implicitHeight + control.buttonPadding * 2
}
