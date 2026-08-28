import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

ApplicationWindow {
    id: root

    visible: true
    width: 800
    height: 600
    minimumWidth: 600
    minimumHeight: 400
    title: "Correcciones"
    color: Theme.background

    // Keyboard shortcuts
    Shortcut {
        sequence: "Ctrl+N"
        onActivated: {
            taskForm.visible = true
            taskForm.forceActiveFocus()
        }
    }

    Shortcut {
        sequence: "Ctrl+S"
        onActivated: {
            controller.save()
            syncStatus.updateStatus("Guardado manualmente")
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: {
            if (taskForm.visible) {
                taskForm.visible = false
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+Q"
        onActivated: Qt.quit()
    }

    // Native menu bar
    menuBar: MenuBar {
        Menu {
            title: "&Archivo"

            Action {
                text: "&Nueva corrección"
                shortcut: "Ctrl+N"
                onTriggered: {
                    taskForm.visible = true
                }
            }

            Action {
                text: "&Guardar"
                shortcut: "Ctrl+S"
                onTriggered: {
                    controller.save()
                    syncStatus.updateStatus("Guardado manualmente")
                }
            }

            MenuSeparator {}

            Action {
                text: "&Salir"
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: "&Editar"

            Action {
                text: "&Deshacer"
                shortcut: "Ctrl+Z"
                onTriggered: {
                    // TODO: Implement undo
                }
            }

            Action {
                text: "&Rehacer"
                shortcut: "Ctrl+Y"
                onTriggered: {
                    // TODO: Implement redo
                }
            }
        }

        Menu {
            title: "&Ver"

            Action {
                text: "Mostrar &formulario"
                shortcut: "Ctrl+T"
                onTriggered: {
                    taskForm.visible = !taskForm.visible
                }
            }
        }

        Menu {
            title: "A&yuda"

            Action {
                text: "Acerca &de..."
                onTriggered: aboutDialog.open()
            }
        }
    }

    // TaskController instance
    TaskController {
        id: controller
        autoSave: true
        autoSaveDelay: 500

        Component.onCompleted: {
            load()
        }

        onSaved: {
            syncStatus.updateStatus("Guardado")
        }

        onLastErrorChanged: {
            if (lastError !== "") {
                syncStatus.updateStatus("Error: " + lastError)
            }
        }
    }

    // About dialog
    Dialog {
        id: aboutDialog
        title: "Acerca de Correcciones"
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Ok

        ColumnLayout {
            spacing: Theme.spacingMedium

            Text {
                text: "📋 Correcciones"
                font.pixelSize: Theme.fontSizeTitle
                font.weight: Theme.fontWeightBold
                color: Theme.textPrimary
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: "Versión 1.0.0"
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.textMuted
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: "Aplicación de escritorio para gestionar\ncorrecciones y checklists"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.textDisabled
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: "Construido con C++ y Qt/QML"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.textDisabled
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    // Main layout
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingXLarge
        spacing: Theme.spacingMedium

        // Title
        Text {
            text: "Correcciones"
            font.pixelSize: Theme.fontSizeTitle
            font.weight: Theme.fontWeightBold
            color: Theme.textPrimary
        }

        // Toggle form button
        ToggleFormButton {
            id: toggleFormBtn
            Layout.fillWidth: true
            isOpen: taskForm.visible
        }

        // Task form
        TaskForm {
            id: taskForm
            Layout.fillWidth: true
            visible: true

            onTaskSaved: function(name, items) {
                var task = controller.addTask(name)
                if (task) {
                    for (var i = 0; i < items.length; i++) {
                        controller.addItemToTask(controller.taskCount - 1, items[i])
                    }
                    syncStatus.updateStatus("Tarea creada: " + name)
                }
            }
        }

        // Tasks list
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: tasksList
                spacing: Theme.spacingMedium
                model: controller.tasks

                delegate: TaskCard {
                    width: tasksList.width

                    taskIndex: index
                    taskName: modelData.name
                    taskDate: modelData.date
                    taskItemCount: modelData.itemCount
                    taskCorrectionCount: modelData.correctionCount

                    onItemToggled: function(taskIdx, corrIdx, itemIdx, done) {
                        if (corrIdx === -1) {
                            controller.toggleItemInTask(taskIdx, itemIdx, done)
                        } else {
                            controller.toggleItemInCorrection(taskIdx, corrIdx, itemIdx, done)
                        }
                    }

                    onItemRemoved: function(taskIdx, corrIdx, itemIdx) {
                        if (corrIdx === -1) {
                            controller.removeItemFromTask(taskIdx, itemIdx)
                        } else {
                            controller.removeItemFromCorrection(taskIdx, corrIdx, itemIdx)
                        }
                    }

                    onNameEdited: function(taskIdx, corrIdx, newName) {
                        if (corrIdx === -1) {
                            controller.setTaskName(taskIdx, newName)
                        } else {
                            controller.setCorrectionName(taskIdx, corrIdx, newName)
                        }
                    }

                    onItemAdded: function(taskIdx, corrIdx, text) {
                        if (corrIdx === -1) {
                            controller.addItemToTask(taskIdx, text)
                        } else {
                            controller.addItemToCorrection(taskIdx, corrIdx, text)
                        }
                    }

                    onTaskDeleted: function(taskIdx) {
                        controller.removeTask(taskIdx)
                        syncStatus.updateStatus("Tarea eliminada")
                    }

                    onCorrectionDeleted: function(taskIdx, corrIdx) {
                        controller.removeCorrectionFromTask(taskIdx, corrIdx)
                        syncStatus.updateStatus("Corrección eliminada")
                    }

                    onCorrectionAdded: function(taskIdx, name) {
                        controller.addCorrectionToTask(taskIdx, name)
                        syncStatus.updateStatus("Corrección agregada")
                    }
                }

                // Empty state
                EmptyState {
                    visible: controller.taskCount === 0
                    width: tasksList.width
                    height: 200
                }
            }
        }

        // Sync status
        SyncStatus {
            id: syncStatus
            Layout.fillWidth: true
            statusText: controller.isLoaded ? "Cargado" : "Cargando..."
        }
    }
}
