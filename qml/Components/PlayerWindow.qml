import QtQuick
import QtQuick.Window
import YaMusic 1.0

// Расширенный плеер в отдельном окне поверх основного. Делит с основным
// окном один QQmlEngine, поэтому AppTheme, appController и image-провайдер
// "yandex" доступны здесь так же, как в любом загружаемом QML. Окно
// непрозрачное и большое по умолчанию; внутренний плеер сам растягивается
// и повторяет фон и акцент основного окна.
Window {
    id: root

    property var controller

    width: 1440
    height: 900
    minimumWidth: 1100
    minimumHeight: 720

    flags: Qt.Window | Qt.WindowSystemMenuHint | Qt.WindowTitleHint |
        Qt.WindowCloseButtonHint

    title: "YaMusic — плеер"
    color: AppTheme.background
    visible: false

    function showPlayer() {
        root.visible = true
        root.requestActivate()
    }

    function hidePlayer() {
        root.visible = false
    }

    ExpandedNowPlaying {
        id: player

        anchors.fill: parent

        controller: root.controller

        onClosed: root.hidePlayer()
    }
}