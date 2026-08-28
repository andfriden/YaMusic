import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller


    readonly property int pageHeight:
        520


    width:
        parent
            ? parent.width
            : 0

    height:
        pageHeight

    implicitWidth:
        width

    implicitHeight:
        pageHeight


    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.backgroundPrimary
    }


    Column {
        anchors.fill:
            parent

        anchors.margins:
            20

        spacing:
            24


        // =========================================================
        // Title
        // =========================================================

        Label {
            width:
                parent.width

            text:
                "Медиатека"

            color:
                AppTheme.textPrimary

            font.pixelSize:
                28

            font.bold:
                true
        }


        // =========================================================
        // Playlists
        // =========================================================

        Column {
            width:
                parent.width

            spacing:
                12


            Label {
                text:
                    "Мои плейлисты"

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    20

                font.bold:
                    true
            }


            // =====================================================
            // Loading
            // =====================================================

            Label {
                text:
                    "Загрузка плейлистов..."

                color:
                    AppTheme.textSecondary

                font.pixelSize:
                    13

                visible:
                    root.controller !== null &&
                    root.controller !== undefined &&
                    root.controller.loadingLibraryPlaylists
            }


            // =====================================================
            // Playlist list
            // =====================================================

            ListView {
                id: playlistsView

                width:
                    parent.width

                height:
                    220

                orientation:
                    ListView.Horizontal

                spacing:
                    14

                clip:
                    true

                boundsBehavior:
                    Flickable.StopAtBounds


                model:
                        root.controller !== null &&
                    root.controller !== undefined
                    ? root.controller.libraryPlaylistsModel
                    : null


                ScrollBar.horizontal:
                    ScrollBar {
                        policy:
                            ScrollBar.AsNeeded
                    }


                delegate:
                    Rectangle {
                        id: playlistCard


                        required property int index
                        required property string uid
                        required property int kind
                        required property string title
                        required property string coverUri
                        required property int trackCount


                        width:
                            170

                        height:
                            200

                        radius:
                            12

                        color:
                            mouseArea.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary

                        border.width:
                            1

                        border.color:
                            AppTheme.borderSubtle

                        clip:
                            true


                        // =================================================
                        // Artwork
                        // =================================================

                        Rectangle {
                            id: artworkContainer

                            anchors.top:
                                parent.top

                            anchors.left:
                                parent.left

                            anchors.right:
                                parent.right

                            height:
                                140

                            color:
                                AppTheme.artworkPlaceholder


                            Image {
                                id: artwork

                                anchors.fill:
                                    parent

                                source:
                                        playlistCard.coverUri.length > 0
                                    ? "image://yandex/" +
                                    playlistCard.coverUri
                                    : ""

                                sourceSize:
                                    Qt.size(
                                        280,
                                        280
                                    )

                                fillMode:
                                    Image.PreserveAspectCrop

                                asynchronous:
                                    true

                                cache:
                                    true

                                smooth:
                                    true

                                visible:
                                    status === Image.Ready
                            }


                            Label {
                                anchors.centerIn:
                                    parent

                                text:
                                    "♪"

                                color:
                                    AppTheme.textSecondary

                                font.pixelSize:
                                    32

                                visible:
                                    artwork.status !==
                                    Image.Ready
                            }
                        }


                        // =================================================
                        // Title
                        // =================================================

                        Label {
                            id: titleLabel

                            anchors.left:
                                parent.left

                            anchors.right:
                                parent.right

                            anchors.top:
                                artworkContainer.bottom

                            anchors.leftMargin:
                                10

                            anchors.rightMargin:
                                10

                            anchors.topMargin:
                                8

                            text:
                                    playlistCard.title.length > 0
                                ? playlistCard.title
                                : qsTr("Плейлист")

                            color:
                                AppTheme.textPrimary

                            font.pixelSize:
                                13

                            font.bold:
                                true

                            elide:
                                Text.ElideRight
                        }


                        // =================================================
                        // Track count
                        // =================================================

                        Label {
                            anchors.left:
                                parent.left

                            anchors.right:
                                parent.right

                            anchors.top:
                                titleLabel.bottom

                            anchors.leftMargin:
                                10

                            anchors.rightMargin:
                                10

                            anchors.topMargin:
                                3

                            text:
                                    playlistCard.trackCount > 0
                                ? qsTr("%1 треков")
                                    .arg(
                                    playlistCard.trackCount
                                )
                                : ""

                            color:
                                AppTheme.textSecondary

                            font.pixelSize:
                                11

                            elide:
                                Text.ElideRight
                        }


                        // =================================================
                        // Click
                        // =================================================

                        MouseArea {
                            id: mouseArea

                            anchors.fill:
                                parent

                            hoverEnabled:
                                true

                            cursorShape:
                                Qt.PointingHandCursor


                            onClicked: {
                                if (
                                    root.controller === null ||
                                    root.controller === undefined
                                ) {
                                    return
                                }


                                console.log(
                                    "Library playlist:",
                                    playlistCard.title,
                                    "| uid:",
                                    playlistCard.uid,
                                    "| kind:",
                                    playlistCard.kind
                                )


                                root.controller
                                    .selectLibraryPlaylist(
                                    playlistCard.index
                                )
                            }
                        }
                    }


                // =====================================================
                // Empty state
                // =====================================================

                Label {
                    anchors.centerIn:
                        parent

                    text:
                        "Плейлистов нет"

                    color:
                        AppTheme.textSecondary

                    font.pixelSize:
                        13

                    visible:
                        root.controller !== null &&
                        root.controller !== undefined &&
                        !root.controller.loadingLibraryPlaylists &&
                        root.controller.libraryPlaylistsModel !== null &&
                        root.controller.libraryPlaylistsModel.count === 0
                }
            }
        }
    }


    // =============================================================
    // Initial load
    // =============================================================

    Component.onCompleted: {
        console.log(
            "========================================"
        )

        console.log(
            "LibraryPage CREATED"
        )

        console.log(
            "controller:",
            root.controller
        )

        if (
            root.controller !== null &&
            root.controller !== undefined
        ) {
            console.log(
                "library model:",
                root.controller.libraryPlaylistsModel
            )

            console.log(
                "library count before load:",
                    root.controller.libraryPlaylistsModel !== null
                    ? root.controller.libraryPlaylistsModel.count
                    : -1
            )

            root.controller.loadLibrary()
        }

        console.log(
            "========================================"
        )
    }
}