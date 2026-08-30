import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

ColumnLayout {
    id: root

    property var tempItems: []

    signal listCreated(string name, var items)

    spacing: Theme.spacingMedium

    // Section title row
    RowLayout {
        Layout.fillWidth: true
        spacing: Theme.spacingSmall

        Text {
            text: "Create list"
            font.pixelSize: Theme.fontSizeLarge
            font.weight: Theme.fontWeightBold
            color: Theme.textPrimary
            Layout.fillWidth: true
        }

        // Dropdown arrow
        Text {
            text: "\u25BC"
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.textMuted

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: contentArea.visible = !contentArea.visible
            }
        }
    }

    // Form content
    ColumnLayout {
        id: contentArea
        Layout.fillWidth: true
        spacing: Theme.spacingSmall

        // List name input
        StyledTextField {
            id: listNameField
            Layout.fillWidth: true
            placeholderText: "List name"
        }

        // Item description input + add button
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingSmall

            StyledTextField {
                id: itemDescField
                Layout.fillWidth: true
                placeholderText: "Item description"
                Keys.onReturnPressed: addItem()
                Keys.onEnterPressed: addItem()
            }

            // Add item button (circular, dark)
            Rectangle {
                Layout.preferredWidth: 36
                Layout.preferredHeight: 36
                radius: 18
                color: Theme.darkMode ? "#334155" : "#1e293b"

                Text {
                    anchors.centerIn: parent
                    text: "+"
                    font.pixelSize: 20
                    font.weight: Theme.fontWeightBold
                    color: "#ffffff"
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: addItem()
                }
            }
        }

        // Preview items (no card, just plain list)
        ColumnLayout {
            Layout.fillWidth: true
            Layout.topMargin: Theme.spacingSmall
            spacing: 0
            visible: root.tempItems.length > 0

            Repeater {
                model: ListModel { id: previewModel }

                delegate: RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    spacing: Theme.spacingSmall

                    Rectangle {
                        width: 18
                        height: 18
                        radius: 3
                        border.color: Theme.borderDefault
                        border.width: 1.5
                        color: "transparent"
                        Layout.alignment: Qt.AlignVCenter
                    }

                    Text {
                        text: model.text
                        font.pixelSize: Theme.fontSizeMedium
                        color: Theme.textSecondary
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }

    function addItem() {
        var text = itemDescField.text.trim()
        if (text === "") return

        root.tempItems.push(text)
        renderPreview()
        itemDescField.text = ""
        itemDescField.forceActiveFocus()
    }

    function renderPreview() {
        previewModel.clear()
        for (var i = 0; i < root.tempItems.length; i++) {
            previewModel.append({ text: root.tempItems[i] })
        }
    }

    function createList() {
        var name = listNameField.text.trim()
        if (name === "") {
            listNameField.forceActiveFocus()
            return
        }

        root.listCreated(name, root.tempItems.slice())
        listNameField.text = ""
        root.tempItems = []
        renderPreview()
    }
}
