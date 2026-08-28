import QtQuick

Item {
    id: root

    property var controller


    // =============================================================
    // Page size
    // =============================================================

    readonly property real pageHeight:
        content.implicitHeight


    width:
        parent
            ? parent.width
            : 0

    height:
        pageHeight

    implicitWidth:
        width

    implicitHeight:
        pageHeight


    // =============================================================
    // Initial data loading
    // =============================================================

    Component.onCompleted: {
        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            root.controller.loadMyWave()
        }
    }


    // =============================================================
    // Content
    // =============================================================

    Column {
        id: content

        width:
            parent.width

        spacing:
            16


        // ---------------------------------------------------------
        // Search
        // ---------------------------------------------------------

        SearchBar {
            width:
                content.width

            controller:
                root.controller
        }


        // ---------------------------------------------------------
        // My Wave
        // ---------------------------------------------------------

        MyWaveSection {
            width:
                content.width

            height:
                190

            compactMode:
                true

            controller:
                root.controller
        }


        // ---------------------------------------------------------
        // Personal playlists
        // ---------------------------------------------------------

        PersonalPlaylistsSection {
            width:
                content.width

            controller:
                root.controller
        }


        // ---------------------------------------------------------
        // Recently listened
        // ---------------------------------------------------------

        RecentListeningSection {
            width:
                content.width

            height:
                300

            controller:
                root.controller
        }
    }
}