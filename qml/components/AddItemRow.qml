import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var item  // { text, done, id }
    signal onRemove
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    RowLayout {
        spacing: theme.spacing.small
        padding: theme.spacing.small

        CheckBox {
            text: ""
            checked: item.done
            Layout.preferredWidth: 30
        }

        Text {
            text: item.text
            Layout.fillWidth: true
            color: item.done ? theme.colors.textMuted : theme.colors.textPrimary
            font.strikeThrough: item.done
        }

        Button {
            text: "×"
            flat: true
            color: theme.colors.error
            onClicked: onRemove()
            Layout.preferredWidth: 30
        }
    }
}