import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root


    // =============================================================
    // Controller
    // =============================================================

    property var controller


    // =============================================================
    // Current section
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
    // Current genre
    // =============================================================

    property string currentGenreTitle:
        ""

    property string currentGenreImage:
        ""

    property string currentGenreColor:
        ""

    property var currentGenreSubGenres:
        []


    // =============================================================
    // Current subgenre
    // =============================================================

    property string currentSubGenreId:
        ""

    property string currentSubGenreTitle:
        ""

    property string currentSubGenreColor:
        ""


    // =============================================================
    // Navigation stack
    // =============================================================

    property var navigationStack:
        []


    // =============================================================
    // Layout
    // =============================================================

    readonly property int sidebarWidth:
        205

    readonly property int contextPanelWidth:
        260


    // =============================================================
    // Context panel
    // =============================================================

    readonly property bool contextPanelVisible:
        root.currentPageType === "artist" ||
        root.currentPageType === "album" ||
        root.currentPageType === "playlist"


    readonly property string contextType:
        root.contextTypeForCurrentPage()


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
            // Scrollable content
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


                // =================================================
                // Page loader
                // =================================================

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

        ignoreUnknownSignals:
            true

        function onChartRequested() {
            root.selectSection(
                "chart"
            )
        }

        function onGenresRequested() {
            root.selectSection(
                "genres"
            )
        }

        function onPlaylistsRequested() {
            root.selectSection(
                "playlists"
            )
        }
    }


    // =============================================================
    // Genres navigation
    // =============================================================

    Connections {
        target:
                root.currentPageType === "section" &&
            root.currentSection === "genres"
            ? pageLoader.item
            : null

        ignoreUnknownSignals:
            true

        function onGenreRequested(
            genreId,
            title,
            image,
            color,
            subGenres
        ) {
            root.openGenrePage(
                genreId,
                title,
                image,
                color,
                subGenres
            )
        }
    }


    // =============================================================
    // Genre page navigation
    // =============================================================

    Connections {
        target:
                root.currentPageType === "genre"
            ? pageLoader.item
            : null

        ignoreUnknownSignals:
            true

        function onSubGenreRequested(
            subGenreId,
            title,
            color
        ) {
            root.openSubGenrePage(
                subGenreId,
                title,
                color
            )
        }
    }


    // =============================================================
    // Navigation from AppController
    // =============================================================

    Connections {
        target:
            root.controller

        ignoreUnknownSignals:
            true

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
    // Select root section
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

        root.currentGenreTitle =
            ""

        root.currentGenreImage =
            ""

        root.currentGenreColor =
            ""

        root.currentGenreSubGenres =
            []

        root.currentSubGenreId =
            ""

        root.currentSubGenreTitle =
            ""

        root.currentSubGenreColor =
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
    // Open Genre
    // =============================================================

    function openGenrePage(
        genreId,
        title,
        image,
        color,
        subGenres
    ) {
        const id =
            String(
                genreId || ""
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


        root.currentGenreTitle =
            title || ""

        root.currentGenreImage =
            image || ""

        root.currentGenreColor =
            color || ""

        root.currentGenreSubGenres =
            subGenres || []


        root.currentPageType =
            "genre"

        root.currentDetailId =
            id

        root.loadCurrentPage()
    }


    // =============================================================
    // Open SubGenre
    // =============================================================

    function openSubGenrePage(
        subGenreId,
        title,
        color
    ) {
        const id =
            String(
                subGenreId || ""
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


        root.currentSubGenreId =
            id

        root.currentSubGenreTitle =
            title || ""

        root.currentSubGenreColor =
            color || ""


        root.currentPageType =
            "subgenre"

        root.currentDetailId =
            id

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


        if (
            root.currentPageType === "genre"
        ) {
            pageLoader.setSource(
                source,
                {
                    controller:
                    root.controller,

                    genreId:
                    root.currentDetailId,

                    genreTitle:
                    root.currentGenreTitle,

                    genreImage:
                    root.currentGenreImage,

                    genreColor:
                    root.currentGenreColor,

                    genreSubGenres:
                    root.currentGenreSubGenres
                }
            )

            return
        }


        if (
            root.currentPageType === "subgenre"
        ) {
            pageLoader.setSource(
                source,
                {
                    controller:
                    root.controller,

                    subGenreId:
                    root.currentSubGenreId,

                    subGenreTitle:
                    root.currentSubGenreTitle,

                    subGenreColor:
                    root.currentSubGenreColor
                }
            )

            return
        }


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

            case "genre":
                return "../Pages/GenrePage.qml"

            case "subgenre":
                return "../Pages/SubGenrePage.qml"

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

            case "genres":
                return "../Pages/GenresPage.qml"

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

            case "genre":
                return "home"

            case "subgenre":
                return "home"

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

            case "genres":
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

        root.currentGenreTitle =
            ""

        root.currentGenreImage =
            ""

        root.currentGenreColor =
            ""

        root.currentGenreSubGenres =
            []

        root.currentSubGenreId =
            ""

        root.currentSubGenreTitle =
            ""

        root.currentSubGenreColor =
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