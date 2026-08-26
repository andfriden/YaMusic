import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    property string currentSection: "home"

    property string contextType:
        contextTypeForSection(root.currentSection)

    signal sectionSelected(string section)

    Rectangle {
        anchors.fill: parent
        color: AppTheme.background
    }

    Row {
        anchors.fill: parent
        spacing: 0

        Sidebar {
            id: sidebar

            width: 205
            height: parent.height

            currentSection: root.currentSection

            onSectionSelected: function(section) {
                console.log("========================================")
                console.log("MainLayout received sectionSelected")
                console.log("section:", section)
                console.log("old currentSection:", root.currentSection)

                root.currentSection = section

                console.log("new currentSection:", root.currentSection)
                console.log("new contextType:", root.contextType)

                root.sectionSelected(section)

                console.log("========================================")
            }
        }

        Rectangle {
            width: 1
            height: parent.height
            color: AppTheme.divider
        }

        Item {
            id: mainArea

            width:
                parent.width -
                sidebar.width -
                contextPanel.width -
                2

            height: parent.height
            clip: true

            ScrollView {
                id: contentScrollView

                anchors.fill: parent

                anchors.topMargin: 10
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.bottomMargin: 126

                clip: true

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
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

                    Loader {
                        id: pageLoader

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top

                        width: parent.width

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

                            item.width = pageLoader.width

                            console.log("========================================")
                            console.log("MainLayout PageLoader")
                            console.log("section:", root.currentSection)
                            console.log("contextType:", root.contextType)
                            console.log("source:", pageLoader.source)
                            console.log("page:", item)
                            console.log("page size:", item.width, item.height)

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

                            console.log("========================================")
                        }
                    }
                }
            }
        }

        Rectangle {
            width: 1
            height: parent.height
            color: AppTheme.divider
        }

        ContextPanel {
            id: contextPanel

            width: 260
            height: parent.height

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            anchors.topMargin: 14
            anchors.bottomMargin: 126

            contextType: root.contextType
            controller: root.controller
        }
    }

    function loadCurrentPage() {
        const source =
            pageSourceForSection(
                root.currentSection
            )

        console.log("========================================")
        console.log("MainLayout loading page")
        console.log("section:", root.currentSection)
        console.log("contextType:", root.contextType)
        console.log("source:", source)
        console.log("controller:", root.controller)
        console.log("========================================")

        pageLoader.setSource(
            source,
            {
                controller: root.controller
            }
        )
    }

    onCurrentSectionChanged: {
        console.log("========================================")
        console.log("MainLayout currentSection CHANGED")
        console.log("currentSection:", root.currentSection)
        console.log("contextType:", root.contextType)
        console.log("========================================")

        loadCurrentPage()
    }

    onControllerChanged: {
        console.log("========================================")
        console.log("MainLayout controller CHANGED")
        console.log("controller:", root.controller)
        console.log("========================================")

        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            loadCurrentPage()
        }
    }

    Component.onCompleted: {
        console.log("========================================")
        console.log("MainLayout CREATED")
        console.log("controller:", root.controller)
        console.log("currentSection:", root.currentSection)
        console.log("contextType:", root.contextType)

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
            "contextPanel:",
            contextPanel.width,
            contextPanel.height
        )

        console.log("========================================")

        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            loadCurrentPage()
        }
    }

    function pageSourceForSection(section) {
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
                console.warn(
                    "Unknown section:",
                    section,
                    "-> HomePage"
                )

                return "../Pages/HomePage.qml"
        }
    }

    function contextTypeForSection(section) {
        switch (section) {
            case "home":
                return "home"

            case "search":
                return "home"

            case "wave":
                return "mywave"

            case "library":
                return "library"

            case "albums":
                return "album"

            case "artists":
                return "artist"

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
}