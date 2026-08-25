import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    property string currentSection:
        "home"

    property string contextType:
        contextTypeForSection(
            currentSection
        )

    signal sectionSelected(
        string section
    )

    Rectangle {
        anchors.fill:
            parent

        color:
            "#f5f5f5"
    }

    Row {
        anchors.fill:
            parent

        spacing:
            0

        // =========================================================
        // Sidebar
        // =========================================================

        Sidebar {
            id: sidebar

            width:
                205

            height:
                parent.height

            currentSection:
                root.currentSection

            onSectionSelected:
                    function(section) {

                root.currentSection =
                    section

                root.sectionSelected(
                    section
                )

                console.log(
                    "Main section selected:",
                    section
                )
            }
        }

        Rectangle {
            width:
                1

            height:
                parent.height

            color:
                "#dddddd"
        }

        // =========================================================
        // Main content
        // =========================================================

        Item {
            id: mainArea

            width:
                parent.width -
                sidebar.width -
                contextPanel.width -
                2

            height:
                parent.height

            clip:
                true

            ScrollView {
                id: contentScrollView

                anchors.fill:
                    parent

                anchors.topMargin:
                    10

                anchors.leftMargin:
                    20

                anchors.rightMargin:
                    20

                anchors.bottomMargin:
                    126

                clip:
                    true

                ScrollBar.vertical:
                    ScrollBar {
                        policy:
                            ScrollBar.AsNeeded
                    }

                Item {
                    id: contentHost

                    width:
                        contentScrollView.availableWidth

                    height:
                        Math.max(
                            pageLoader.height,
                            contentScrollView.availableHeight
                        )

                    clip:
                        false

                    Loader {
                        id: pageLoader

                        anchors.left:
                            parent.left

                        anchors.right:
                            parent.right

                        anchors.top:
                            parent.top

                        width:
                            parent.width

                        height:
                                item !== null
                            ? (
                                    item.pageHeight !== undefined
                                    ? item.pageHeight
                                    : (
                                            item.implicitHeight > 0
                                            ? item.implicitHeight
                                            : item.height
                                    )
                            )
                            : 1

                        onLoaded: {

                            if (!item) {
                                return
                            }

                            item.width =
                                pageLoader.width

                            console.log(
                                "========================================"
                            )

                            console.log(
                                "MainLayout PageLoader"
                            )

                            console.log(
                                "section:",
                                root.currentSection
                            )

                            console.log(
                                "page:",
                                item
                            )

                            console.log(
                                "page size:",
                                item.width,
                                item.height
                            )

                            console.log(
                                "pageHeight:",
                                    item.pageHeight !== undefined
                                    ? item.pageHeight
                                    : "undefined"
                            )

                            console.log(
                                "page controller:",
                                item.controller
                            )

                            console.log(
                                "========================================"
                            )
                        }
                    }
                }
            }
        }

        Rectangle {
            width:
                1

            height:
                parent.height

            color:
                "#dddddd"
        }

        // =========================================================
        // Context panel
        // =========================================================

        ContextPanel {
            id: contextPanel

            width:
                260

            height:
                parent.height

            anchors.top:
                parent.top

            anchors.bottom:
                parent.bottom

            anchors.topMargin:
                14

            anchors.bottomMargin:
                126

            contextType:
                root.contextType

            controller:
                root.controller
        }
    }

    // =============================================================
    // Page loading
    // =============================================================

    function loadCurrentPage() {

        const source =
            pageSourceForSection(
                root.currentSection
            )

        console.log(
            "========================================"
        )

        console.log(
            "MainLayout loading page"
        )

        console.log(
            "section:",
            root.currentSection
        )

        console.log(
            "source:",
            source
        )

        console.log(
            "controller:",
            root.controller
        )

        console.log(
            "========================================"
        )

        pageLoader.setSource(
            source,
            {
                controller:
                root.controller
            }
        )
    }

    onCurrentSectionChanged: {
        loadCurrentPage()
    }

    onControllerChanged: {

        /*
         * Controller may be assigned after MainLayout
         * itself has already been constructed.
         *
         * Reload current page so that the initial
         * property is passed during object creation.
         */
        if (
            root.controller !== null
        ) {
            loadCurrentPage()
        }
    }

    Component.onCompleted: {

        console.log(
            "========================================"
        )

        console.log(
            "MainLayout CREATED"
        )

        console.log(
            "controller:",
            root.controller
        )

        console.log(
            "currentSection:",
            root.currentSection
        )

        console.log(
            "mainArea:",
            mainArea.width,
            mainArea.height
        )

        console.log(
            "contentHost:",
            contentHost.width,
            contentHost.height
        )

        console.log(
            "========================================"
        )

        if (
            root.controller !== null
        ) {
            loadCurrentPage()
        }
    }

    // =============================================================
    // Page routing
    // =============================================================

    function pageSourceForSection(
        section
    ) {
        switch (section) {

            case "home":
                return "../Pages/HomePage.qml"

            case "search":
                return "../Pages/SearchPage.qml"

            case "wave":
                return "../Pages/MyWavePage.qml"

            case "library":
                return "../Pages/LibraryPage.qml"

            case "albums":
                return "../Pages/AlbumPage.qml"

            case "artists":
                return "../Pages/ArtistPage.qml"

            case "playlists":
                return "../Pages/PlaylistPage.qml"

            case "recent":
                return "../Pages/RecentPage.qml"

            case "liked":
                return "../Pages/HomePage.qml"

            default:
                return "../Pages/HomePage.qml"
        }
    }

    // =============================================================
    // Context routing
    // =============================================================

    function contextTypeForSection(
        section
    ) {
        switch (section) {

            case "artists":
                return "artist"

            case "albums":
                return "album"

            case "playlists":
                return "playlist"

            case "library":
                return "library"

            default:
                return "home"
        }
    }
}