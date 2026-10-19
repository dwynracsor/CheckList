import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? Theme.dark : Theme.light

    ColumnLayout {
        anchors.fill: true
        spacing: theme.spacing.medium
        padding: theme.spacing.medium

        // Header
        Text {
            text: qsTr("Estadísticas de Productividad")
            font.pixelSize: 20
            font.bold: true
            color: theme.colors.textPrimary
            Layout.fillWidth: true
        }

        // Selector de rango de días
        RowLayout {
            spacing: theme.spacing.small
            Layout.fillWidth: true

            Text {
                text: qsTr("Ver últimos:")
                font.pixelSize: 12
                color: theme.colors.textSecondary
            }

            RowLayout {
                spacing: theme.spacing.extraSmall

                Repeater {
                    model: [7, 14, 30]
                    delegate: Button {
                        text: modelData + " días"
                        flat: true
                        checked: root.selectedDays === modelData
                        onClicked: root.selectedDays = modelData
                        Layout.preferredWidth: 70
                    }
                }
            }

            Text {
                text: ""
                Layout.fillWidth: true
            }
        }

        property int selectedDays: 7

        // Tarjetas resumen (desde JSON del reporte semanal)
        RowLayout {
            spacing: theme.spacing.small
            Layout.fillWidth: true

            Repeater {
                model: 5
                delegate: StatCard {
                    title: index === 0 ? qsTr("Tareas completadas") :
                           index === 1 ? qsTr("Ítems checkeados") :
                           index === 2 ? qsTr("Pomodoros") :
                           index === 3 ? qsTr("Tiempo total") :
                           qsTr("Racha actual")
                    value: index === 0 ? JSON.parse(controller.getWeeklyReportJson(root.selectedDays)).totalTasksCompleted :
                           index === 1 ? JSON.parse(controller.getWeeklyReportJson(root.selectedDays)).totalItemsChecked :
                           index === 2 ? JSON.parse(controller.getWeeklyReportJson(root.selectedDays)).totalPomodoros :
                           index === 3 ? root.formatSeconds(JSON.parse(controller.getWeeklyReportJson(root.selectedDays)).totalTimeSeconds) :
                           JSON.parse(controller.getProductivityStreakJson()).currentStreak
                    subtitle: index === 0 ? qsTr("En el período") :
                           index === 1 ? qsTr("Total de ítems") :
                           index === 2 ? qsTr("Sesiones completadas") :
                           index === 3 ? qsTr("Horas invertidas") :
                           qsTr("Días consecutivos")
                    icon: index === 0 ? "✅" :
                          index === 1 ? "☑️" :
                          index === 2 ? "🍅" :
                          index === 3 ? "⏱" :
                          "🔥"
                    darkMode: root.darkMode
                    Layout.fillWidth: true
                }
            }
        }

        // Gráfico de barras semanal
        WeeklyChart {
            controller: root.controller
            days: root.selectedDays
            darkMode: root.darkMode
            Layout.fillWidth: true
            Layout.preferredHeight: 200
        }

        // Racha de productividad y distribución por categoría
        RowLayout {
            spacing: theme.spacing.medium
            Layout.fillWidth: true

            StreakIndicator {
                controller: root.controller
                darkMode: root.darkMode
                Layout.fillWidth: true
            }

            CategoryDistribution {
                controller: root.controller
                darkMode: root.darkMode
                Layout.fillWidth: true
            }
        }

        // Tareas pendientes antiguas
        ColumnLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.small

            Text {
                text: qsTr("Tareas pendientes antiguas")
                font.pixelSize: 16
                font.bold: true
                color: theme.colors.textPrimary
                Layout.fillWidth: true
            }

            Repeater {
                model: root.oldPendingTasks
                delegate: RowLayout {
                    spacing: theme.spacing.small
                    Layout.fillWidth: true

                    Text {
                        text: modelData.name
                        font.pixelSize: 13
                        color: theme.colors.textPrimary
                        Layout.fillWidth: true
                    }

                    Text {
                        text: modelData.days + " días"
                        font.pixelSize: 12
                        color: theme.colors.warning
                    }
                }
            }

            Text {
                text: oldPendingTasks.length === 0 ? qsTr("Sin tareas pendientes antiguas") : ""
                font.pixelSize: 11
                color: theme.colors.textMuted
                Layout.fillWidth: true
                visible: oldPendingTasks.length === 0
            }
        }
    }

    // Computed models para tareas pendientes antiguas
    readonly property var oldPendingTasks: (function() {
        var today = new Date();
        today.setHours(0, 0, 0, 0);
        var thresholdDays = 7;
        var result = [];
        for (var i = 0; i < controller.taskCount; i++) {
            var task = controller.getTask(i);
            if (!task || task.isFinished) continue;
            var taskDate = QDate.fromString(task.date, "yyyy-MM-dd");
            var days = Math.floor((today - taskDate) / (1000 * 60 * 60 * 24));
            if (days >= thresholdDays) {
                result.push({ name: task.name, days: days });
            }
        }
        result.sort(function(a, b) { return b.days - a.days; });
        return result;
    })()

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