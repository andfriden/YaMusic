import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    NavigationController {
        id: navigationController
        controller: root.controller
    }

    readonly property string currentSection:
        navigationController.currentSection

    readonly property string currentPageType:
        navigationController.currentPageType

    readonly property bool canGoBack:
        navigationController.navigationStack.length > 0 &&
        navigationController.currentPageType !== "section"

    readonly property bool contextPanelVisible:
        root.currentPageType === "artist" ||
        root.currentPageType === "album" ||
        root.currentPageType === "playlist"

    readonly property string contextType:
        navigationController.contextTypeForCurrentPage()

    readonly property int topBarHeight: 82
    readonly property int contextPanelWidth: 260

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    TopBar {
        id: topBar

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: root.topBarHeight

        currentSection: root.currentSection
        canGoBack: root.canGoBack

        onSectionSelected: function(section) {
            navigationController.selectSection(section)
        }

        onBackRequested: {
            navigationController.goBack()
        }
    }

    Row {
        anchors.top: topBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        spacing: 0

        Item {
            id: mainArea

            width:
                parent.width -
                (
                    root.contextPanelVisible
                        ? root.contextPanelWidth + 1
                        : 0
                )

            height: parent.height

            clip: true

            ScrollView {
                id: contentScrollView

                anchors.fill: parent

                anchors.topMargin: 10
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.bottomMargin: 0

                clip: true

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }

                contentWidth: availableWidth

                contentHeight:
                    Math.max(
                        pageLoader.height +
                        bottomContentSpacer.height,
                        availableHeight
                    )

                Loader {
                    id: pageLoader

                    width: contentScrollView.availableWidth

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

                        item.width = pageLoader.width
                    }
                }

                Item {
                    id: bottomContentSpacer

                    width: contentScrollView.availableWidth
                    height: 36

                    y: pageLoader.height
                }
            }
        }

        Rectangle {
            width: root.contextPanelVisible ? 1 : 0
            height: parent.height

            color: AppTheme.divider
        }

        ContextPanel {
            id: contextPanel

            width:
                root.contextPanelVisible
                    ? root.contextPanelWidth
                    : 0

            height: parent.height

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            anchors.topMargin: 14

            contextType: root.contextType
            controller: root.controller
        }
    }

    Connections {
        target: navigationController

        function onPageLoadRequested(source, properties) {
            pageLoader.setSource(
                source,
                properties
            )
        }
    }

    Connections {
        target:
                root.currentPageType === "section" &&
            root.currentSection === "home"
            ? pageLoader.item
            : null

        ignoreUnknownSignals: true

        function onChartRequested() {
            navigationController.selectSection("chart")
        }

        function onGenresRequested() {
            navigationController.selectSection("genres")
        }

        function onPlaylistsRequested() {
            navigationController.selectSection("playlists")
        }

        function onSportRequested() {
            navigationController.selectSection("sport")
        }

        function onMyPlaylistsRequested() {
            navigationController.selectSection("library")
        }
    }

    Connections {
        target:
                root.currentPageType === "section" &&
            root.currentSection === "genres"
            ? pageLoader.item
            : null

        ignoreUnknownSignals: true

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