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
    // Layout constants
    // =============================================================

    readonly property int sidebarWidth:
        205

    readonly property int contextPanelWidth:
        260


    // =============================================================
    // Context panel visibility
    // =============================================================

    readonly property bool contextPanelVisible:
        root.currentPageType === "artist" ||
        root.currentPageType === "album" ||
        root.currentPageType === "playlist"


    // =============================================================
    // Context type
    // =============================================================

    readonly property string contextType:
        contextTypeForCurrentPage()


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
                root.sidebarWidth

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
                root.sidebarWidth -
                1 -
                (
                    root.contextPanelVisible
                        ? root.contextPanelWidth + 1
                        : 0
                )

            height:
                parent.height

            clip:
                true


            // =====================================================
            // Scrollable page area
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
                    0

                clip:
                    true


                ScrollBar.vertical:
                    ScrollBar {
                        policy:
                            ScrollBar.AsNeeded
                    }


                contentWidth:
                    availableWidth

                contentHeight:
                    Math.max(
                        pageLoader.height +
                        bottomContentSpacer.height,
                        availableHeight
                    )


                Loader {
                    id: pageLoader

                    width:
                        contentScrollView.availableWidth

                    height:
                            item !== null &&
                        item !== undefined
                        ? Math.max(
                            item.implicitHeight,
                            item.height
                        )
                        : 0


                    onLoaded: {
                        if (
                            item === null ||
                            item === undefined
                        ) {
                            return
                        }

                        item.width =
                            pageLoader.width
                    }
                }


                // =================================================
                // Bottom spacer
                // =================================================

                Item {
                    id: bottomContentSpacer

                    width:
                        contentScrollView.availableWidth

                    height:
                        36

                    y:
                        pageLoader.height
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
                root.contextPanelVisible
                    ? 1
                    : 0

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
                root.contextPanelVisible
                    ? root.contextPanelWidth
                    : 0

            height:
                parent.height

            anchors.top:
                parent.top

            anchors.bottom:
                parent.bottom

            anchors.topMargin:
                14

            contextType:
                root.contextType

            controller:
                root.controller
        }
    }


    // =============================================================
    // Home navigation
    // =============================================================

    Connections {
        target:
                root.currentPageType === "section" &&
            root.currentSection === "home"
            ? pageLoader.item
            : null


        function onChartRequested() {
            root.selectSection(
                "chart"
            )
        }


        function onPlaylistsRequested() {
            root.selectSection(
                "playlists"
            )
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
            root.openPlaylistPage()
        }


        function onSearchPageRequested(
            query
        ) {
            root.selectSection(
                "search"
            )
        }
    }


    // =============================================================
    // Root section navigation
    // =============================================================

    function selectSection(
        section
    ) {
        root.navigationStack =
            []

        root.currentSection =
            String(
                section || "home"
            )

        root.currentPageType =
            "section"

        root.currentDetailId =
            ""

        root.loadCurrentPage()
    }


    // =============================================================
    // Open Artist
    // =============================================================

    function openArtistPage(
        artistId
    ) {
        const id =
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


        root.loadCurrentPage()
    }


    // =============================================================
    // Open Album
    // =============================================================

    function openAlbumPage(
        albumId
    ) {
        const id =
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


        root.loadCurrentPage()
    }


    // =============================================================
    // Open Playlist
    // =============================================================

    function openPlaylistPage() {
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
            "playlist"

        root.currentDetailId =
            ""


        root.loadCurrentPage()
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


        const stack =
            root.navigationStack.slice()


        const previous =
            stack.pop()


        root.navigationStack =
            stack


        root.currentPageType =
            previous.type || "section"

        root.currentSection =
            previous.section || "home"

        root.currentDetailId =
            previous.id || ""


        root.loadCurrentPage()
    }


    // =============================================================
    // Load current page
    // =============================================================

    function loadCurrentPage() {
        const source =
            root.pageSourceForCurrentPage()


        pageLoader.setSource(
            source,
            {
                controller:
                root.controller
            }
        )
    }


    // =============================================================
    // Current page source
    // =============================================================

    function pageSourceForCurrentPage() {
        switch (
            root.currentPageType
            ) {

            case "artist":
                return "../Pages/ArtistPage.qml"


            case "album":
                return "../Pages/AlbumPage.qml"


            case "playlist":
                return "../Pages/PlaylistPage.qml"


            case "section":
            default:
                return root.pageSourceForSection(
                    root.currentSection
                )
        }
    }


    // =============================================================
    // Root section source
    // =============================================================

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
                return "../Pages/PlaylistsPage.qml"


            case "recent":
                return "../Pages/RecentPage.qml"


            case "chart":
                return "../Pages/ChartPage.qml"


            case "liked":
                return "../Pages/HomePage.qml"


            case "albums":
                return "../Pages/AlbumPage.qml"


            case "artists":
                return "../Pages/ArtistPage.qml"


            default:
                return "../Pages/HomePage.qml"
        }
    }


    // =============================================================
    // Context type
    // =============================================================

    function contextTypeForCurrentPage() {
        switch (
            root.currentPageType
            ) {

            case "artist":
                return "artist"


            case "album":
                return "album"


            case "playlist":
                return "playlist"


            case "section":
            default:
                return root.contextTypeForSection(
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
                return "home"


            case "recent":
                return "home"


            case "chart":
                return "home"


            case "liked":
                return "home"


            default:
                return "home"
        }
    }


    // =============================================================
    // Controller changed
    // =============================================================

    onControllerChanged: {
        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            root.loadCurrentPage()
        }
    }


    // =============================================================
    // Initial state
    // =============================================================

    Component.onCompleted: {
        root.currentSection =
            "home"

        root.currentPageType =
            "section"

        root.currentDetailId =
            ""

        root.navigationStack =
            []

        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            root.loadCurrentPage()
        }
    }
}