import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

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
    property bool lyricsVisible: false

Connections {
        target: appController

        function onStatusChanged(message) {
            statusBar.message = message
        }
    }

    Connections {
        target: appController.themeController

        function onDarkThemeChanged() {
            AppTheme.dark = appController.themeController.darkTheme
        }
    }

    Component.onCompleted: {
        AppTheme.dark = appController.themeController.darkTheme
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
            authController: authController
        }

        NowPlayingBar {
            id: nowPlayingBar

            width: parent.width
            height: 124

            controller: appController

            onExpandedRequested: {
                window.expandedNowPlayingVisible = true
            }

            onLyricsRequested: {
                appController.lyricsController.loadLyrics()
                window.lyricsVisible = true
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

    LyricsView {
        id: lyricsView

        anchors.fill: parent

        visible: authController.authenticated &&
            window.lyricsVisible

        controller: appController

        onClosed: {
            window.lyricsVisible = false
        }
    }
}
