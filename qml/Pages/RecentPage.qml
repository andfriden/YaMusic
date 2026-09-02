import QtQuick

Item {
    id: root

    property var controller

    readonly property int pageHeight: 620

    width: parent ? parent.width : 0
    height: pageHeight

    implicitWidth: width
    implicitHeight: pageHeight

    RecentListeningSection {
        anchors.fill: parent

        controller: root.controller
    }

  }