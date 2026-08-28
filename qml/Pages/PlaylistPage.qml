import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller


    readonly property int pageHeight:
        700


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
            20


        // =========================================================
        // Playlist header
        // =========================================================

        Row {
            width:
                parent.width

            height:
                180

            spacing:
                20


            // -----------------------------------------------------
            // Artwork
            // -----------------------------------------------------

            Rectangle {
                width:
                    180

                height:
                    180

                radius:
                    12

                color:
                    AppTheme.panelSecondary

                clip:
                    true


                Image {
                    id: playlistArtwork

                    anchors.fill:
                        parent

                    source:
                            root.controller !== null &&
                        root.controller !== undefined &&
                        root.controller.currentPlaylistCoverUri.length > 0
                        ? "image://yandex/" +
                        root.controller.currentPlaylistCoverUri
                        : ""

                    sourceSize:
                        Qt.size(
                            360,
                            360
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
                        48

                    visible:
                        playlistArtwork.status !==
                        Image.Ready
                }
            }


            // -----------------------------------------------------
            // Information
            // -----------------------------------------------------

            Column {
                anchors.verticalCenter:
                    parent.verticalCenter

                width:
                    parent.width -
                    200

                spacing:
                    8


                Label {
                    width:
                        parent.width

                    text:
                            root.controller !== null &&
                        root.controller !== undefined &&
                        root.controller.currentPlaylistTitle.length > 0
                        ? root.controller.currentPlaylistTitle
                        : "Плейлист"

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        28

                    font.bold:
                        true

                    elide:
                        Text.ElideRight
                }


                Label {
                    width:
                        parent.width

                    text:
                            root.controller !== null &&
                        root.controller !== undefined &&
                        root.controller.loadingPlaylist
                        ? "Загрузка..."
                        : ""

                    color:
                        AppTheme.textSecondary

                    font.pixelSize:
                        13
                }
            }
        }


        // =========================================================
        // Tracks
        // =========================================================

        PlaylistView {
            id: playlistView

            width:
                parent.width

            height:
                Math.max(
                    0,
                    parent.height - 200
                )

            controller:
                root.controller

            visible:
                root.controller !== null &&
                root.controller !== undefined &&
                !root.controller.loadingPlaylist
        }
    }
}