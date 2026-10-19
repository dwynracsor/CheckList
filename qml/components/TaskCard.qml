import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var task
    property int taskIndex
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    signal taskSelected

    RowLayout {
        id: card
        Layout.fillWidth: true
        spacing: theme.spacing.small
        padding: theme.spacing.medium

        background: Rectangle {
            color: theme.colors.cardBackground
            radius: 8
            border.color: theme.colors.border
            border.width: 1
        }

        CheckBox {
            text: ""
            checked: task.isFinished
            onCheckedChanged: {
                for (var i = 0; i < task.itemCount; i++) {
                    controller.toggleItemInTask(taskIndex, i, checked);
                }
            }
            Layout.preferredWidth: 30
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.extraSmall

            RowLayout {
                spacing: theme.spacing.small
                Layout.fillWidth: true

                Text {
                    text: task.name
                    font.bold: true
                    font.pixelSize: 14
                    color: task.isFinished ? theme.colors.textMuted : theme.colors.textPrimary
                    Layout.fillWidth: true
                }

                Text {
                    text: task.category.length > 0 ? "#" + task.category : ""
                    font.pixelSize: 11
                    color: theme.colors.accent
                    visible: task.category.length > 0
                }

                Text {
                    text: task.date
                    font.pixelSize: 11
                    color: theme.colors.textSecondary
                }
            }

            RowLayout {
                spacing: theme.spacing.small
                Layout.fillWidth: true

                ProgressBar {
                    value: task.itemCount > 0 ? (task.itemCount - 0) / task.itemCount * 100 : 0
                    Layout.fillWidth: true
                }

                Text {
                    text: task.itemCount > 0 ? "0/" + task.itemCount : "0/0"
                    font.pixelSize: 11
                    color: theme.colors.textSecondary
                    Layout.preferredWidth: 40
                }
            }

            TimerControl {
                task: root.task
                taskIndex: root.taskIndex
                controller: root.controller
                darkMode: root.darkMode
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            spacing: theme.spacing.small
            alignment: Qt.AlignTop

            Button {
                text: qsTr("Edit")
                flat: true
                Layout.preferredWidth: 50
                onClicked: {}
            }

            Button {
                text: qsTr("Delete")
                flat: true
                color: theme.colors.error
                Layout.preferredWidth: 50
                onClicked: {}
            }
        }
    }

    ColumnLayout {
        visible: true
        Layout.fillWidth: true
        spacing: theme.spacing.extraSmall

        Repeater {
            model: task.itemCount
            delegate: ItemVm {
                itemIndex: index
                taskIndex: root.taskIndex
                controller: root.controller
            }
        }
    }

    TaskAttachments {
        task: root.task
        controller: root.controller
        darkMode: root.darkMode
        Layout.fillWidth: true
    }
}