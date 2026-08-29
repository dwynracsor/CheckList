import QtQuick
import QtQuick.Layouts
import ChecklistApp

RowLayout {
    id: root

    property string statusText: ""
    property bool autoHide: true
    property int autoHideDelay: 3000

    signal hidden()

    spacing: Theme.spacingSmall

    // Auto-hide timer
    Timer {
        id: hideTimer
        interval: root.autoHideDelay
        onTriggered: {
            root.opacity = 0
            root.hidden()
        }
    }

    // Status icon
    Rectangle {
        width: 8
        height: 8
        radius: 4
        color: {
            if (root.statusText.indexOf("Error") >= 0) return Theme.danger
            if (root.statusText.indexOf("Guardado") >= 0) return Theme.success
            return Theme.textDisabled
        }
        opacity: root.statusText !== "" ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    // Status text
    Text {
        text: root.statusText
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.textDisabled
        opacity: root.statusText !== "" ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    Item { Layout.fillWidth: true }

    // Update status method
    function updateStatus(message) {
        root.statusText = message
        root.opacity = 1
        hideTimer.restart()
    }

    Behavior on opacity {
        NumberAnimation { duration: 300 }
    }
}
