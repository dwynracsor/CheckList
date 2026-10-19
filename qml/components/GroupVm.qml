import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var group
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    ColumnLayout {
        Layout.fillWidth: true
        spacing: theme.spacing.small

        RowLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.small

            Text {
                text: group.label
                font.bold: true
                font.pixelSize: 16
                color: theme.colors.textPrimary
                Layout.fillWidth: true
            }

            Text {
                text: "(" + group.tasks.length + ")"
                color: theme.colors.textSecondary
                font.pixelSize: 12
            }
        }

        ColumnLayout {
            spacing: theme.spacing.small

            Repeater {
                model: group.tasks
                delegate: TaskCard {
                    task: modelData
                    taskIndex: controller.tasks().indexOf(modelData)
                    controller: root.controller
                    darkMode: root.darkMode
                }
            }
        }
    }
}