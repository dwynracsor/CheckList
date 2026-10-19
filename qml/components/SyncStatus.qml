import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    Timer {
        id: syncTimer
        interval: controller.syncManager().config().syncIntervalSeconds * 1000
        running: controller.syncManager().config().autoSync && controller.syncManager().config().serverUrl.length > 0
        repeat: true
        onTriggered: controller.syncNow()
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: theme.spacing.small

        RowLayout {
            Layout.fillWidth: true
            spacing: theme.spacing.small

            Rectangle {
                width: 12
                height: 12
                radius: 6
                color: controller.syncIsOnline() ? theme.colors.success : theme.colors.error
            }

            Text {
                text: controller.syncStatusString()
                font.bold: true
                font.pixelSize: 14
                color: theme.colors.textPrimary
            }

            Text {
                text: "(" + controller.syncPendingCount() + " pendientes)"
                font.pixelSize: 12
                color: theme.colors.textSecondary
            }

            Text {
                text: ""
                Layout.fillWidth: true
            }

            Button {
                text: controller.syncIsOnline() ? qsTr("Sincronizar") : qsTr("Sin conexión")
                flat: true
                enabled: controller.syncIsOnline() && controller.syncPendingCount() > 0
                onClicked: controller.syncNow()
                Layout.preferredWidth: 80
            }
        }

        GroupBox {
            title: qsTr("Configuración de Servidor")
            Layout.fillWidth: true

            ColumnLayout {
                spacing: theme.spacing.small
                padding: theme.spacing.small

                TextField {
                    id: serverUrlField
                    placeholderText: qsTr("https://api.tuservidor.com/sync")
                    text: ""
                    Layout.fillWidth: true
                    onTextChanged: {
                    }
                }

                TextField {
                    id: apiKeyField
                    placeholderText: qsTr("API Key (opcional)")
                    echoMode: TextInput.Password
                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: theme.spacing.small
                    Layout.fillWidth: true

                    SpinBox {
                        id: intervalSpin
                        from: 60
                        to: 86400
                        value: 300
                        Layout.fillWidth: true
                    }

                    Text {
                        text: qsTr("segundos entre syncs")
                        font.pixelSize: 11
                        color: theme.colors.textSecondary
                    }
                }

                CheckBox {
                    text: qsTr("Sincronización automática")
                    checked: true
                }

                RowLayout {
                    Layout.alignment: Qt.AlignRight
                    spacing: theme.spacing.small

                    Button {
                        text: qsTr("Guardar Configuración")
                        onClicked: {
                            controller.setSyncConfig(
                                serverUrlField.text,
                                apiKeyField.text,
                                intervalSpin.value,
                                true
                            );
                        }
                        Layout.preferredWidth: 100
                    }

                    Button {
                        text: qsTr("Probar Conexión")
                        flat: true
                        onClicked: {
                            controller.setSyncOnline(serverUrlField.text.length > 0);
                        }
                        Layout.preferredWidth: 100
                    }
                }
            }
        }
    }
}