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
    title: qsTr("YaMusic")
    color: AppTheme.background

    // int вместо двух bool, чтобы исключить противоречивые
    // комбинации (открыты оба оверлея сразу)
    readonly property int overlayNone: 0
    readonly property int overlayNowPlaying: 1
    readonly property int overlayLyrics: 2

    property int activeOverlay: overlayNone

    Connections {
        target: appController.themeController

        function onDarkThemeChanged() {
            AppTheme.dark = appController.themeController.darkTheme
        }
    }

    Component.onCompleted: {
        AppTheme.dark = appController.themeController.darkTheme
    }

    Connections {
        target: appController

        function onStatusChanged(message) {
            statusBar.show(message)
        }
    }

    // Только когда не авторизован
    Loader {
        anchors.fill: parent

        active: !authController.authenticated
        source: active ? "Pages/LoginPage.qml" : ""
    }

    Column {
        anchors.fill: parent
        spacing: 0

        visible: authController.authenticated &&
            window.activeOverlay === window.overlayNone

        MainLayout {
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

            onExpandedRequested: window.activeOverlay = window.overlayNowPlaying
            onLyricsRequested: {
                appController.lyricsController.loadLyrics()
                window.activeOverlay = window.overlayLyrics
            }
        }
    }

    // Оверлеи сами растягиваются на весь экран
    ExpandedNowPlaying {
        anchors.fill: parent

        visible: authController.authenticated &&
            window.activeOverlay === window.overlayNowPlaying

        controller: appController

        onClosed: window.activeOverlay = window.overlayNone
    }

    LyricsView {
        anchors.fill: parent

        visible: authController.authenticated &&
            window.activeOverlay === window.overlayLyrics

        controller: appController

        onClosed: window.activeOverlay = window.overlayNone
    }

    StatusBar {
        id: statusBar
    }
}