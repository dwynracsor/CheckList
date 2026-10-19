import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    property var categoryData: []

    Component.onCompleted: {
        loadCategories();
    }

    function loadCategories() {
        var jsonString = controller.getCategoryTimeDistributionJson();
        try {
            categoryData = JSON.parse(jsonString);
        } catch (e) {
            console.log("Error parsing category stats: " + e);
            categoryData = [];
        }
    }

    readonly property int totalTime: (function() {
        var total = 0;
        for (var i = 0; i < categoryData.length; i++) {
            total += categoryData[i].totalTimeSeconds;
        }
        return total;
    })()

    ColumnLayout {
        anchors.fill: true
        spacing: theme.spacing.small

        Text {
            text: qsTr("Tiempo invertido por categoría")
            font.bold: true
            font.pixelSize: 14
            color: theme.colors.textPrimary
            Layout.fillWidth: true
        }

        ColumnLayout {
            spacing: theme.spacing.extraSmall
            Layout.fillWidth: true

            Repeater {
                model: categoryData
                delegate: RowLayout {
                    Layout.fillWidth: true
                    spacing: theme.spacing.small

                    Rectangle {
                        width: 4
                        height: 20
                        color: index < 5 ? ["#3F51B5", "#FF9800", "#4CAF50", "#F44336", "#9C27B0"][index] : theme.colors.border
                        radius: 4
                    }

                    Text {
                        text: modelData.category
                        font.pixelSize: 12
                        color: theme.colors.textPrimary
                        Layout.preferredWidth: 100
                    }

                    Rectangle {
                        color: theme.colors.border
                        radius: 4
                        Layout.fillWidth: true
                        Layout.preferredHeight: 16

                        Rectangle {
                            color: index < 5 ? ["#3F51B5", "#FF9800", "#4CAF50", "#F44336", "#9C27B0"][index] : theme.colors.accent
                            radius: 4
                            width: totalTime > 0
                                ? (modelData.totalTimeSeconds / totalTime) * parent.width
                                : 0
                            height: parent.height
                        }
                    }

                    Text {
                        text: root.formatTime(modelData.totalTimeSeconds)
                        font.pixelSize: 11
                        color: theme.colors.textSecondary
                        Layout.preferredWidth: 70
                    }
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            Layout.fillWidth: true
            spacing: theme.spacing.small

            Text {
                text: qsTr("Total:")
                font.bold: true
                color: theme.colors.textPrimary
            }

            Text {
                text: root.formatSeconds(root.totalTime)
                font.bold: true
                color: theme.colors.accent
            }
        }

        Text {
            text: categoryData.length === 0 ? qsTr("Sin datos de tiempo registrados") : ""
            color: theme.colors.textMuted
            font.italic: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            visible: categoryData.length === 0
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

    function formatTime(seconds) {
        if (seconds < 60) return seconds + "s";
        if (seconds < 3600) return Math.floor(seconds / 60) + "m";
        return Math.floor(seconds / 3600) + "h";
    }
}