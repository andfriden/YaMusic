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
    // Navigation
    // =============================================================

    Connections {
        target:
            appController


        // ---------------------------------------------------------
        // Album
        // ---------------------------------------------------------

        function onAlbumPageRequested(
            albumId
        )
        {
            console.log(
                "Album page requested:",
                albumId
            )

            mainLayout.currentSection =
                "albums"
        }


        // ---------------------------------------------------------
        // Artist
        // ---------------------------------------------------------

        function onArtistPageRequested(
            artistId
        )
        {
            console.log(
                "Artist page requested:",
                artistId
            )

            mainLayout.currentSection =
                "artists"
        }


        // ---------------------------------------------------------
        // Playlist
        // ---------------------------------------------------------

        function onPlaylistPageRequested()
        {
            console.log(
                "Playlist page requested"
            )

            mainLayout.currentSection =
                "playlist"
        }
    }


    // =============================================================
    // Main Content + Mini Player
    // =============================================================

    Column {
        id: applicationLayout

        anchors.fill:
            parent

        spacing:
            0

        visible:
            !window.expandedNowPlayingVisible


        // =========================================================
        // Main Layout
        // =========================================================

        MainLayout {
            id: mainLayout

            width:
                parent.width

            height:
                parent.height -
                nowPlayingBar.height

            controller:
                appController
        }


        // =========================================================
        // Mini Player
        // =========================================================

        NowPlayingBar {
            id: nowPlayingBar

            width:
                parent.width

            height:
                124

            controller:
                appController


            onExpandedRequested:
            {
                window.expandedNowPlayingVisible =
                    true
            }
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