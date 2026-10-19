import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property int count: 0
    property int target: 4
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    readonly property int completedCount: Math.min(count, target)
    readonly property int remainingCount: Math.max(target - count, 0)
    readonly property bool isComplete: count >= target

    ColumnLayout {
        spacing: theme.spacing.extraSmall
        alignment: Qt.AlignCenter

        RowLayout {
            spacing: theme.spacing.extraSmall

            Repeater {
                model: target
                delegate: Rectangle {
                    width: 20
                    height: 20
                    radius: 10
                    color: index < completedCount ? theme.colors.success : theme.colors.border
                    border.color: theme.colors.textMuted
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: index < completedCount ? "✓" : ""
                        font.pixelSize: 14
                        color: index < completedCount ? "#FFFFFF" : theme.colors.textMuted
                    }
                }
            }
        }

        Text {
            text: qsTr("%1/%2 pomodoros").arg(completedCount).arg(target)
            font.pixelSize: 11
            color: isComplete ? theme.colors.success : theme.colors.textSecondary
            font.bold: isComplete
        }
    }
}