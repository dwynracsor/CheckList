import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

RowLayout {
    id: root

    property string placeholderText: "Agregar ítem..."

    signal addItem(string text)

    spacing: Theme.spacingSmall

    AppControls.StyledTextField {
        id: inputField
        Layout.fillWidth: true
        placeholderText: root.placeholderText
        font.pixelSize: Theme.fontSizeSmall
        Keys.onReturnPressed: addBtn.clicked()
        Keys.onEnterPressed: addBtn.clicked()
    }

    AppControls.PrimaryButton {
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
