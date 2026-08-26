import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    property string contextType: "home"

    // =============================================================
    // Context panel routing
    // =============================================================

    Loader {
        id: panelLoader

        anchors.fill: parent

        source:
            root.sourceForContext(
                root.contextType
            )

        onLoaded: {
            if (!item) {
                return
            }

            item.controller =
                root.controller
        }
    }

    // =============================================================
    // Controller forwarding
    // =============================================================

    onControllerChanged: {
        if (
            panelLoader.item !== null &&
            panelLoader.item !== undefined
        ) {
            panelLoader.item.controller =
                root.controller
        }
    }

    // =============================================================
    // Context routing
    // =============================================================

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

    // =============================================================
    // Debug
    // =============================================================

    onContextTypeChanged: {
        console.log(
            "ContextPanel:"
        )

        console.log(
            "contextType:",
            root.contextType
        )

        console.log(
            "source:",
            root.sourceForContext(
                root.contextType
            )
        )
    }

    Component.onCompleted: {

        console.log(
            "========== ContextPanel =========="
        )

        console.log(
            "controller:",
            root.controller
        )

        console.log(
            "contextType:",
            root.contextType
        )

        console.log(
            "source:",
            root.sourceForContext(
                root.contextType
            )
        )

        console.log(
            "================================="
        )
    }
}