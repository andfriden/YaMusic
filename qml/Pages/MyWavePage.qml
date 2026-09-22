import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

/*
 * Страница «Моя волна» — полная (не компактная) версия секции.
 */
Item {
    id: root

    property var controller

    anchors.fill: parent

    implicitWidth: width
    implicitHeight: height

    MyWaveSection {
        anchors.fill: parent

        controller: root.controller
        compactMode: false
    }

    Component.onCompleted: {
        if (root.controller !== null && root.controller !== undefined) {
            root.controller.loadMyWave()
        }
    }
}