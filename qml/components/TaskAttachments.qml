import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var task       // TaskQml
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    signal attachmentAdded()
    signal attachmentRemoved()

    ColumnLayout {
        Layout.fillWidth: true
        spacing: theme.spacing.small

        RowLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.small

            Text {
                text: qsTr("Adjuntos (%1)").arg(task.attachmentCount)
                font.bold: true
                font.pixelSize: 14
                color: theme.colors.textPrimary
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("+")
                flat: true
                onClicked: attachmentDialog.open()
                Layout.preferredWidth: 35
                Layout.preferredHeight: 35
                tooltip: qsTr("Agregar adjunto")
            }
        }

        ColumnLayout {
            spacing: theme.spacing.extraSmall
            Layout.fillWidth: true

            Repeater {
                model: task.attachmentCount
                delegate: RowLayout {
                    Layout.fillWidth: true
                    spacing: theme.spacing.small

                    Rectangle {
                        width: 4
                        height: 20
                        color: theme.colors.accent
                        radius: 2
                    }

                    Text {
                        text: task.attachmentFileName(index)
                        font.pixelSize: 12
                        color: theme.colors.textPrimary
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    Text {
                        text: root.formatSize(task.attachmentFileSize(index))
                        font.pixelSize: 11
                        color: theme.colors.textSecondary
                        Layout.preferredWidth: 60
                    }

                    Button {
                        text: qsTr("Abrir")
                        flat: true
                        onClicked: {
                            console.log("Opening: " + task.attachmentFilePath(index));
                        }
                        Layout.preferredWidth: 50
                    }

                    Button {
                        text: "×"
                        flat: true
                        color: theme.colors.error
                        onClicked: task.removeAttachment(index)
                        Layout.preferredWidth: 30
                    }
                }
            }

            Text {
                text: task.attachmentCount === 0 ? qsTr("Sin adjuntos") : ""
                font.pixelSize: 11
                color: theme.colors.textMuted
                font.italic: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                visible: task.attachmentCount === 0
            }
        }

        Dialog {
            id: attachmentDialog
            title: qsTr("Agregar Adjunto")
            modal: true

            ColumnLayout {
                spacing: theme.spacing.medium
                padding: theme.spacing.medium
                width: 400

                TextField {
                    id: fileNameField
                    placeholderText: qsTr("Nombre del archivo")
                    Layout.fillWidth: true
                }

                TextField {
                    id: filePathField
                    placeholderText: qsTr("Ruta del archivo")
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: fileTypeCombo
                    model: ["documento", "imagen", "video", "audio", "otro"]
                    currentIndex: 0
                    Layout.fillWidth: true
                }

                RowLayout {
                    Layout.alignment: Qt.AlignRight
                    spacing: theme.spacing.small

                    Button {
                        text: qsTr("Cancelar")
                        flat: true
                        onClicked: attachmentDialog.close()
                        Layout.preferredWidth: 80
                    }

                    Button {
                        text: qsTr("Agregar")
                        onClicked: {
                            var fileName = fileNameField.text.trimmed();
                            var filePath = filePathField.text.trimmed();
                            var fileType = fileTypeCombo.currentText;
                            var fileSize = 0;

                            if (fileName.length > 0 && filePath.length > 0) {
                                task.addAttachment(fileName, filePath, fileType, fileSize);
                                attachmentDialog.close();
                                fileNameField.text = "";
                                filePathField.text = "";
                                attachmentAdded();
                            }
                        }
                        Layout.preferredWidth: 80
                    }
                }
            }
        }
    }

    function formatSize(bytes) {
        if (bytes === 0) return "0 B";
        var k = 1024;
        var sizes = ["B", "KB", "MB", "GB"];
        var i = Math.floor(Math.log(bytes) / Math.log(k));
        return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + " " + sizes[i];
    }
}