import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root


    // =============================================================
    // Controller
    // =============================================================

    property var controller


    // =============================================================
    // Background
    // =============================================================

    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.background
    }


    // =============================================================
    // Header
    // =============================================================

    Label {
        id: titleLabel

        anchors.left:
            parent.left

        anchors.leftMargin:
            24

        anchors.right:
            parent.right

        anchors.rightMargin:
            24

        anchors.top:
            parent.top

        anchors.topMargin:
            24

        text:
            root.controller
                ? root.controller.currentPlaylistTitle
                : ""

        color:
            AppTheme.textPrimary

        font.pixelSize:
            26

        font.weight:
            Font.DemiBold

        elide:
            Text.ElideRight
    }


    // =============================================================
    // Tracks
    // =============================================================

    ListView {
        id: tracksView

        anchors.left:
            parent.left

        anchors.leftMargin:
            16

        anchors.right:
            parent.right

        anchors.rightMargin:
            16

        anchors.top:
            titleLabel.bottom

        anchors.topMargin:
            20

        anchors.bottom:
            parent.bottom

        anchors.bottomMargin:
            16

        clip:
            true

        spacing:
            2

        model:
            root.controller
                ? root.controller.playlistModel
                : null


        delegate:
            Item {
                id: trackDelegate

                width:
                    tracksView.width

                height:
                    66


                required property int index
                required property string trackId
                required property string title
                required property string artist
                required property string artistId
                required property string album
                required property string albumId
                required property string coverUri
                required property int durationMs
                required property bool liked


                // =================================================
                // Hover background
                // =================================================

                Rectangle {
                    anchors.fill:
                        parent

                    radius:
                        8

                    color:
                        rowMouseArea.containsMouse
                            ? AppTheme.panelActive
                            : "transparent"
                }


                // =================================================
                // Row click
                // =================================================

                MouseArea {
                    id: rowMouseArea

                    anchors.fill:
                        parent

                    hoverEnabled:
                        true

                    cursorShape:
                        Qt.PointingHandCursor

                    z:
                        0

                    onClicked: {
                        if (!root.controller)
                            return

                        root.controller.selectPlaylistTrack(
                            trackDelegate.index
                        )
                    }
                }


                // =================================================
                // Artwork
                // =================================================

                Rectangle {
                    id: artworkContainer

                    width:
                        48

                    height:
                        48

                    anchors.left:
                        parent.left

                    anchors.leftMargin:
                        8

                    anchors.verticalCenter:
                        parent.verticalCenter

                    radius:
                        6

                    color:
                        AppTheme.panel

                    clip:
                        true


                    Image {
                        anchors.fill:
                            parent

                        source:
                                trackDelegate.coverUri.length > 0
                            ? "image://yandex/" +
                                trackDelegate.coverUri
                            : ""

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous:
                            true

                        cache:
                            true

                        smooth:
                            true
                    }
                }


                // =================================================
                // Track information
                // =================================================

                Column {
                    id: trackInfo

                    anchors.left:
                        artworkContainer.right

                    anchors.leftMargin:
                        12

                    anchors.right:
                        durationLabel.left

                    anchors.rightMargin:
                        10

                    anchors.verticalCenter:
                        parent.verticalCenter

                    spacing:
                        0


                    // =================================================
                    // Title
                    // =================================================

                    Label {
                        width:
                            parent.width

                        height:
                            18

                        text:
                            trackDelegate.title

                        color:
                            AppTheme.textPrimary

                        font.pixelSize:
                            14

                        font.weight:
                            Font.DemiBold

                        elide:
                            Text.ElideRight
                    }


                    // =================================================
                    // Artist
                    // =================================================

                    Item {
                        id: artistItem

                        width:
                            parent.width

                        height:
                            18


                        Label {
                            id: artistLabel

                            anchors.fill:
                                parent

                            text:
                                trackDelegate.artist

                            color:
                                artistMouseArea.containsMouse
                                    ? AppTheme.accent
                                    : AppTheme.textSecondary

                            font.pixelSize:
                                12

                            elide:
                                Text.ElideRight
                        }


                        MouseArea {
                            id: artistMouseArea

                            anchors.fill:
                                parent

                            hoverEnabled:
                                true

                            enabled:
                                trackDelegate.artistId.length > 0

                            cursorShape:
                                enabled
                                    ? Qt.PointingHandCursor
                                    : Qt.ArrowCursor

                            z:
                                10

                            onClicked: {
                                if (!root.controller)
                                    return

                                root.controller.loadArtist(
                                    trackDelegate.artistId
                                )
                            }
                        }
                    }


                    // =================================================
                    // Album
                    // =================================================

                    Item {
                        id: albumItem

                        width:
                            parent.width

                        height:
                            trackDelegate.album.length > 0
                                ? 16
                                : 0


                        Label {
                            id: albumLabel

                            anchors.fill:
                                parent

                            text:
                                trackDelegate.album

                            color:
                                albumMouseArea.containsMouse
                                    ? AppTheme.accent
                                    : AppTheme.textSecondary

                            font.pixelSize:
                                10

                            elide:
                                Text.ElideRight
                        }


                        MouseArea {
                            id: albumMouseArea

                            anchors.fill:
                                parent

                            hoverEnabled:
                                true

                            enabled:
                                trackDelegate.albumId.length > 0

                            cursorShape:
                                enabled
                                    ? Qt.PointingHandCursor
                                    : Qt.ArrowCursor

                            z:
                                10

                            onClicked: {
                                if (!root.controller)
                                    return

                                root.controller.loadAlbum(
                                    trackDelegate.albumId
                                )
                            }
                        }
                    }
                }


                // =================================================
                // Duration
                // =================================================

                Label {
                    id: durationLabel

                    anchors.right:
                        likeButton.left

                    anchors.rightMargin:
                        8

                    anchors.verticalCenter:
                        parent.verticalCenter

                    text:
                        formatDuration(
                            trackDelegate.durationMs
                        )

                    color:
                        AppTheme.textSecondary

                    font.pixelSize:
                        12
                }


                // =================================================
                // Like button
                // =================================================

                Item {
                    id: likeButton

                    width:
                        34

                    height:
                        34

                    anchors.right:
                        parent.right

                    anchors.rightMargin:
                        6

                    anchors.verticalCenter:
                        parent.verticalCenter

                    z:
                        20


                    Label {
                        anchors.centerIn:
                            parent

                        text:
                            trackDelegate.liked
                                ? "♥"
                                : "♡"

                        color:
                            trackDelegate.liked
                                ? AppTheme.accent
                                : AppTheme.textSecondary

                        font.pixelSize:
                            21
                    }


                    MouseArea {
                        anchors.fill:
                            parent

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked: {
                            if (!root.controller)
                                return

                            root.controller.toggleLike(
                                trackDelegate.trackId,
                                trackDelegate.liked
                            )
                        }
                    }
                }
            }
    }


    // =============================================================
    // Helpers
    // =============================================================

    function formatDuration(durationMs)
    {
        if (
            durationMs <= 0
        )
        {
            return ""
        }


        var totalSeconds =
            Math.floor(
                durationMs / 1000
            )


        var minutes =
            Math.floor(
                totalSeconds / 60
            )


        var seconds =
            totalSeconds % 60


        return minutes +
            ":" +
            (
                seconds < 10
                    ? "0"
                    : ""
            ) +
            seconds
    }
}