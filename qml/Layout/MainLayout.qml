import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller
    property var authController

    readonly property int topBarHeight: 68
    readonly property int sidebarWidth: 220
    readonly property int contextPanelWidth: 280

    readonly property string currentSection:
        navigationController.currentSection

    signal crashReportsRequested()
    signal settingsRequested()

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

    NavigationController {
        id: navigationController
        controller: root.controller
    }

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    // ---- Боковой сайдбар (как в mashiro) ----
    Sidebar {
        id: sidebar

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        width: root.sidebarWidth

        currentSection: root.currentSection

        onSectionSelected: function(section) {
            navigationController.selectSection(section)
        }
    }

    TopBar {
        id: topBar

        anchors.top: parent.top
        anchors.left: sidebar.right
        anchors.right: parent.right

        height: root.topBarHeight

        controller: root.controller
        authController: root.authController
        currentSection: root.currentSection
        canGoBack: root.canGoBack

        onBackRequested: navigationController.goBack()
        onCrashReportsRequested: root.crashReportsRequested()
    }

    Row {
        anchors.top: topBar.bottom
        anchors.left: sidebar.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        spacing: 0

        Item {
            id: mainArea

            width: parent.width - (root.contextPanelVisible ? root.contextPanelWidth + 1 : 0)
            height: parent.height
            clip: true

            Flickable {
                id: contentScrollView

                anchors.fill: parent
                anchors.topMargin: 20
                anchors.leftMargin: 28
                anchors.rightMargin: 28

                clip: true
                boundsBehavior: Flickable.StopAtBounds

                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                contentWidth: width
                contentHeight: Math.max(
                    pageLoader.height + bottomContentSpacer.height,
                    height
                )

                Loader {
                    id: pageLoader

                    width: contentScrollView.width

                    // Страницы, которым нужна вся высота области просмотра,
                    // берут её отсюда: считать её из item.implicitHeight нельзя —
                    // страница сама тянется за родителем и получается цикл,
                    // из-за которого высота схлопывается в ноль.
                    height: item && item.fillHeight !== true
                        ? item.implicitHeight
                        : contentScrollView.height

                    // Ширина именно биндингом: при разворачивании окна
                    // страница должна переехать вместе с ней.
                    onLoaded: {
                        if (item) {
                            item.width = Qt.binding(function() {
                                return pageLoader.width
                            })
                        }
                    }
                }

                Item {
                    id: bottomContentSpacer

                    width: contentScrollView.width
                    height: 40
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

            width: root.contextPanelVisible ? root.contextPanelWidth : 0
            height: parent.height

            contextType: root.contextType
            controller: root.controller
        }
    }

    Connections {
        target: navigationController

        function onPageLoadRequested(source, properties) {
            pageLoader.setSource(source, properties)
        }
    }

    // Страницы разделов не знают про NavigationController, поэтому сообщают о навигации сигналами
    Connections {
        target: root.currentPageType === "section" &&
                (root.currentSection === "home" || root.currentSection === "genres")
            ? pageLoader.item
            : null

        ignoreUnknownSignals: true

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

        function onWaveRequested() {
            navigationController.selectSection("wave")
        }

        function onReleasesRequested() {
            navigationController.selectSection("releases")
        }

        function onLikedRequested() {
            navigationController.selectSection("liked")
        }

        function onSearchRequested() {
            navigationController.selectSection("search")
        }

        function onStationPageRequested(stationType, stationTag, title) {
            navigationController.openStationPage(stationType, stationTag, title)
        }

        function onGenreRequested(genreId, title, image, color, subGenres) {
            navigationController.openGenrePage(genreId, title, image, color)
        }
    }
}