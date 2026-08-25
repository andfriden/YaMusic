import QtQuick
import QtQuick.Controls.Basic

import YaMusic.Core

import "qml/Layout"
import "qml/Home"

ApplicationWindow {
    id: window

    width: 1440
    height: 900

    minimumWidth: 1100
    minimumHeight: 720

    visible: true

    title: "YaMusic"

    color: "#f5f5f5"

    AppController {
        id: appController

        onStatusChanged: function(message) {
            statusBar.message = message
        }
    }

    Component.onCompleted: {
        console.log("========================================")
        console.log("YaMusic Main.qml CREATED")
        console.log("appController:", appController)
        console.log("currentSection:", mainLayout.currentSection)
        console.log("========================================")

        appController.loadRecommendations()
    }

    Connections {
        target: appController

        function onCurrentAlbumChanged() {
            if (
                appController.albumController !== null &&
                String(
                    appController.albumController.albumTitle
                ).length > 0
            ) {
                mainLayout.currentSection =
                    "albums"
            }
        }

        function onCurrentArtistChanged() {
            if (
                appController.artistController !== null &&
                String(
                    appController.artistController.artistName
                ).length > 0
            ) {
                mainLayout.currentSection =
                    "artists"
            }
        }

        function onCurrentPlaylistChanged() {
            if (
                String(
                    appController.currentPlaylistTitle
                ).length > 0
            ) {
                mainLayout.currentSection =
                    "playlists"
            }
        }
    }

    MainLayout {
        id: mainLayout

        anchors.fill:
            parent

        controller:
            appController

        onSectionSelected:
                function(section) {
            console.log(
                "Main section selected:",
                section
            )
        }
    }

    StatusBar {
        id: statusBar

        visible:
            false

        message:
            "Готово"
    }

    NowPlayingBar {
        id: nowPlayingBar

        anchors.left:
            parent.left

        anchors.right:
            parent.right

        anchors.bottom:
            parent.bottom

        anchors.leftMargin:
            16

        anchors.rightMargin:
            16

        anchors.bottomMargin:
            12

        height:
            110

        controller:
            appController
    }
}