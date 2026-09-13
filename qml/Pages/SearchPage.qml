import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller


    width:
        parent
            ? parent.width
            : 0

    height:
        parent
            ? parent.height
            : 700

    implicitWidth:
        width

    implicitHeight:
        height

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    Column {
        anchors.fill: parent

        spacing: 16

        SearchBar {
            width: parent.width
            controller: root.controller
        }

        SearchResultsSection {
            width: parent.width
            height: 620
            controller: root.controller
        }
    }
}