import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T

QtObject {
    // Button base style
    component ButtonStyle: T.Button {
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

    // Primary button (blue)
    component PrimaryButton: ButtonStyle {
        backgroundColor: Theme.primary
        backgroundColorHover: Theme.primaryHover
        textColor: "#ffffff"
    }

    // Success button (green)
    component SuccessButton: ButtonStyle {
        backgroundColor: Theme.success
        backgroundColorHover: Theme.successHover
        textColor: "#ffffff"
    }

    // Danger button (red)
    component DangerButton: ButtonStyle {
        backgroundColor: Theme.danger
        backgroundColorHover: Theme.dangerHover
        textColor: "#ffffff"
    }

    // Small button variant
    component SmallButton: ButtonStyle {
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

    // Text field
    component StyledTextField: TextField {
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

    // Text area (auto-resize)
    component StyledTextArea: TextArea {
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

    // CheckBox
    component StyledCheckBox: CheckBox {
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
}
