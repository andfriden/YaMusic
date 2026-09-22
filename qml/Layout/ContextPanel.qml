import QtQuick
import YaMusic 1.0

/*
 * Контекстная панель справа: загружает специализированную панель
 * в зависимости от текущего типа контекста (исполнитель, альбом,
 * плейлист, медиатека, моя волна).
 */
Item {
    id: root

    property var controller
    property string contextType: ""

    Loader {
        id: panelLoader

        anchors.fill: parent

        source: root.sourceForContext(root.contextType)

        onLoaded: {
            if (item) {
                item.controller = root.controller
            }
        }
    }

    // Контроллер может смениться, когда панель уже загружена
    onControllerChanged: {
        if (panelLoader.item) {
            panelLoader.item.controller = root.controller
        }
    }

    function sourceForContext(type) {
        switch (type) {
            case "artist":
                return "qrc:/qt/qml/YaMusic/Context/ContextPanelArtist.qml"
            case "album":
                return "qrc:/qt/qml/YaMusic/Context/ContextPanelAlbum.qml"
            case "library":
                return "qrc:/qt/qml/YaMusic/Context/ContextPanelLibrary.qml"
            case "playlist":
                return "qrc:/qt/qml/YaMusic/Context/ContextPanelPlaylist.qml"
            case "mywave":
                return "qrc:/qt/qml/YaMusic/Context/ContextPanelMyWave.qml"
            default:
                return ""
        }
    }
}