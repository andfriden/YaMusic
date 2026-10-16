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

            onCrashReportsRequested: crashReportsDialog.open()
            onSettingsRequested: settingsWindow.showSettings()
        }

        NowPlayingBar {
            id: nowPlayingBar

            width: parent.width
            height: 124

            controller: appController

            onExpandedRequested: playerWindow.showPlayer()
            onLyricsRequested: {
                appController.lyricsController.loadLyrics()
                window.activeOverlay = window.overlayLyrics
            }
            onMiniPlayerRequested: miniPlayer.showPlayer()
        }
    }

    // Тексты песен остаются оверлеем в основном окне
    LyricsView {
        anchors.fill: parent

        visible: authController.authenticated &&
            window.activeOverlay === window.overlayLyrics

        controller: appController

        onClosed: window.activeOverlay = window.overlayNone
    }

    MiniPlayer {
        id: miniPlayer

        controller: appController

        onExpandedRequested: playerWindow.showPlayer()
    }

    PlayerWindow {
        id: playerWindow

        controller: appController
    }

    SettingsWindow {
        id: settingsWindow

        controller: appController
        authController: authController

        onCrashReportsRequested: crashReportsDialog.open()
    }

    StatusBar {
        id: statusBar
    }

    // Отчёты о падениях: диалог живёт в окне, а не в топбаре, — тогда он
    // открывается после падения и до входа в аккаунт тоже.
    CrashReportsDialog {
        id: crashReportsDialog

        controller: appController
    }
}