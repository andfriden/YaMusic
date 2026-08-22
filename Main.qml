import QtQuick
import QtQuick.Controls.Basic

import YaMusic.Core

ApplicationWindow {
    id: window

    width: 1200
    height: 980

    minimumWidth: 800
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
        appController.loadRecommendations()
    }

    ScrollView {
        id: contentScrollView

        anchors.top:
            parent.top

        anchors.left:
            parent.left

        anchors.right:
            parent.right

        anchors.bottom:
            nowPlayingBar.top

        anchors.topMargin: 30
        anchors.leftMargin: 30
        anchors.rightMargin: 30
        anchors.bottomMargin: 16

        clip: true

        ScrollBar.vertical:
            ScrollBar {
                policy:
                    ScrollBar.AsNeeded
            }

        Column {
            id: contentColumn

            width:
                contentScrollView.availableWidth

            spacing: 16

            SearchBar {
                width:
                    parent.width

                controller:
                    appController
            }

            MyWaveSection {
                width:
                    parent.width

                height: 190

                controller:
                    appController
            }

            PersonalPlaylistsSection {
                width:
                    parent.width

                height: 190

                controller:
                    appController
            }

            RecentListeningSection {
                width:
                    parent.width

                height: 300

                controller:
                    appController
            }

            PlaylistView {
                width:
                    parent.width

                height: 300

                controller:
                    appController
            }

            AlbumView {
                width:
                    parent.width

                height: 300

                controller:
                    appController
            }

            ArtistView {
                width:
                    parent.width

                height: 300

                controller:
                    appController
            }

            SearchResultsSection {
                width:
                    parent.width

                height: 300

                controller:
                    appController
            }

            StatusBar {
                id: statusBar

                width:
                    parent.width

                message:
                    "Готово"
            }

            Item {
                width: 1

                height: 12
            }
        }
    }

    NowPlayingBar {
        id: nowPlayingBar

        anchors.left:
            parent.left

        anchors.right:
            parent.right

        anchors.bottom:
            parent.bottom

        anchors.leftMargin: 30
        anchors.rightMargin: 30
        anchors.bottomMargin: 20

        height: 142

        controller:
            appController
    }
}