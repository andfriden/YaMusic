import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    readonly property int pageHeight: 700

    width: parent ? parent.width : 0
    height: pageHeight

    implicitWidth: width
    implicitHeight: pageHeight

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    PlaylistView {
        anchors.fill: parent
        controller: root.controller
    }
}