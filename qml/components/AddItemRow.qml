import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

RowLayout {
    id: root

    property string placeholderText: qsTr("Agregar ítem...")

    signal addItem(string text)

    spacing: Theme.spacingSmall

    StyledTextField {
        id: inputField
        Layout.fillWidth: true
        placeholderText: root.placeholderText
        font.pixelSize: Theme.fontSizeSmall
        Keys.onReturnPressed: addBtn.clicked()
        Keys.onEnterPressed: addBtn.clicked()
    }

    PrimaryButton {
        id: addBtn
        text: "+"
        onClicked: {
            var text = inputField.text.trim()
            if (text === "") return
            root.addItem(text)
            inputField.text = ""
            inputField.forceActiveFocus()
        }
    }
}
