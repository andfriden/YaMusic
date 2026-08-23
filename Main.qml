import QtQuick
import QtQuick.Controls.Basic

import YaMusic.Core

import "qml/Components"
import "qml/Home"
import "qml/Layout"
import "qml/MyWave"
import "qml/Search"

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
        appController.loadRecommendations()
    }

    Connections {
        target: appController

        function onCurrentAlbumChanged() {
            if (
                (appController.currentAlbumTitle || "").length > 0 ||
                appController.albumModel.count > 0
            ) {
                mainLayout.currentSection = "albums"
            }
        }

        function onCurrentArtistChanged() {
            if (
                (appController.currentArtistName || "").length > 0 ||
                appController.artistModel.count > 0
            ) {
                mainLayout.currentSection = "artists"
            }
        }

        function onCurrentPlaylistChanged() {
            if (
                (appController.currentPlaylistTitle || "").length > 0 ||
                appController.playlistModel.count > 0
            ) {
                mainLayout.currentSection = "playlists"
            }
        }
    }

    MainLayout {
        id: mainLayout

        anchors.fill: parent

        anchors.bottomMargin:
            nowPlayingBar.height +
            nowPlayingBar.anchors.bottomMargin +
            10

        controller:
            appController

        Loader {
            id: contentLoader

            anchors.left: parent.left
            anchors.right: parent.right

            width: parent.width

            sourceComponent:
                contentComponentForSection(
                    mainLayout.currentSection)

            onLoaded: {
                if (item) {
                    item.width = width
                }
            }
        }
    }

    StatusBar {
        id: statusBar

        visible: false

        message: "Готово"
    }

    NowPlayingBar {
        id: nowPlayingBar

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        anchors.leftMargin: 16
        anchors.rightMargin: 16
        anchors.bottomMargin: 16

        height: 142

        controller:
            appController
    }

    /*
     * Home
     */

    Component {
        id: homeComponent

        Column {
            width:
                parent
                    ? parent.width
                    : 0

            spacing: 16

            SearchBar {
                width: parent.width

                controller:
                    appController
            }

            MyWaveSection {
                width: parent.width
                height: 190

                controller:
                    appController
            }

            PersonalPlaylistsSection {
                width: parent.width
                height: 190

                controller:
                    appController
            }

            RecentListeningSection {
                width: parent.width
                height: 300

                controller:
                    appController
            }

            Item {
                width: 1
                height: 12
            }

            StatusBar {
                width: parent.width

                message:
                    statusBar.message
            }
        }
    }

    /*
     * Search
     */

    Component {
        id: searchComponent

        Column {
            width:
                parent
                    ? parent.width
                    : 0

            spacing: 16

            SearchBar {
                width: parent.width

                controller:
                    appController
            }

            SearchResultsSection {
                width: parent.width
                height: 620

                controller:
                    appController
            }
        }
    }

    /*
     * My Wave
     */

    Component {
        id: waveComponent

        MyWaveSection {
            width:
                parent
                    ? parent.width
                    : 0

            height: 620

            controller:
                appController
        }
    }

    /*
     * Library
     */

    Component {
        id: libraryComponent

        Column {
            width:
                parent
                    ? parent.width
                    : 0

            spacing: 16

            PersonalPlaylistsSection {
                width: parent.width
                height: 300

                controller:
                    appController
            }
        }
    }

    /*
     * Albums
     */

    Component {
        id: albumComponent

        AlbumView {
            width:
                parent
                    ? parent.width
                    : 0

            height: 620

            controller:
                appController
        }
    }

    /*
     * Artists
     */

    Component {
        id: artistComponent

        ArtistView {
            width:
                parent
                    ? parent.width
                    : 0

            height: 620

            controller:
                appController
        }
    }

    /*
     * Playlists
     */

    Component {
        id: playlistComponent

        PlaylistView {
            width:
                parent
                    ? parent.width
                    : 0

            height: 620

            controller:
                appController
        }
    }

    /*
     * Recently Played
     */

    Component {
        id: recentComponent

        RecentListeningSection {
            width:
                parent
                    ? parent.width
                    : 0

            height: 620

            controller:
                appController
        }
    }

    /*
     * Routing
     */

    function contentComponentForSection(section)
    {
        switch (section) {
            case "search":
                return searchComponent

            case "mywave":
                return waveComponent

            case "library":
                return libraryComponent

            case "albums":
                return albumComponent

            case "artists":
                return artistComponent

            case "playlists":
                return playlistComponent

            case "recent":
                return recentComponent

            case "home":
            default:
                return homeComponent
        }
    }
}