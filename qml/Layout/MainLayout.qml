import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root


    // =============================================================
    // Controller
    // =============================================================

    property var controller


    // =============================================================
    // Current root section
    // =============================================================

    property string currentSection:
        "home"


    // =============================================================
    // Current page
    // =============================================================

    property string currentPageType:
        "section"

    property string currentDetailId:
        ""


    // =============================================================
    // Navigation stack
    // =============================================================

    property var navigationStack:
        []


    // =============================================================
    // Context
    // =============================================================

    readonly property string contextType:
        contextTypeForCurrentPage()


    signal sectionSelected(
        string section
    )


    // =============================================================
    // Background
    // =============================================================

    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.backgroundPrimary
    }


    // =============================================================
    // Main layout
    // =============================================================

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
                root.selectSection(
                    section
                )
            }
        }


        // =========================================================
        // Sidebar separator
        // =========================================================

        Rectangle {
            width:
                1

            height:
                parent.height

            color:
                AppTheme.divider
        }


        // =========================================================
        // Main area
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


            // =====================================================
            // Page scroll
            // =====================================================

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


                // -------------------------------------------------
                // Explicit content size
                // -------------------------------------------------

                contentWidth:
                    availableWidth

                contentHeight:
                    Math.max(
                        pageLoader.height,
                        availableHeight
                    )


                // -------------------------------------------------
                // Current page
                // -------------------------------------------------

                Loader {
                    id: pageLoader

                    width:
                        contentScrollView.availableWidth

                    height:
                            item !== null
                        ? Math.max(
                            item.implicitHeight,
                            item.height
                        )
                        : 0


                    onLoaded: {
                        if (
                            !item
                        ) {
                            return
                        }


                        item.width =
                            pageLoader.width
                    }
                }
            }


            // =====================================================
            // Back button
            // =====================================================

            ToolButton {
                id: backButton

                width:
                    38

                height:
                    38

                anchors.left:
                    parent.left

                anchors.top:
                    parent.top

                anchors.leftMargin:
                    8

                anchors.topMargin:
                    8

                z:
                    1000

                visible:
                    root.currentPageType !== "section" &&
                    root.navigationStack.length > 0

                text:
                    "‹"


                contentItem:
                    Text {
                        text:
                            backButton.text

                        color:
                            backButton.hovered
                                ? AppTheme.accent
                                : AppTheme.textPrimary

                        font.pixelSize:
                            30

                        horizontalAlignment:
                            Text.AlignHCenter

                        verticalAlignment:
                            Text.AlignVCenter
                    }


                background:
                    Rectangle {
                        radius:
                            8

                        color:
                            backButton.hovered
                                ? AppTheme.panelHover
                                : "transparent"
                    }


                onClicked: {
                    root.goBack()
                }
            }
        }


        // =========================================================
        // Context separator
        // =========================================================

        Rectangle {
            width:
                1

            height:
                parent.height

            color:
                AppTheme.divider
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
    // Root section navigation
    // =============================================================

    function selectSection(section) {
        root.navigationStack =
            []

        root.currentSection =
            section

        root.currentPageType =
            "section"

        root.currentDetailId =
            ""


        loadCurrentPage()


        root.sectionSelected(
            section
        )
    }


    // =============================================================
    // Open Artist
    // =============================================================

    function openArtistPage(
        artistId
    ) {
        var id =
            String(
                artistId || ""
            ).trim()


        if (
            id.length === 0
        ) {
            return
        }


        root.navigationStack =
            root.navigationStack.concat(
                [
                    {
                        type:
                        root.currentPageType,

                        section:
                        root.currentSection,

                        id:
                        root.currentDetailId
                    }
                ]
            )


        root.currentPageType =
            "artist"

        root.currentDetailId =
            id


        loadCurrentPage()
    }


    // =============================================================
    // Open Album
    // =============================================================

    function openAlbumPage(
        albumId
    ) {
        var id =
            String(
                albumId || ""
            ).trim()


        if (
            id.length === 0
        ) {
            return
        }


        root.navigationStack =
            root.navigationStack.concat(
                [
                    {
                        type:
                        root.currentPageType,

                        section:
                        root.currentSection,

                        id:
                        root.currentDetailId
                    }
                ]
            )


        root.currentPageType =
            "album"

        root.currentDetailId =
            id


        loadCurrentPage()
    }


    // =============================================================
    // Back
    // =============================================================

    function goBack() {
        if (
            root.navigationStack.length === 0
        ) {
            return
        }


        var stack =
            root.navigationStack.slice()


        var previous =
            stack.pop()


        root.navigationStack =
            stack


        root.currentPageType =
            previous.type || "section"

        root.currentSection =
            previous.section || "home"

        root.currentDetailId =
            previous.id || ""


        loadCurrentPage()
    }


    // =============================================================
    // Load current page
    // =============================================================

    function loadCurrentPage() {
        var source =
            pageSourceForCurrentPage()


        pageLoader.setSource(
            source,
            {
                controller:
                root.controller
            }
        )
    }


    // =============================================================
    // Page source
    // =============================================================

    function pageSourceForCurrentPage() {
        switch (
            root.currentPageType
            ) {

            case "artist":
                return "../Pages/ArtistPage.qml"


            case "album":
                return "../Pages/AlbumPage.qml"


            case "section":
            default:
                return pageSourceForSection(
                    root.currentSection
                )
        }
    }


    function pageSourceForSection(
        section
    ) {
        switch (
            section
            ) {

            case "home":
                return "../Pages/HomePage.qml"


            case "search":
                return "../Pages/SearchPage.qml"


            case "wave":
                return "../Pages/MyWavePage.qml"


            case "library":
                return "../Pages/LibraryPage.qml"


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
    // Context
    // =============================================================

    function contextTypeForCurrentPage() {
        switch (
            root.currentPageType
            ) {

            case "artist":
                return "artist"


            case "album":
                return "album"


            case "section":
            default:
                return contextTypeForSection(
                    root.currentSection
                )
        }
    }


    function contextTypeForSection(
        section
    ) {
        switch (
            section
            ) {

            case "home":
                return "home"


            case "search":
                return "home"


            case "wave":
                return "mywave"


            case "library":
                return "library"


            case "playlists":
                return "playlist"


            case "liked":
                return "home"


            case "recent":
                return "home"


            default:
                return "home"
        }
    }


    // =============================================================
    // Navigation requests from AppController
    // =============================================================

    Connections {
        target:
            root.controller


        function onArtistPageRequested(
            artistId
        ) {
            root.openArtistPage(
                artistId
            )
        }


        function onAlbumPageRequested(
            albumId
        ) {
            root.openAlbumPage(
                albumId
            )
        }


        function onPlaylistPageRequested() {
            root.selectSection(
                "playlists"
            )
        }
    }


    // =============================================================
    // Controller
    // =============================================================

    onControllerChanged: {
        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            loadCurrentPage()
        }
    }


    // =============================================================
    // Initial state
    // =============================================================

    Component.onCompleted: {
        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            loadCurrentPage()
        }
    }
}