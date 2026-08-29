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
    title: qsTr("Correcciones")
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
                    taskController.save()
                    syncStatus.updateStatus(qsTr("Guardado manualmente"))
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
            title: qsTr("&Archivo")

            Action {
                text: qsTr("&Nueva corrección")
                shortcut: "Ctrl+N"
                onTriggered: {
                    taskForm.visible = true
                }
            }

            Action {
                text: qsTr("&Guardar")
                shortcut: "Ctrl+S"
                onTriggered: {
                    taskController.save()
                    syncStatus.updateStatus(qsTr("Guardado manualmente"))
                }
            }

            MenuSeparator {}

            Action {
                text: qsTr("&Salir")
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: qsTr("&Editar")

            Action {
                text: qsTr("&Deshacer")
                shortcut: "Ctrl+Z"
                enabled: taskController.canUndo
                onTriggered: taskController.undo()
            }

            Action {
                text: qsTr("&Rehacer")
                shortcut: "Ctrl+Y"
                enabled: taskController.canRedo
                onTriggered: taskController.redo()
            }
        }

        Menu {
            title: qsTr("&Ver")

            Action {
                text: qsTr("Mostrar &formulario")
                shortcut: "Ctrl+T"
                onTriggered: {
                    taskForm.visible = !taskForm.visible
                }
            }

            MenuSeparator {}

            Action {
                text: Theme.darkMode ? qsTr("Modo claro") : qsTr("Modo oscuro")
                onTriggered: Theme.darkMode = !Theme.darkMode
            }
        }

        Menu {
            title: qsTr("A&yuda")

            Action {
                text: qsTr("Acerca &de...")
                onTriggered: aboutDialog.open()
            }
        }
    }

    // TaskController instance (from main.cpp context property)
    Connections {
        target: taskController

        Component.onCompleted: {
            taskController.load()
        }

        function onSaved() {
            syncStatus.updateStatus("Guardado")
        }

        function onLastErrorChanged() {
            if (taskController.lastError !== "") {
                syncStatus.updateStatus("Error: " + taskController.lastError)
            }
        }
    }

    // About dialog
    Dialog {
        id: aboutDialog
        title: qsTr("Acerca de Correcciones")
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
                text: qsTr("Versión 1.0.0")
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.textMuted
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: qsTr("Aplicación de escritorio para gestionar\ncorrecciones y checklists")
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.textDisabled
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: qsTr("Construido con C++ y Qt/QML")
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
            text: qsTr("Correcciones")
            font.pixelSize: Theme.fontSizeTitle
            font.weight: Theme.fontWeightBold
            color: Theme.textPrimary
        }

        // Search field
        StyledTextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: qsTr("Buscar tareas...")
            onTextChanged: tasksList.refreshFilter()
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
                var task = taskController.addTask(name)
                if (task) {
                    for (var i = 0; i < items.length; i++) {
                        taskController.addItemToTask(taskController.taskCount - 1, items[i])
                    }
                    syncStatus.updateStatus(qsTr("Tarea creada: ") + name)
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
                model: ListModel { id: filteredModel }

                Component.onCompleted: refreshFilter()

                function refreshFilter() {
                    filteredModel.clear()
                    var query = searchField.text.toLowerCase()
                    for (var i = 0; i < taskController.taskCount; i++) {
                        var task = taskController.getTask(i)
                        if (!task) continue
                        if (query === "" || task.name.toLowerCase().indexOf(query) >= 0) {
                            filteredModel.append({
                                taskIndex: i,
                                name: task.name,
                                date: task.date,
                                itemCount: task.itemCount,
                                correctionCount: task.correctionCount
                            })
                        }
                    }
                }

                Connections {
                    target: taskController
                    function onTasksChanged() { tasksList.refreshFilter() }
                }

                delegate: TaskCard {
                    width: tasksList.width

                    taskIndex: model.taskIndex
                    taskName: model.name
                    taskDate: model.date
                    taskItemCount: model.itemCount
                    taskCorrectionCount: model.correctionCount

                    onItemToggled: function(taskIdx, corrIdx, itemIdx, done) {
                        if (corrIdx === -1) {
                            taskController.toggleItemInTask(taskIdx, itemIdx, done)
                        } else {
                            taskController.toggleItemInCorrection(taskIdx, corrIdx, itemIdx, done)
                        }
                    }

                    onItemRemoved: function(taskIdx, corrIdx, itemIdx) {
                        if (corrIdx === -1) {
                            taskController.removeItemFromTask(taskIdx, itemIdx)
                        } else {
                            taskController.removeItemFromCorrection(taskIdx, corrIdx, itemIdx)
                        }
                    }

                    onNameEdited: function(taskIdx, corrIdx, newName) {
                        if (corrIdx === -1) {
                            taskController.setTaskName(taskIdx, newName)
                        } else {
                            taskController.setCorrectionName(taskIdx, corrIdx, newName)
                        }
                    }

                    onItemAdded: function(taskIdx, corrIdx, text) {
                        if (corrIdx === -1) {
                            taskController.addItemToTask(taskIdx, text)
                        } else {
                            taskController.addItemToCorrection(taskIdx, corrIdx, text)
                        }
                    }

                    onTaskDeleted: function(taskIdx) {
                        taskController.removeTask(taskIdx)
                        syncStatus.updateStatus(qsTr("Tarea eliminada"))
                    }

                    onCorrectionDeleted: function(taskIdx, corrIdx) {
                        taskController.removeCorrectionFromTask(taskIdx, corrIdx)
                        syncStatus.updateStatus(qsTr("Corrección eliminada"))
                    }

                    onCorrectionAdded: function(taskIdx, name) {
                        taskController.addCorrectionToTask(taskIdx, name)
                        syncStatus.updateStatus(qsTr("Corrección agregada"))
                    }
                }

                // Empty state
                EmptyState {
                    visible: filteredModel.count === 0
                    width: tasksList.width
                    height: 200
                }
            }
        }

        // Sync status
        SyncStatus {
            id: syncStatus
            Layout.fillWidth: true
            statusText: taskController.isLoaded ? qsTr("Cargado") : qsTr("Cargando...")
        }
    }
}
