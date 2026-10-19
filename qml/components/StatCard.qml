import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property string title: ""
    property string value: ""
    property string subtitle: ""
    property string icon: ""
    property bool darkMode: false

    readonly property var theme: darkMode ? Theme.dark : Theme.light

    Rectangle {
        anchors.fill: parent
        color: theme.colors.cardBackground
        radius: 8
        border.color: theme.colors.border
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: theme.spacing.small
            spacing: theme.spacing.extraSmall

            RowLayout {
                spacing: theme.spacing.small

                Text {
                    text: root.icon
                    font.pixelSize: 20
                    Layout.preferredWidth: 30
                }

                Text {
                    text: root.title
                    font.pixelSize: 12
                    color: theme.colors.textSecondary
                    Layout.fillWidth: true
                }
            }

            Text {
                text: root.value
                font.pixelSize: 24
                font.bold: true
                color: theme.colors.textPrimary
            }

            Text {
                text: root.subtitle
                font.pixelSize: 10
                color: theme.colors.textMuted
            }
        }
    }
}