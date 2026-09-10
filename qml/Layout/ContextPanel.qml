import QtQuick

Item {
    id: root

    property var controller
    property string contextType: ""


    // =============================================================
    // Context panel loader
    // =============================================================

    Loader {
        id: panelLoader

        anchors.fill: parent

        source: root.sourceForContext(root.contextType)

        onLoaded: {
            if (item === null || item === undefined) {
                return
            }

            item.controller = root.controller
        }
    }


    // =============================================================
    // Controller synchronization
    // =============================================================

    onControllerChanged: {
        if (
            panelLoader.item !== null &&
            panelLoader.item !== undefined
        ) {
            panelLoader.item.controller = root.controller
        }
    }


    // =============================================================
    // Context source
    // =============================================================

    function sourceForContext(type) {
        switch (type) {

            case "artist":
                return "qrc:/qt/qml/YaMusic/Context/ContextPanelArtist.qml"

            case "album":
                return "qrc:/qt/qml/YaMusic/Context/ContextPanelAlbum.qml"

            case "library":
                return "qrc:/qt/qml/YaMusic/Context/ContextPanelLibrary.qml"

            case "mywave":
                return "qrc:/qt/qml/YaMusic/Context/ContextPanelMyWave.qml"

            default:
                return ""
        }
    }
}