import QtQuick
import QtQuick.Controls.Basic

import YaMusic.Core

import "qml/Layout"

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

        controller: appController

        Loader {
            id: contentLoader

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            width: parent.width

            height:
                    item !== null
                ? Math.max(
                    item.implicitHeight,
                    1
                )
                : 1

            sourceComponent:
                contentComponentForSection(
                    mainLayout.currentSection
                )

            onLoaded: {
                if (!item) {
                    return
                }

                item.width =
                    contentLoader.width
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
        anchors.bottomMargin: 12

        height: 110

        controller: appController
    }

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
                controller: appController
            }

            MyWaveSection {
                width: parent.width
                height: 190

                compactMode: true

                controller: appController
            }

            PersonalPlaylistsSection {
                width: parent.width
                height: 190

                controller: appController
            }

            RecentListeningSection {
                width: parent.width
                height: 300

                controller: appController
            }

            Item {
                width: 1
                height: 12
            }
        }
    }

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
                controller: appController
            }

            SearchResultsSection {
                width: parent.width
                height: 620

                controller: appController
            }
        }
    }

    Component {
        id: waveComponent

        MyWaveSection {
            width:
                parent
                    ? parent.width
                    : 0

            height: 620

            compactMode: false

            controller: appController

            Component.onCompleted: {
                appController.loadMyWave()
            }
        }
    }

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

                controller: appController
            }

            RecentListeningSection {
                width: parent.width
                height: 300

                controller: appController
            }

            Item {
                width: 1
                height: 12
            }
        }
    }

    Component {
        id: albumComponent

        AlbumView {
            width:
                parent
                    ? parent.width
                    : 0

            height: 620

            controller: appController
        }
    }

    Component {
        id: artistComponent

        ArtistPage {
            width:
                parent
                    ? parent.width
                    : 0

            controller:
                appController
        }
    }

    Component {
        id: playlistComponent

        PlaylistView {
            width:
                parent
                    ? parent.width
                    : 0

            height: 620

            controller: appController
        }
    }

    Component {
        id: recentComponent

        RecentListeningSection {
            width:
                parent
                    ? parent.width
                    : 0

            height: 620

            controller: appController
        }
    }

    function contentComponentForSection(section) {
        switch (section) {
            case "search":
                return searchComponent

            case "wave":
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