import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var controller
    property int days: 7
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    property var dailyStats: []

    Component.onCompleted: {
        loadStats();
    }

    function loadStats() {
        var jsonString = controller.getDailyStatsJson(days);
        try {
            dailyStats = JSON.parse(jsonString);
        } catch (e) {
            console.log("Error parsing daily stats: " + e);
            dailyStats = [];
        }
    }

    readonly property int maxValue: (function() {
        var max = 0;
        for (var i = 0; i < dailyStats.length; i++) {
            if (dailyStats[i].tasksCompleted > max) {
                max = dailyStats[i].tasksCompleted;
            }
        }
        return max > 0 ? max : 1;
    })()

    ColumnLayout {
        anchors.fill: true
        spacing: theme.spacing.small

        Text {
            text: qsTr("Tareas completadas por día (últimos %1 días)").arg(days)
            font.bold: true
            font.pixelSize: 14
            color: theme.colors.textPrimary
            Layout.fillWidth: true
        }

        RowLayout {
            id: chartContainer
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: theme.spacing.small
            clip: true

            Repeater {
                model: dailyStats
                delegate: ColumnLayout {
                    Layout.alignment: Qt.AlignBottom
                    spacing: theme.spacing.extraSmall

                    Rectangle {
                        id: bar
                        color: modelData.tasksCompleted > 0 ? theme.colors.accent : theme.colors.border
                        radius: 4
                        Layout.preferredWidth: 30
                        Layout.preferredHeight: Math.max(
                            (modelData.tasksCompleted / root.maxValue) * 150,
                            1
                        )
                        ToolTip.visible: mouseArea.containsMouse
                        ToolTip.text: qsTr("%1: %2 tareas").arg(modelData.date).arg(modelData.tasksCompleted)

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                        }
                    }

                    Text {
                        text: modelData.tasksCompleted
                        font.pixelSize: 11
                        color: modelData.tasksCompleted > 0 ? theme.colors.textPrimary : theme.colors.textMuted
                        visible: modelData.tasksCompleted > 0
                    }

                    Text {
                        text: modelData.date.slice(5)
                        font.pixelSize: 9
                        color: theme.colors.textSecondary
                        rotation: -45
                        Layout.preferredWidth: 40
                    }
                }
            }
        }

        RowLayout {
            spacing: theme.spacing.medium
            Layout.fillWidth: true

            Rectangle {
                color: theme.colors.accent
                radius: 4
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
            }

            Text {
                text: qsTr("Tareas completadas")
                font.pixelSize: 11
                color: theme.colors.textSecondary
            }

            Text {
                text: qsTr("Máximo: %1").arg(root.maxValue)
                font.pixelSize: 10
                color: theme.colors.textMuted
                Layout.alignment: Qt.AlignRight
                Layout.fillWidth: true
            }
        }
    }
}