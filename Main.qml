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
    {}


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

                artistId


            mainLayout.currentSection =
                "artists"
        }


        // ---------------------------------------------------------
        // Playlist
        // ---------------------------------------------------------

        function onPlaylistPageRequested()
        {
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