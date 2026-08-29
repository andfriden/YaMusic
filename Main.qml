import QtQuick
import QtQuick.Controls.Basic

import YaMusic.Core

import "qml/Layout"
import "qml/Home"
import "qml/Components"


ApplicationWindow {
    id: window

    width:
        1440

    height:
        900

    minimumWidth:
        1100

    minimumHeight:
        720

    visible:
        true

    title:
        "YaMusic"

    color:
        AppTheme.background


    property bool expandedNowPlayingVisible:
        false


    // =============================================================
    // App Controller
    // =============================================================

    AppController {
        id: appController

        onStatusChanged:
                function(message) {
            statusBar.message =
                message
        }
    }


    // =============================================================
    // Startup
    // =============================================================

    Component.onCompleted:
    {
        console.log(
            "========================================"
        )

        console.log(
            "YaMusic Main.qml CREATED"
        )

        console.log(
            "appController:",
            appController
        )

        console.log(
            "currentSection:",
            mainLayout.currentSection
        )

        console.log(
            "========================================"
        )


        appController.loadRecommendations()
    }


    // =============================================================
    // Controller connections
    // =============================================================

    Connections {
        target:
            appController


        function onCurrentAlbumChanged()
        {
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


        function onCurrentArtistChanged()
        {
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


        function onCurrentPlaylistChanged()
        {
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


    // =============================================================
    // Main Layout
    // =============================================================

    MainLayout {
        id: mainLayout

        anchors.fill:
            parent

        controller:
            appController

        visible:
            !window.expandedNowPlayingVisible

        onSectionSelected:
                function(section)
            {
                console.log(
                    "Main section selected:",
                    section
                )
            }
    }


    // =============================================================
    // Status Bar
    // =============================================================

    StatusBar {
        id: statusBar

        visible:
            false

        message:
            "Готово"
    }


    // =============================================================
    // Mini Player
    // =============================================================

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

        visible:
            !window.expandedNowPlayingVisible

        onExpandedRequested:
        {
            window.expandedNowPlayingVisible =
                true
        }
    }


    // =============================================================
    // Expanded Now Playing
    // =============================================================

    ExpandedNowPlaying {
        id: expandedNowPlaying

        anchors.fill:
            parent

        visible:
            window.expandedNowPlayingVisible

        controller:
            appController

        onClosed:
        {
            window.expandedNowPlayingVisible =
                false
        }
    }
}