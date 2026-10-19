import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: root
    property string title: qsTr("Confirm")
    property string message: qsTr("Are you sure?")
    signal onAccept
    signal onReject
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    modal: true
    focus: true

    ColumnLayout {
        spacing: theme.spacing.medium
        padding: theme.spacing.medium
        width: 300

        Text {
            text: root.title
            font.bold: true
            font.pixelSize: 16
            color: theme.colors.textPrimary
            Layout.fillWidth: true
        }

        Text {
            text: root.message
            color: theme.colors.textSecondary
            font.pixelSize: 14
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: theme.spacing.small

            Button {
                text: qsTr("Cancel")
                flat: true
                onClicked: {
                    root.onReject()
                    root.close()
                }
                Layout.preferredWidth: 80
            }

            Button {
                text: qsTr("OK")
                onClicked: {
                    root.onAccept()
                    root.close()
                }
                Layout.preferredWidth: 80
            }
        }
    }
}