import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.2

Item {
    id: root
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    FileDialog {
        id: exportDialog
        title: qsTr("Exportar a iCal (.ics)")
        nameFilters: ["iCal Calendar (*.ics)"]
        onAccepted: {
            var icsData = controller.exportTasksToICal();
            console.log("Exportando a: " + exportDialog.fileUrl);
            console.log(icsData);
            exportStatus.text = qsTr("Exportado correctamente");
        }
    }

    FileDialog {
        id: importDialog
        title: qsTr("Importar desde iCal (.ics)")
        nameFilters: ["iCal Calendar (*.ics)"]
        onAccepted: {
            console.log("Importando desde: " + importDialog.fileUrl);
            importStatus.text = qsTr("Selecciona un archivo .ics para importar");
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: theme.spacing.small

        Text {
            text: qsTr("Integración con Calendario (iCal)")
            font.bold: true
            font.pixelSize: 16
            color: theme.colors.textPrimary
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: theme.spacing.small
            Layout.fillWidth: true

            Button {
                text: qsTr("Exportar Todo a iCal")
                onClicked: exportDialog.open()
                Layout.fillWidth: true
                Layout.preferredHeight: 40
            }

            Button {
                text: qsTr("Importar desde iCal")
                flat: true
                onClicked: importDialog.open()
                Layout.fillWidth: true
                Layout.preferredHeight: 40
            }
        }

        GroupBox {
            title: qsTr("Vista Previa de Exportación")
            Layout.fillWidth: true

            ColumnLayout {
                spacing: theme.spacing.small
                padding: theme.spacing.small

                Text {
                    text: qsTr("Se exportarán %1 tareas").arg(controller.taskCount)
                    font.pixelSize: 12
                    color: theme.colors.textSecondary
                }

                Text {
                    text: qsTr("Formato: iCalendar (.ics) - compatible con Google Calendar, Outlook, Apple Calendar")
                    font.pixelSize: 10
                    color: theme.colors.textMuted
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                TextArea {
                    id: icsPreview
                    readOnly: true
                    placeholderText: qsTr("Haz clic en 'Exportar Todo a iCal' para ver la vista previa")
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    font.family: "Courier New"
                    font.pixelSize: 10
                    color: theme.colors.textPrimary
                }

                RowLayout {
                    Layout.alignment: Qt.AlignRight
                    spacing: theme.spacing.small

                    Button {
                        text: qsTr("Generar Vista Previa")
                        flat: true
                        onClicked: {
                            var ics = controller.exportTasksToICal();
                            icsPreview.text = ics;
                        }
                        Layout.preferredWidth: 100
                    }
                }
            }
        }

        GroupBox {
            title: qsTr("Importar Tareas")
            Layout.fillWidth: true

            ColumnLayout {
                spacing: theme.spacing.small
                padding: theme.spacing.small

                Text {
                    text: qsTr("Importa tareas desde un archivo .ics generado por otra aplicación de calendario.")
                    font.pixelSize: 11
                    color: theme.colors.textSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: theme.spacing.small

                    Button {
                        text: qsTr("Seleccionar Archivo .ics")
                        onClicked: importDialog.open()
                        Layout.fillWidth: true
                    }

                    Button {
                        text: qsTr("Probar con Datos de Ejemplo")
                        flat: true
                        onClicked: {
                            var exampleIcal = "BEGIN:VCALENDAR\r\nVERSION:2.0\r\nPRODID:-//Example//Example//EN\r\nBEGIN:VEVENT\r\nUID:task_example_1\r\nDTSTART:20241225T090000Z\r\nDTEND:20241225T170000Z\r\nSUMMARY:Tarea de ejemplo importada\r\nCATEGORIES:Personal\r\nSTATUS:CONFIRMED\r\nEND:VEVENT\r\nEND:VCALENDAR\r\n";
                            controller.importTasksFromICal(exampleIcal);
                            importStatus.text = qsTr("Se importaron %1 tareas").arg(controller.taskCount);
                        }
                        Layout.fillWidth: true
                    }
                }

                Text {
                    id: importStatus
                    text: qsTr("Ninguna importación realizada")
                    font.pixelSize: 11
                    color: theme.colors.textMuted
                    Layout.fillWidth: true
                }
            }
        }
    }
}