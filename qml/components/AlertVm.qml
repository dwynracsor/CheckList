import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var alert  // { taskId, taskName, category, message1, message2 }
    signal onDismiss
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    RowLayout {
        Layout.fillWidth: true
        spacing: theme.spacing.small
        padding: theme.spacing.small

        Rectangle {
            color: theme.colors.alertBackground
            radius: 6
            border.color: theme.colors.border
            border.width: 1
            anchors.fill: parent
            anchors.margins: 0
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.extraSmall

            RowLayout {
                spacing: theme.spacing.small
                Layout.fillWidth: true

                Text {
                    text: alert.taskName
                    font.bold: true
                    color: theme.colors.textPrimary
                    Layout.fillWidth: true
                }

                Text {
                    text: alert.category.length > 0 ? "#" + alert.category : ""
                    font.pixelSize: 10
                    color: theme.colors.warning
                    visible: alert.category.length > 0
                }
            }

            RowLayout {
                spacing: theme.spacing.small
                Layout.fillWidth: true

                Text {
                    text: alert.message1
                    font.pixelSize: 11
                    color: theme.colors.textSecondary
                }

                Text {
                    text: alert.message2
                    font.pixelSize: 11
                    color: theme.colors.textSecondary
                }
            }
        }

        Button {
            text: "✕"
            flat: true
            color: theme.colors.textMuted
            onClicked: onDismiss()
            Layout.preferredWidth: 30
        }
    }
}