import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    property string newTemplateName: ""
    property string newTemplateItemText: ""

    ColumnLayout {
        anchors.fill: true
        spacing: theme.spacing.medium
        padding: theme.spacing.medium

        Text {
            text: qsTr("Plantillas de tareas")
            font.pixelSize: 18
            font.bold: true
            color: theme.colors.textPrimary
            Layout.fillWidth: true
        }

        GroupBox {
            title: qsTr("Crear plantilla nueva")
            Layout.fillWidth: true

            ColumnLayout {
                spacing: theme.spacing.small
                padding: theme.spacing.small

                RowLayout {
                    spacing: theme.spacing.small
                    Layout.fillWidth: true

                    TextField {
                        text: root.newTemplateName
                        placeholderText: qsTr("Nombre de la plantilla")
                        onTextChanged: root.newTemplateName = text
                        Layout.fillWidth: true
                    }

                    Button {
                        text: qsTr("+")
                        flat: true
                        enabled: root.newTemplateName.trimmed().length > 0
                        onClicked: {
                            controller.addTemplate(root.newTemplateName.trimmed(), []);
                            root.newTemplateName = "";
                        }
                        Layout.preferredWidth: 30
                    }
                }

                ColumnLayout {
                    spacing: theme.spacing.extraSmall

                    RowLayout {
                        spacing: theme.spacing.small
                        Layout.fillWidth: true

                        TextField {
                            text: root.newTemplateItemText
                            placeholderText: qsTr("Agregar ítem a la plantilla...")
                            onTextChanged: root.newTemplateItemText = text
                            onAccepted: addTemplateItem()
                            Layout.fillWidth: true
                        }

                        Button {
                            text: qsTr("+")
                            flat: true
                            enabled: root.newTemplateItemText.trimmed().length > 0
                            onClicked: addTemplateItem()
                            Layout.preferredWidth: 30
                        }
                    }

                    Repeater {
                        model: newTemplateItems
                        delegate: RowLayout {
                            spacing: theme.spacing.small

                            CheckBox {
                                checked: false
                                enabled: false
                                Layout.preferredWidth: 30
                            }

                            Text {
                                text: modelData
                                font.pixelSize: 12
                                color: theme.colors.textPrimary
                                Layout.fillWidth: true
                            }

                            Button {
                                text: "×"
                                flat: true
                                color: theme.colors.error
                                onClicked: {
                                    newTemplateItems.splice(index, 1);
                                }
                                Layout.preferredWidth: 30
                            }
                        }
                    }
                }

                Button {
                    text: qsTr("Guardar plantilla")
                    enabled: root.newTemplateName.trimmed().length > 0
                    onClicked: {
                        controller.addTemplate(root.newTemplateName.trimmed(), newTemplateItems);
                        root.newTemplateName = "";
                        newTemplateItems = [];
                    }
                    Layout.fillWidth: true
                }
            }
        }

        GroupBox {
            title: qsTr("Plantillas existentes")
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                spacing: theme.spacing.small

                Repeater {
                    model: controller.templateCount
                    delegate: ItemDelegate {
                        width: parent.width
                        text: controller.getTemplateName(index)
                        subtext: qsTr("%1 ítems - %2").arg(
                            controller.getTemplateItemCount(index)).arg(
                            controller.getTemplateRecurrence(index) === 0 ? qsTr("Sin repetición") :
                            controller.getTemplateRecurrence(index) === 1 ? qsTr("Diaria") :
                            controller.getTemplateRecurrence(index) === 2 ? qsTr("Semanal") : qsTr("Mensual"))
                        rightItem: RowLayout {
                            spacing: theme.spacing.extraSmall

                            Button {
                                text: qsTr("Usar")
                                flat: true
                                onClicked: {
                                    var templateId = controller.getTemplateId(index);
                                    controller.createTaskFromTemplate(templateId, controller.getTemplateName(index));
                                }
                                Layout.preferredWidth: 50
                            }

                            Button {
                                text: "×"
                                flat: true
                                color: theme.colors.error
                                onClicked: controller.removeTemplate(controller.getTemplateId(index))
                                Layout.preferredWidth: 30
                            }
                        }
                        Layout.fillWidth: true
                    }
                }

                Text {
                    text: controller.templateCount === 0 ? qsTr("No hay plantillas creadas") : ""
                    color: theme.colors.textMuted
                    font.italic: true
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignCenter
                    visible: controller.templateCount === 0
                }
            }
        }
    }

    property var newTemplateItems: []

    function addTemplateItem() {
        var text = root.newTemplateItemText.trimmed();
        if (text.length > 0) {
            newTemplateItems.push(text);
            root.newTemplateItemText = "";
        }
    }
}