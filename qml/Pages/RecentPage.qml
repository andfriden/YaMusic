import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    width: parent ? parent.width : 0
    height: parent ? parent.height : 620
    implicitWidth: width
    implicitHeight: height

    RecentListeningSection {
        anchors.fill: parent
        controller: root.controller
    }
}