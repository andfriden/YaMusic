import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    width: parent ? parent.width : 0
    implicitWidth: width
    implicitHeight: content.implicitHeight
    height: implicitHeight

    Component.onCompleted: {
        if (root.controller !== null && root.controller !== undefined) {
            root.controller.loadMyWave()
            root.controller.loadRecommendations()
        }
    }

    Column {
        id: content

        width: parent.width
        spacing: 24

        SearchBar {
            width: content.width
            controller: root.controller
        }

        MyWaveSection {
            width: content.width
            compactMode: true
            controller: root.controller
        }

        PersonalPlaylistsSection {
            width: content.width
            controller: root.controller
            homeMode: true
        }

        RecentListeningSection {
            width: content.width
            controller: root.controller
        }
    }
}