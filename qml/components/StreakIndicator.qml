import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    property var streakData: {}

    Component.onCompleted: {
        loadStreak();
    }

    function loadStreak() {
        var jsonString = controller.getProductivityStreakJson();
        try {
            streakData = JSON.parse(jsonString);
        } catch (e) {
            console.log("Error parsing streak: " + e);
            streakData = { currentStreak: 0, maxStreak: 0, daysActive: 0 };
        }
    }

    ColumnLayout {
        anchors.fill: true
        spacing: theme.spacing.small

        Text {
            text: qsTr("Racha de productividad")
            font.bold: true
            font.pixelSize: 14
            color: theme.colors.textPrimary
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: theme.spacing.small
            Layout.fillWidth: true

            StatCard {
                title: qsTr("Racha actual")
                value: streakData.currentStreak
                subtitle: qsTr("Días consecutivos")
                icon: "🔥"
                color: streakData.currentStreak > 0 ? theme.colors.success : theme.colors.textMuted
                darkMode: root.darkMode
                Layout.fillWidth: true
            }

            StatCard {
                title: qsTr("Racha máxima")
                value: streakData.maxStreak
                subtitle: qsTr("Mejor racha histórica")
                icon: "🏆"
                color: theme.colors.accent
                darkMode: root.darkMode
                Layout.fillWidth: true
            }

            StatCard {
                title: qsTr("Días activos")
                value: streakData.daysActive
                subtitle: qsTr("Con progreso")
                icon: "📅"
                color: theme.colors.warning
                darkMode: root.darkMode
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.extraSmall

            RowLayout {
                spacing: theme.spacing.extraSmall

                Text {
                    text: qsTr("Progreso hoy:")
                    font.pixelSize: 11
                    color: theme.colors.textSecondary
                }

                ProgressBar {
                    value: streakData.currentStreak > 0 ? 100 : 0
                    Layout.fillWidth: true
                }

                Text {
                    text: streakData.currentStreak > 0 ? qsTr("Activo") : qsTr("Inactivo")
                    font.pixelSize: 10
                    color: streakData.currentStreak > 0 ? theme.colors.success : theme.colors.textMuted
                    Layout.preferredWidth: 60
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignCenter
                spacing: theme.spacing.extraSmall

                Repeater {
                    model: Math.min(streakData.currentStreak, 10)
                    delegate: Text {
                        text: "🔥"
                        font.pixelSize: 20
                    }
                }

                Text {
                    text: streakData.currentStreak > 10 ? qsTr("+%1 más").arg(streakData.currentStreak - 10) : ""
                    font.pixelSize: 12
                    color: theme.colors.success
                    visible: streakData.currentStreak > 10
                }
            }
        }
    }
}