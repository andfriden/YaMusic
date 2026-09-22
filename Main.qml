import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

import "Layout"
import "Home"
import "Components"
import "Pages"

/*
 * Корневой элемент приложения.
 *
 * Экран состоит из трёх слоёв, которые переключаются через
 * window.activeOverlay:
 *
 *   0 — основной экран (MainLayout + NowPlayingBar)
 *   1 — развёрнутый плеер (ExpandedNowPlaying)
 *   2 — текст песни (LyricsView)
 *
 * Основной экран и оверлеи взаимно исключают друг друга по
 * видимости; состояние хранится в одном int, а не в двух bool —
 * это исключает противоречивые комбинации (оба флага true).
 */
ApplicationWindow {
    id: window

    width: 1440
    height: 900
    minimumWidth: 1100
    minimumHeight: 720
    visible: true
    title: qsTr("YaMusic")
    color: AppTheme.background

    // ---- Состояние оверлея ----
    readonly property int overlayNone: 0
    readonly property int overlayNowPlaying: 1
    readonly property int overlayLyrics: 2

    property int activeOverlay: overlayNone

    // ---- Тема ----
    Connections {
        target: appController.themeController

        function onDarkThemeChanged() {
            AppTheme.dark = appController.themeController.darkTheme
        }
    }

    Component.onCompleted: {
        AppTheme.dark = appController.themeController.darkTheme
    }

    // ---- Статус-бар ----
    Connections {
        target: appController

        function onStatusChanged(message) {
            statusBar.show(message)
        }
    }

    // ---- Экран входа (только когда не авторизован) ----
    Loader {
        anchors.fill: parent

        active: !authController.authenticated
        source: active ? "Pages/LoginPage.qml" : ""
    }

    // ---- Основной экран ----
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

    // ---- Оверлеи ----
    // Каждый сам полноэкранный (anchors.fill внутри себя).
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