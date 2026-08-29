import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ChecklistApp

Dialog {
    id: root

    property string titleText: qsTr("Confirmar")
    property string messageText: qsTr("¿Estás seguro?")
    property string confirmText: qsTr("Eliminar")
    property color confirmColor: Theme.danger

    signal confirmed()

    modal: true
    anchors.centerIn: parent
    title: titleText
    standardButtons: Dialog.Cancel

    ColumnLayout {
        width: parent.width
        spacing: Theme.spacingMedium

        Text {
            text: root.messageText
            font.pixelSize: Theme.fontSizeMedium
            color: Theme.textSecondary
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        DangerButton {
            text: root.confirmText
            onClicked: {
                root.confirmed()
                root.close()
            }
            Layout.alignment: Qt.AlignRight
        }
    }
}
