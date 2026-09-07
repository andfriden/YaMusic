import QtQuick
import QtQuick.Controls.Basic


Item {
    id: root


    // =============================================================
    // Controller
    // =============================================================

    property var controller


    // =============================================================
    // Navigation controller
    // =============================================================

    NavigationController {
        id: navigationController

        controller:
            root.controller
    }


    // =============================================================
    // Navigation state
    // =============================================================

    readonly property string currentSection:
        navigationController.currentSection

    readonly property string currentPageType:
        navigationController.currentPageType

    readonly property string currentDetailId:
        navigationController.currentDetailId

    readonly property string currentGenreTitle:
        navigationController.currentGenreTitle

    readonly property string currentGenreImage:
        navigationController.currentGenreImage

    readonly property string currentGenreColor:
        navigationController.currentGenreColor

    readonly property var navigationStack:
        navigationController.navigationStack


    // =============================================================
    // Layout
    // =============================================================

    readonly property int topBarHeight:
        82

    readonly property int contextPanelWidth:
        260


    readonly property bool contextPanelVisible:
        root.currentPageType === "artist" ||
        root.currentPageType === "album" ||
        root.currentPageType === "playlist"


    readonly property string contextType:
        navigationController.contextTypeForCurrentPage()


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
    // Top bar
    // =============================================================

    TopBar {
        id: topBar

        anchors.left:
            parent.left

        anchors.right:
            parent.right

        anchors.top:
            parent.top

        height:
            root.topBarHeight

        currentSection:
            root.currentSection

        onSectionSelected:
                function(section) {
            navigationController.selectSection(
                section
            )
        }
    }


    // =============================================================
    // Main content
    // =============================================================

    Row {
        id: contentRow

        anchors.left:
            parent.left

        anchors.right:
            parent.right

        anchors.top:
            topBar.bottom

        anchors.bottom:
            parent.bottom

        spacing:
            0


        // =========================================================
        // Main area
        // =========================================================

        Item {
            id: mainArea

            width:
                parent.width -
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
                        ? item.implicitHeight
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
                    navigationController.goBack()
                }
            }
        }


        // =========================================================
        // Context divider
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

            contextType:
                root.contextType

            controller:
                root.controller
        }
    }


    // =============================================================
    // Page loading
    // =============================================================

    Connections {
        target:
            navigationController


        function onPageLoadRequested(
            source,
            properties
        ) {
            pageLoader.setSource(
                source,
                properties
            )
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
            navigationController.selectSection(
                "chart"
            )
        }


        function onGenresRequested() {
            navigationController.selectSection(
                "genres"
            )
        }


        function onPlaylistsRequested() {
            navigationController.selectSection(
                "playlists"
            )
        }


        function onSportRequested() {
            navigationController.selectSection(
                "sport"
            )
        }


        function onMyPlaylistsRequested() {
            navigationController.selectSection(
                "library"
            )
        }
    }


    // =============================================================
    // Genre navigation
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
            navigationController.openGenrePage(
                genreId,
                title,
                image,
                color
            )
        }
    }
}