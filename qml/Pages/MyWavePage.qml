import QtQuick

Item {
    id: root

    property var controller

    readonly property int pageHeight: 700

    width: parent ? parent.width : 0
    height: pageHeight

    implicitWidth: width
    implicitHeight: pageHeight

    MyWaveSection {
        anchors.fill: parent

        controller: root.controller

        compactMode: false
    }

    Component.onCompleted: {
        if (root.controller !== null) {
            root.controller.loadMyWave()
        }
    }
}