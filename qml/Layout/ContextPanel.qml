import QtQuick

Item {
    id: root

    property var controller
    property string contextType: "home"

    Loader {
        id: panelLoader

        anchors.fill: parent

        source: root.sourceForContext(root.contextType)

        onLoaded: {
            if (!item) {
                return
            }

            item.controller = root.controller
        }
    }

    onControllerChanged: {
        if (panelLoader.item) {
            panelLoader.item.controller = root.controller
        }
    }

    function sourceForContext(type) {
        switch (type) {
            case "artist":
                return "qrc:/qt/qml/YaMusic/qml/Context/ContextPanelArtist.qml"

            case "album":
                return "qrc:/qt/qml/YaMusic/qml/Context/ContextPanelAlbum.qml"

            case "playlist":
                return "qrc:/qt/qml/YaMusic/qml/Context/ContextPanelPlaylist.qml"

            case "library":
                return "qrc:/qt/qml/YaMusic/qml/Context/ContextPanelLibrary.qml"

            case "mywave":
                return "qrc:/qt/qml/YaMusic/qml/Context/ContextPanelMyWave.qml"

            case "home":
            default:
                return "qrc:/qt/qml/YaMusic/qml/Context/ContextPanelHome.qml"
        }
    }
}