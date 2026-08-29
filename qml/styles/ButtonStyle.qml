import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import ChecklistApp

T.Button {
    id: control

    property color backgroundColor: Theme.borderDefault
    property color backgroundColorHover: Theme.borderDefault
    property color textColor: Theme.textSecondary
    property real buttonRadius: Theme.radiusSmall
    property real buttonPadding: Theme.spacingMedium

    contentItem: Text {
        text: control.text
        font.pixelSize: Theme.fontSizeMedium
        font.weight: Theme.fontWeightMedium
        color: control.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        radius: control.buttonRadius
        color: control.down || control.hovered
               ? control.backgroundColorHover
               : control.backgroundColor
    }

    implicitWidth: contentItem.implicitWidth + control.buttonPadding * 2
    implicitHeight: contentItem.implicitHeight + control.buttonPadding * 2
}
