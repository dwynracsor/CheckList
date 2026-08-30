import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

ApplicationWindow {
    id: root

    visible: true
    width: 500
    height: 700
    minimumWidth: 400
    minimumHeight: 500
    title: "CheckList"
    color: Theme.background

    // TaskController instance
    Connections {
        target: taskController
        Component.onCompleted: taskController.load()
    }

    Shortcut {
        sequence: "Ctrl+N"
        onActivated: {
            createListForm.visible = true
        }
    }

    Shortcut {
        sequence: "Ctrl+S"
        onActivated: taskController.save()
    }

    Shortcut {
        sequence: "Ctrl+Q"
        onActivated: Qt.quit()
    }

    // Main scrollable content
    ScrollView {
        anchors.fill: parent
        clip: true

        ColumnLayout {
            width: root.width
            spacing: 0
            anchors.margins: Theme.spacingLarge

            // Header
            Header {
                Layout.fillWidth: true
                Layout.bottomMargin: Theme.spacingLarge

                onToggleDarkMode: Theme.darkMode = !Theme.darkMode
                onNotificationClicked: {
                    // Scroll to top / show notifications
                }
            }

            // Search bar
            StyledTextField {
                id: searchField
                Layout.fillWidth: true
                Layout.bottomMargin: Theme.spacingLarge
                placeholderText: "Search list"
                onTextChanged: root.refreshModel()
            }

            // Create list section
            CreateListForm {
                id: createListForm
                Layout.fillWidth: true
                Layout.bottomMargin: Theme.spacingLarge
                visible: true

                onListCreated: function(name, items) {
                    var task = taskController.addTask(name)
                    if (task) {
                        for (var i = 0; i < items.length; i++) {
                            taskController.addItemToTask(taskController.taskCount - 1, items[i])
                        }
                    }
                }
            }

            // My lists section title
            Text {
                text: "My lists"
                font.pixelSize: Theme.fontSizeLarge
                font.weight: Theme.fontWeightBold
                color: Theme.textPrimary
                Layout.bottomMargin: Theme.spacingMedium
            }

            // Lists container
            ColumnLayout {
                id: listsContainer
                Layout.fillWidth: true
                spacing: Theme.spacingMedium

                Repeater {
                    model: ListModel { id: listModel }

                    ListCard {
                        Layout.fillWidth: true
                        taskIndex: model.taskIndex
                        listName: model.name
                        listDate: model.date

                        onItemToggled: function(taskIdx, itemIdx, done) {
                            taskController.toggleItemInTask(taskIdx, itemIdx, done)
                        }

                        onItemRemoved: function(taskIdx, itemIdx) {
                            taskController.removeItemFromTask(taskIdx, itemIdx)
                        }

                        onItemAdded: function(taskIdx, text) {
                            taskController.addItemToTask(taskIdx, text)
                        }

                        onNameEdited: function(taskIdx, newName) {
                            taskController.setTaskName(taskIdx, newName)
                        }

                        onListDeleted: function(taskIdx) {
                            taskController.removeTask(taskIdx)
                        }
                    }
                }

                // Empty state
                ColumnLayout {
                    Layout.fillWidth: true
                    visible: listModel.count === 0
                    spacing: Theme.spacingMedium
                    Layout.topMargin: Theme.spacingXLarge

                    Text {
                        text: "\uD83D\uDCCB"
                        font.pixelSize: 48
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Text {
                        text: "No lists yet"
                        font.pixelSize: Theme.fontSizeLarge
                        font.weight: Theme.fontWeightMedium
                        color: Theme.textDisabled
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Text {
                        text: "Create your first list above"
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.textDisabled
                        Layout.alignment: Qt.AlignHCenter
                        opacity: 0.7
                    }
                }
            }

            // Bottom spacing
            Item { Layout.preferredHeight: Theme.spacingXLarge }
        }
    }

    // ListModel helper
    function refreshModel() {
        listModel.clear()
        var query = searchField.text.toLowerCase()
        for (var i = 0; i < taskController.taskCount; i++) {
            var task = taskController.getTask(i)
            if (!task) continue
            if (query === "" || task.name.toLowerCase().indexOf(query) >= 0) {
                listModel.append({
                    taskIndex: i,
                    name: task.name,
                    date: task.date,
                    itemCount: task.itemCount
                })
            }
        }
    }

    Connections {
        target: taskController
        function onTasksChanged() { refreshModel() }
    }

    Component.onCompleted: refreshModel()
}
