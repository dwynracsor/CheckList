import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import ChecklistApp

TextArea {
    id: area

    property real areaRadius: Theme.radiusSmall
    property bool autoResize: true

    wrapMode: TextArea.Wrap
    placeholderTextColor: Theme.textDisabled
    color: Theme.textSecondary
    font.pixelSize: Theme.fontSizeMedium

    background: Rectangle {
        radius: area.areaRadius
        border.color: area.activeFocus
                      ? Theme.borderFocus
                      : Theme.borderDefault
        border.width: 1
    }

    leftPadding: Theme.spacingMedium
    rightPadding: Theme.spacingMedium
    topPadding: Theme.spacingSmall
    bottomPadding: Theme.spacingSmall

    onTextChanged: {
        if (autoResize) {
            implicitHeight = Math.max(30, contentHeight + topPadding + bottomPadding)
        }
    }
}
