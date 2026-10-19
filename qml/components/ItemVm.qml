import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property int itemIndex
    property int taskIndex
    property var controller
    property var item  // { text, done, id }
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    RowLayout {
        spacing: theme.spacing.small
        padding: theme.spacing.small

        CheckBox {
            text: ""
            checked: item.done
            onCheckedChanged: {
                controller.toggleItemInTask(taskIndex, itemIndex, checked);
            }
            Layout.preferredWidth: 30
        }

        TextField {
            text: item.text
            readOnly: !root.editMode
            onEditingFinished: {
                if (!readOnly) {
                    controller.setItemTextInTask(taskIndex, itemIndex, text);
                    root.editMode = false;
                }
            }
            Layout.fillWidth: true
        }

        Button {
            text: root.editMode ? "✓" : "✎"
            flat: true
            onClicked: root.editMode = !root.editMode
            Layout.preferredWidth: 30
        }

        Button {
            text: "×"
            flat: true
            color: theme.colors.error
            onClicked: controller.removeItemFromTask(taskIndex, itemIndex)
            Layout.preferredWidth: 30
        }
    }

    property bool editMode: false
}