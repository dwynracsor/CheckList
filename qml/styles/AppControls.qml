import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

QtObject {
    id: appControls

    readonly property var primaryButton: {
        "text": "",
        "flat": false,
        "Layout": { "preferredWidth": 120, "preferredHeight": 36 }
    }

    readonly property var textField: {
        "placeholderText": "",
        "text": "",
        "Layout": { "fillWidth": true, "preferredHeight": 36 }
    }

    readonly property var comboBox: {
        "model": [],
        "currentIndex": 0,
        "Layout": { "preferredWidth": 120, "preferredHeight": 36 }
    }
}