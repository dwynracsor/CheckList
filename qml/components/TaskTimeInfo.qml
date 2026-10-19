import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var task   // TaskQml
    property int taskIndex
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    readonly property string estimatedTime: task.estimatedDurationSeconds > 0
        ? root.formatSeconds(task.estimatedDurationSeconds)
        : qsTr("Sin estimación")
    readonly property string actualTime: root.formatSeconds(task.actualDurationSeconds)
    readonly property string remainingTime: task.remainingTimeSeconds >= 0
        ? root.formatSeconds(task.remainingTimeSeconds)
        : qsTr("N/A")
    readonly property string progressText: task.progressPercentage + "%"

    GroupBox {
        title: qsTr("Tiempo y Pomodoro")
        Layout.fillWidth: true

        ColumnLayout {
            spacing: theme.spacing.small
            padding: theme.spacing.small

            RowLayout {
                spacing: theme.spacing.small
                Layout.fillWidth: true

                Text {
                    text: qsTr("Estimado:")
                    font.pixelSize: 12
                    color: theme.colors.textSecondary
                    Layout.preferredWidth: 80
                }

                SpinBox {
                    from: 0
                    to: 99999
                    value: task.estimatedDurationSeconds / 60
                    onValueChanged: {
                        var idx = taskIndex;
                        if (idx >= 0) controller.setTaskEstimatedDuration(idx, value * 60);
                    }
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30
                }

                Text {
                    text: qsTr("min")
                    font.pixelSize: 11
                    color: theme.colors.textMuted
                }
            }

            RowLayout {
                spacing: theme.spacing.medium
                Layout.fillWidth: true

                ColumnLayout {
                    spacing: theme.spacing.extraSmall
                    Layout.fillWidth: true

                    Text {
                        text: qsTr("Invertido:")
                        font.pixelSize: 11
                        color: theme.colors.textSecondary
                    }

                    Text {
                        text: actualTime
                        font.pixelSize: 16
                        font.bold: true
                        color: theme.colors.textPrimary
                    }
                }

                ColumnLayout {
                    spacing: theme.spacing.extraSmall
                    Layout.fillWidth: true

                    Text {
                        text: qsTr("Restante:")
                        font.pixelSize: 11
                        color: theme.colors.textSecondary
                    }

                    Text {
                        text: remainingTime
                        font.pixelSize: 16
                        font.bold: true
                        color: task.remainingTimeSeconds >= 0
                            ? (task.remainingTimeSeconds === 0 ? theme.colors.success : theme.colors.textPrimary)
                            : theme.colors.textMuted
                    }
                }

                ColumnLayout {
                    spacing: theme.spacing.extraSmall
                    Layout.fillWidth: true

                    Text {
                        text: qsTr("Progreso:")
                        font.pixelSize: 11
                        color: theme.colors.textSecondary
                    }

                    Text {
                        text: progressText
                        font.pixelSize: 16
                        font.bold: true
                        color: theme.colors.accent
                    }
                }
            }

            TimerControl {
                task: root.task
                taskIndex: root.taskIndex
                controller: root.controller
                darkMode: root.darkMode
                Layout.fillWidth: true
            }

            PomodoroIndicator {
                count: task.pomodoroCount
                darkMode: root.darkMode
                Layout.fillWidth: true
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