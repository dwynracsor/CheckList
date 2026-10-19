import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var task      // TaskQml
    property int taskIndex // Índice en el controlador
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    readonly property string actualTime: root.formatSeconds(task.actualDurationSeconds)
    readonly property string estimatedTime: task.estimatedDurationSeconds > 0
        ? root.formatSeconds(task.estimatedDurationSeconds)
        : qsTr("Sin estimación")
    readonly property string remainingTime: task.remainingTimeSeconds >= 0
        ? root.formatSeconds(task.remainingTimeSeconds)
        : qsTr("N/A")

    RowLayout {
        Layout.fillWidth: true
        spacing: theme.spacing.small
        padding: theme.spacing.small

        RowLayout {
            spacing: theme.spacing.extraSmall

            Button {
                text: task.isTiming ? "⏸" : "▶"
                flat: true
                enabled: task.estimatedDurationSeconds > 0 || task.isTiming
                onClicked: {
                    if (task.isTiming) {
                        controller.stopTaskTiming(taskIndex);
                    } else {
                        controller.startTaskTiming(taskIndex);
                    }
                }
                Layout.preferredWidth: 35
                Layout.preferredHeight: 35
            }

            Button {
                text: qsTr("⏹")
                flat: true
                enabled: task.isTiming
                onClicked: controller.stopTaskTiming(taskIndex)
                Layout.preferredWidth: 35
                Layout.preferredHeight: 35
            }
        }

        ColumnLayout {
            spacing: theme.spacing.extraSmall

            RowLayout {
                spacing: theme.spacing.extraSmall

                Text {
                    text: actualTime
                    font.pixelSize: 13
                    font.bold: true
                    color: task.isTiming ? theme.colors.success : theme.colors.textPrimary
                }

                Text {
                    text: "/" + estimatedTime
                    font.pixelSize: 11
                    color: theme.colors.textSecondary
                }
            }

            ProgressBar {
                value: task.progressPercentage
                Layout.fillWidth: true
            }

            RowLayout {
                spacing: theme.spacing.extraSmall

                Text {
                    text: qsTr("Pomodoros: %1").arg(task.pomodoroCount)
                    font.pixelSize: 11
                    color: theme.colors.textSecondary
                }

                Text {
                    text: task.remainingTimeSeconds >= 0
                        ? qsTr("(%1 restante)").arg(root.remainingTime)
                        : ""
                    font.pixelSize: 10
                    color: theme.colors.textMuted
                    visible: task.remainingTimeSeconds >= 0
                }
            }
        }

        Button {
            text: qsTr("🍅")
            flat: true
            onClicked: controller.incrementTaskPomodoro(taskIndex)
            Layout.preferredWidth: 35
            Layout.preferredHeight: 35
            tooltip: qsTr("Agregar pomodoro")
        }
    }

    function formatSeconds(seconds) {
        if (seconds < 0) return "0:00";
        var h = Math.floor(seconds / 3600);
        var m = Math.floor((seconds % 3600) / 60);
        var s = seconds % 60;
        if (h > 0) {
            return h + ":" + String(m).padStart(2, '0') + ":" + String(s).padStart(2, '0');
        }
        return m + ":" + String(s).padStart(2, '0');
    }
}