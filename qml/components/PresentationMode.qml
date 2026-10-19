import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    readonly property var currentTask: controller.presentationMode && controller.presentationTaskIndex >= 0 && controller.presentationTaskIndex < controller.taskCount
        ? controller.getTask(controller.presentationTaskIndex)
        : null

    ColumnLayout {
        anchors.fill: true
        spacing: theme.spacing.large
        padding: theme.spacing.large

        RowLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.small

            Button {
                text: "◀ Anterior"
                flat: true
                enabled: controller.presentationMode && controller.presentationTaskIndex > 0
                onClicked: controller.previousPresentationTask()
                Layout.preferredWidth: 120
                Layout.preferredHeight: 50
                font.pixelSize: 18
            }

            Text {
                text: controller.presentationMode ? qsTr("Modo Presentación") : qsTr("Modo Normal")
                font.pixelSize: 24
                font.bold: true
                color: controller.presentationMode ? theme.colors.success : theme.colors.textPrimary
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
            }

            Button {
                text: "Siguiente ▶"
                flat: true
                enabled: controller.presentationMode && controller.presentationTaskIndex < controller.taskCount - 1
                onClicked: controller.nextPresentationTask()
                Layout.preferredWidth: 120
                Layout.preferredHeight: 50
                font.pixelSize: 18
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.small

            Text {
                text: qsTr("%1 de %2").arg(controller.presentationTaskIndex + 1).arg(controller.taskCount)
                font.pixelSize: 16
                color: theme.colors.textSecondary
            }

            ProgressBar {
                value: controller.taskCount > 0 ? ((controller.presentationTaskIndex + 1) * 100 / controller.taskCount) : 0
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: theme.spacing.large
            visible: currentTask !== null

            Text {
                text: currentTask.name
                font.pixelSize: 48
                font.bold: true
                color: theme.colors.textPrimary
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
            }

            RowLayout {
                Layout.alignment: Qt.AlignCenter
                spacing: theme.spacing.medium

                Text {
                    text: currentTask.category.length > 0 ? "#" + currentTask.category : qsTr("Sin categoría")
                    font.pixelSize: 18
                    color: theme.colors.accent
                    visible: currentTask.category.length > 0
                }

                Text {
                    text: currentTask.date
                    font.pixelSize: 18
                    color: theme.colors.textSecondary
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: theme.spacing.small

                Repeater {
                    model: currentTask.itemCount
                    delegate: RowLayout {
                        spacing: theme.spacing.medium
                        Layout.fillWidth: true

                        CheckBox {
                            text: ""
                            checked: modelData.done
                            font.pixelSize: 20
                            Layout.preferredWidth: 50
                        }

                        Text {
                            text: modelData.text
                            font.pixelSize: 24
                            color: modelData.done ? theme.colors.textMuted : theme.colors.textPrimary
                            font.strikeThrough: modelData.done
                            Layout.fillWidth: true
                            wrapMode: Text.Wrap
                        }
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignCenter
                spacing: theme.spacing.large

                ColumnLayout {
                    alignment: Qt.AlignCenter
                    spacing: theme.spacing.extraSmall

                    Text {
                        text: qsTr("Tiempo estimado")
                        font.pixelSize: 14
                        color: theme.colors.textSecondary
                    }

                    Text {
                        text: currentTask.estimatedDurationSeconds > 0
                            ? root.formatSeconds(currentTask.estimatedDurationSeconds)
                            : qsTr("Sin estimación")
                        font.pixelSize: 32
                        font.bold: true
                        color: theme.colors.accent
                    }
                }

                ColumnLayout {
                    alignment: Qt.AlignCenter
                    spacing: theme.spacing.extraSmall

                    Text {
                        text: qsTr("Tiempo invertido")
                        font.pixelSize: 14
                        color: theme.colors.textSecondary
                    }

                    Text {
                        text: root.formatSeconds(currentTask.actualDurationSeconds)
                        font.pixelSize: 32
                        font.bold: true
                        color: theme.colors.success
                    }
                }

                ColumnLayout {
                    alignment: Qt.AlignCenter
                    spacing: theme.spacing.extraSmall

                    Text {
                        text: qsTr("Pomodoros")
                        font.pixelSize: 14
                        color: theme.colors.textSecondary
                    }

                    Text {
                        text: currentTask.pomodoroCount
                        font.pixelSize: 32
                        font.bold: true
                        color: theme.colors.warning
                    }
                }
            }
        }

        ColumnLayout {
            visible: !controller.presentationMode || currentTask === null
            Layout.alignment: Qt.AlignCenter
            spacing: theme.spacing.medium

            Text {
                text: qsTr("Modo Presentación desactivado")
                font.pixelSize: 24
                color: theme.colors.textSecondary
                font.italic: true
            }

            Text {
                text: qsTr("Activa el modo presentación desde el menú principal")
                font.pixelSize: 16
                color: theme.colors.textMuted
            }
        }
    }

    function formatSeconds(seconds) {
        if (seconds < 0) return "0:00";
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds % 3600) / 60);
        var s = seconds % 60;
        if (h > 0) {
            return h + "h " + String(m).padStart(2, '0') + "m";
        }
        return m + "m " + String(s).padStart(2, '0') + "s";
    }
}