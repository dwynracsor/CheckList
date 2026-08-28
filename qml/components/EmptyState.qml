import QtQuick
import QtQuick.Layouts
import ChecklistApp

ColumnLayout {
    id: root

    property string emptyText: "No hay correcciones aún. ¡Crea una!"
    property string emoji: "📋"

    spacing: Theme.spacingMedium
    opacity: 0
    scale: 0.8

    Component.onCompleted: {
        appearAnim.start()
    }

    NumberAnimation {
        id: appearAnim
        target: root
        property: "opacity"
        from: 0
        to: 1
        duration: 400
        easing.type: Easing.OutQuad
    }

    NumberAnimation {
        id: scaleAnim
        target: root
        property: "scale"
        from: 0.8
        to: 1
        duration: 400
        easing.type: Easing.OutBack
    }

    Text {
        id: emojiText
        text: root.emoji
        font.pixelSize: 64
        Layout.alignment: Qt.AlignHCenter

        SequentialAnimation on y {
            loops: Animation.Infinite
            NumberAnimation { from: emojiText.y; to: emojiText.y - 8; duration: 1000; easing.type: Easing.InOutQuad }
            NumberAnimation { from: emojiText.y - 8; to: emojiText.y; duration: 1000; easing.type: Easing.InOutQuad }
        }
    }

    Text {
        text: root.emptyText
        font.pixelSize: Theme.fontSizeLarge
        font.weight: Theme.fontWeightMedium
        color: Theme.textDisabled
        Layout.alignment: Qt.AlignHCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Text {
        text: "Haz clic en \"Crear nueva corrección\" para comenzar"
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.textDisabled
        Layout.alignment: Qt.AlignHCenter
        opacity: 0.7
    }
}
