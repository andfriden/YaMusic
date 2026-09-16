import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import YaMusic 1.0

Item {
    id: root

    property var controller


    width:
        parent
            ? parent.width
            : 0

    height:
        parent
            ? parent.height
            : 700

    implicitWidth:
        width

    implicitHeight:
        height


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


                Row {
                    width:
                        parent.width

                    spacing:
                        10

                    Label {
                        width:
                            parent.width -
                            deleteButton.width -
                            parent.spacing

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

                        anchors.verticalCenter:
                            parent.verticalCenter
                    }


                    Rectangle {
                        id: deleteButton

                        width:
                            32

                        height:
                            32

                        radius:
                            7

                        color:
                            deleteMouse.containsMouse
                                ? AppTheme.panelHover
                                : AppTheme.panel

                        border.width:
                            1

                        border.color:
                            AppTheme.borderSubtle

                        visible:
                            root.controller &&
                            root.controller.currentPlaylistKind > 0

                        Label {
                            anchors.centerIn:
                                parent

                            text:
                                "🗑"

                            color:
                                deleteMouse.containsMouse
                                    ? AppTheme.accent
                                    : AppTheme.textSecondary

                            font.pixelSize:
                                14
                        }

                        MouseArea {
                            id: deleteMouse

                            anchors.fill:
                                parent

                            hoverEnabled:
                                true

                            cursorShape:
                                Qt.PointingHandCursor

                            onClicked: {
                                if (!root.controller)
                                    return

                                deleteConfirmDialog.open()
                            }
                        }
                    }
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


            // Delete confirmation dialog
            Dialog {
                id: deleteConfirmDialog

                title: "Удалить плейлист?"
                standardButtons: Dialog.Yes | Dialog.No

                anchors.centerIn:
                    parent

                onAccepted: {
                    if (root.controller) {
                        root.controller.deleteCurrentPlaylist()
                    }
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