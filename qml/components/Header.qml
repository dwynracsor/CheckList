import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

RowLayout {
    id: root

    signal toggleDarkMode()
    signal notificationClicked()

    spacing: Theme.spacingSmall

    // App title
    Text {
        text: "CheckList"
        font.pixelSize: Theme.fontSizeTitle
        font.weight: Theme.fontWeightBold
        color: Theme.textPrimary
    }

    // Dark mode toggle switch
    Rectangle {
        id: toggleTrack
        Layout.preferredWidth: 44
        Layout.preferredHeight: 24
        radius: 12
        color: Theme.darkMode ? Theme.primary : Theme.borderDefault
        border.color: Theme.darkMode ? Theme.primary : Theme.borderDefault
        border.width: 1

        Behavior on color { ColorAnimation { duration: 200 } }

        Rectangle {
            id: toggleThumb
            width: 20
            height: 20
            radius: 10
            color: "#ffffff"
            x: Theme.darkMode ? toggleTrack.width - width - 2 : 2
            y: 2

            Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutQuad } }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: root.toggleDarkMode()
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.toggleDarkMode()
        }
    }

    Item { Layout.fillWidth: true }

    // Notification bell icon
    Item {
        Layout.preferredWidth: 36
        Layout.preferredHeight: 36

        Text {
            id: bellIcon
            anchors.centerIn: parent
            text: "\uD83D\uDD14"
            font.pixelSize: 20
            color: Theme.textPrimary
        }

        // Notification badge
        Rectangle {
            visible: taskController.taskCount > 0
            width: 16
            height: 16
            radius: 8
            color: Theme.danger
            anchors.top: bellIcon.top
            anchors.topMargin: -4
            anchors.right: bellIcon.right
            anchors.rightMargin: -6

            Text {
                anchors.centerIn: parent
                text: taskController.taskCount > 9 ? "9+" : taskController.taskCount.toString()
                font.pixelSize: 9
                font.weight: Theme.fontWeightBold
                color: "#ffffff"
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.notificationClicked()
        }
    }
}
