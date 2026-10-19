import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: root
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    modal: true
    focus: true

    title: qsTr("Crear desde plantilla")

    ColumnLayout {
        spacing: theme.spacing.medium
        padding: theme.spacing.medium
        width: 400

        Text {
            text: qsTr("Selecciona una plantilla:")
            font.bold: true
            color: theme.colors.textPrimary
            Layout.fillWidth: true
        }

        ListView {
            id: templateList
            model: controller.templateCount
            delegate: ItemDelegate {
                width: parent.width
                text: controller.getTemplateName(index)
                subtext: controller.getTemplateItemCount(index) > 0
                    ? qsTr("%1 ítems").arg(controller.getTemplateItemCount(index))
                    : qsTr("Sin ítems")
                highlighted: ListView.isCurrentItem
                onClicked: {
                    currentIndex = index;
                }
                Layout.fillWidth: true
            }
            currentIndex: 0
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.small

            Text {
                text: qsTr("Ítems de la plantilla:")
                font.pixelSize: 11
                color: theme.colors.textSecondary
                visible: templateList.currentIndex >= 0 && controller.templateCount > 0
            }

            Repeater {
                model: Math.min(controller.getTemplateItemCount(templateList.currentIndex), 5)
                delegate: RowLayout {
                    spacing: theme.spacing.small

                    CheckBox {
                        checked: controller.getTemplateItemDone(templateList.currentIndex, index)
                        enabled: false
                        Layout.preferredWidth: 30
                    }

                    Text {
                        text: controller.getTemplateItemText(templateList.currentIndex, index)
                        font.pixelSize: 12
                        color: theme.colors.textPrimary
                        Layout.fillWidth: true
                    }
                }
                visible: templateList.currentIndex >= 0 && controller.templateCount > 0
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: theme.spacing.small

            Button {
                text: qsTr("Cancelar")
                flat: true
                onClicked: root.close()
                Layout.preferredWidth: 80
            }

            Button {
                text: qsTr("Crear tarea")
                onClicked: {
                    if (templateList.currentIndex >= 0 && controller.templateCount > 0) {
                        var templateId = controller.getTemplateId(templateList.currentIndex);
                        var taskNameInput = taskNameInputField.text;
                        if (taskNameInput.length > 0) {
                            controller.createTaskFromTemplate(templateId, taskNameInput);
                            taskNameInputField.text = "";
                            root.close();
                        }
                    }
                }
                Layout.preferredWidth: 100
            }

            TextInput {
                id: taskNameInputField
                placeholderText: qsTr("Nombre de la tarea...")
                Layout.fillWidth: true
                Layout.preferredHeight: 30
            }
        }
    }
}