import QtQuick

Item {
    id: root


    // =============================================================
    // Controller
    // =============================================================

    property var controller


    // =============================================================
    // Navigation requests
    // =============================================================

    signal chartRequested()
    signal genresRequested()
    signal playlistsRequested()


    // =============================================================
    // Page size
    // =============================================================

    width:
        parent
            ? parent.width
            : 0

    implicitWidth:
        width

    implicitHeight:
        content.implicitHeight

    height:
        implicitHeight


    // =============================================================
    // Initial data loading
    // =============================================================

    Component.onCompleted: {
        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            root.controller.loadMyWave()
            root.controller.loadRecommendations()
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
            24


        // =========================================================
        // Search
        // =========================================================

        SearchBar {
            width:
                content.width

            controller:
                root.controller
        }


        // =========================================================
        // My Wave
        // =========================================================

        MyWaveSection {
            width:
                content.width

            compactMode:
                true

            controller:
                root.controller
        }


        // =========================================================
        // Charts / Genres / Playlists
        // =========================================================

        PersonalChartSection {
            width:
                content.width


            onChartRequested:
                root.chartRequested()


            onGenresRequested:
                root.genresRequested()


            onPlaylistsRequested:
                root.playlistsRequested()
        }


        // =========================================================
        // Personal playlists
        // =========================================================

        PersonalPlaylistsSection {
            width:
                content.width

            controller:
                root.controller

            homeMode:
                true
        }


        // =========================================================
        // Recently listened
        // =========================================================

        RecentListeningSection {
            width:
                content.width

            controller:
                root.controller
        }
    }
}