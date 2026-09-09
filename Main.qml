import QtQuick
import QtQuick.Controls.Basic

import "Layout"
import "Home"
import "Components"
import "Pages"

ApplicationWindow {
    id: window

    width: 1440
    height: 900
    minimumWidth: 1100
    minimumHeight: 720
    visible: true
    title: "YaMusic"
    color: AppTheme.background

    property bool expandedNowPlayingVisible: false

    Connections {
        target: appController

        function onStatusChanged(message) {
            statusBar.message = message
        }

        function onAlbumPageRequested(albumId) {
            mainLayout.currentSection = "albums"
        }

        function onArtistPageRequested(artistId) {
            mainLayout.currentSection = "artists"
        }

        function onPlaylistPageRequested() {
            mainLayout.currentSection = "playlist"
        }
    }

    Button {
        id: logoutButton

        text: "Выйти"

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 12

        visible: authController.authenticated

        z: 100

        onClicked: {
            authController.logout()
        }
    }

    Loader {
        id: pageLoader

        anchors.fill: parent

        active: !authController.authenticated

        source: "Pages/LoginPage.qml"
    }

    Column {
        id: applicationLayout

        anchors.fill: parent
        spacing: 0

        visible: authController.authenticated &&
            !window.expandedNowPlayingVisible

        MainLayout {
            id: mainLayout

            width: parent.width
            height: parent.height - nowPlayingBar.height

            controller: appController
        }

        NowPlayingBar {
            id: nowPlayingBar

            width: parent.width
            height: 124

            controller: appController

            onExpandedRequested: {
                window.expandedNowPlayingVisible = true
            }
        }
    }

    StatusBar {
        id: statusBar

        visible: false
        message: "Готово"
    }

    ExpandedNowPlaying {
        id: expandedNowPlaying

        anchors.fill: parent

        visible: authController.authenticated &&
            window.expandedNowPlayingVisible

        controller: appController

        onClosed: {
            window.expandedNowPlayingVisible = false
        }
    }
}