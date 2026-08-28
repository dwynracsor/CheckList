import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

ColumnLayout {
    id: root

    property var tempItems: []

    signal itemAdded(string text)

    function getItems() {
        return tempItems.slice()
    }

    function clear() {
        tempItems = []
        renderTempItems()
    }

    function renderTempItems() {
        tempItemsModel.clear()
        for (var i = 0; i < tempItems.length; i++) {
            tempItemsModel.append({ text: tempItems[i], index: i })
        }
    }

    // Builder area
    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 200
        color: Theme.cardBackground
        radius: Theme.radiusMedium
        border.color: Theme.borderDefault
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingMedium
            spacing: Theme.spacingSmall

            // Input row
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingSmall

                AppControls.StyledTextArea {
                    id: itemInput
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    placeholderText: "Escribe un ítem y presiona Agregar"
                    wrapMode: TextArea.NoWrap
                    Keys.onReturnPressed: addItemBtn.clicked()
                    Keys.onEnterPressed: addItemBtn.clicked()
                }

                AppControls.ButtonStyle {
                    id: addItemBtn
                    text: "Agregar"
                    onClicked: {
                        var text = itemInput.text.trim()
                        if (text === "") return

                        root.tempItems.push(text)
                        root.renderTempItems()
                        itemInput.text = ""
                        itemInput.forceActiveFocus()
                    }
                }
            }

            // Items list
            ListView {
                id: itemsList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: ListModel { id: tempItemsModel }

                delegate: RowLayout {
                    width: itemsList.width
                    spacing: Theme.spacingSmall

                    Text {
                        text: model.text
                        font.pixelSize: Theme.fontSizeMedium
                        color: Theme.textSecondary
                        Layout.fillWidth: true
                    }

                    Button {
                        text: "✕"
                        font.pixelSize: Theme.fontSizeSmall
                        background: Rectangle { color: "transparent" }
                        contentItem: Text {
                            text: parent.text
                            color: Theme.textDisabled
                        }
                        onClicked: {
                            root.tempItems.splice(model.index, 1)
                            root.renderTempItems()
                        }
                    }
                }

                // Empty state
                Text {
                    visible: tempItemsModel.count === 0
                    anchors.centerIn: parent
                    text: "No hay ítems agregados."
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.textDisabled
                }
            }
        }
    }
}
