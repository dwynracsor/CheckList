import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var correction  // { name, items: [] }
    property int taskIndex
    property int correctionIndex
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    ColumnLayout {
        spacing: theme.spacing.small

        RowLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.small

            TextField {
                text: correction.name
                onEditingFinished: {
                    controller.setCorrectionName(taskIndex, correctionIndex, text);
                }
                Layout.fillWidth: true
            }

            Button {
                text: "×"
                flat: true
                color: theme.colors.error
                onClicked: controller.removeCorrectionFromTask(taskIndex, correctionIndex)
                Layout.preferredWidth: 30
            }
        }

        ColumnLayout {
            spacing: theme.spacing.extraSmall

            Repeater {
                model: correction.items.length
                delegate: ItemVm {
                    itemIndex: index
                    taskIndex: root.taskIndex
                    controller: root.controller
                    item: correction.items[index]
                }
            }

            RowLayout {
                spacing: theme.spacing.small
                Layout.fillWidth: true

                TextField {
                    id: newItemField
                    placeholderText: qsTr("Add correction item...")
                    Layout.fillWidth: true
                    onAccepted: addCorrectionItem()
                }

                Button {
                    text: qsTr("+")
                    flat: true
                    onClicked: addCorrectionItem()
                    Layout.preferredWidth: 30
                }
            }
        }
    }

    function addCorrectionItem() {
        var text = newItemField.text.trimmed();
        if (text.length === 0) return;
        controller.addItemToCorrection(taskIndex, correctionIndex, text);
        newItemField.text = "";
    }
}