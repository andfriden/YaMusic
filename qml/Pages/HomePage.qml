import QtQuick

Item {
    id: root

    property var controller


    // =============================================================
    // Page contract
    // =============================================================

    readonly property real pageHeight:
        764

    width:
        parent
            ? parent.width
            : 0

    height:
        pageHeight


    // =============================================================
    // Initial data loading
    // =============================================================

    Component.onCompleted: {

        /*
         * Home starts with My Wave, so load it immediately.
         *
         * AppController already protects against concurrent/redundant
         * loading while a request is active.
         */

        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            root.controller.loadMyWave()
        }


        // =========================================================
        // Debug
        // =========================================================

        console.log(
            "========================================"
        )

        console.log(
            "HomePage CREATED"
        )

        console.log(
            "controller:",
            root.controller
        )

        console.log(
            "width:",
            root.width
        )

        console.log(
            "height:",
            root.height
        )

        console.log(
            "pageHeight:",
            root.pageHeight
        )

        console.log(
            "My Wave loading requested"
        )

        console.log(
            "========================================"
        )
    }


    // =============================================================
    // Content
    // =============================================================

    Column {
        id: content

        anchors.left:
            parent.left

        anchors.right:
            parent.right

        anchors.top:
            parent.top

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

            height:
                190

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