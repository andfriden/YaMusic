import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

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
    // Main playlist field
    // =============================================================

    Rectangle {
        anchors.left:
            parent.left

        anchors.right:
            parent.right

        anchors.top:
            parent.top

        anchors.bottom:
            parent.bottom

        radius:
            10

        color:
            AppTheme.panel

        border.width:
            1

        border.color:
            AppTheme.borderSubtle


        // =========================================================
        // Content
        // =========================================================

        Column {
            anchors.fill:
                parent

            anchors.margins:
                12

            spacing:
                12


            // =====================================================
            // Header
            // =====================================================

            Row {
                width:
                    parent.width

                spacing:
                    10


                Label {
                    text:
                        root.controller &&
                        root.controller.currentPlaylistTitle
                            ? root.controller.currentPlaylistTitle
                            : qsTr("Плейлист")

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        18

                    font.bold:
                        true

                    anchors.verticalCenter:
                        parent.verticalCenter
                }


                Label {
                    text:
                            root.controller &&
                        root.controller.currentPlaylistTrackCount > 0
                        ? root.controller.currentPlaylistTrackCount +
                            qsTr(" треков")
                        : ""

                    color:
                        AppTheme.textSecondary

                    anchors.verticalCenter:
                        parent.verticalCenter
                }
            }


            // =====================================================
            // Tracks
            // =====================================================

            ListView {
                id: tracksView

                width:
                    parent.width

                height:
                    parent.height - 45

                clip:
                    true

                spacing:
                    6

                model:
                    root.controller &&
                    root.controller.playlistModel
                        ? root.controller.playlistModel
                        : null


                ScrollBar.vertical:
                    ScrollBar {
                        policy:
                            ScrollBar.AsNeeded
                    }


                // =================================================
                // Track delegate
                // =================================================

                delegate:
                    Rectangle {
                        id: trackDelegate

                        width:
                            tracksView.width -
                            (
                                tracksView.ScrollBar.vertical.visible
                                    ? 10
                                    : 0
                            )

                        height:
                            68

                        radius:
                            8

                        color:
                            rowMouseArea.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary

                        border.width:
                            1

                        border.color:
                            AppTheme.borderSubtle


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


                        // =============================================
                        // Row click
                        // =============================================

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


                        // =============================================
                        // Artwork
                        // =============================================

                        Rectangle {
                            id: coverContainer

                            width:
                                52

                            height:
                                52

                            anchors.left:
                                parent.left

                            anchors.leftMargin:
                                8

                            anchors.verticalCenter:
                                parent.verticalCenter

                            radius:
                                6

                            color:
                                AppTheme.artworkPlaceholder

                            clip:
                                true


                            Image {
                                id: cover

                                anchors.fill:
                                    parent

                                source:
                                        trackDelegate.coverUri.length > 0
                                    ? "image://yandex/" +
                                        trackDelegate.coverUri
                                    : ""

                                sourceSize:
                                    Qt.size(
                                        104,
                                        104
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
                                    24

                                visible:
                                    cover.status !== Image.Ready
                            }
                        }


                        // =============================================
                        // Track information
                        // =============================================

                        Column {
                            id: trackInfo

                            anchors.left:
                                coverContainer.right

                            anchors.leftMargin:
                                12

                            anchors.right:
                                durationLabel.left

                            anchors.rightMargin:
                                12

                            anchors.verticalCenter:
                                parent.verticalCenter

                            spacing:
                                2


                            // =========================================
                            // Title
                            // =========================================

                            Label {
                                width:
                                    parent.width

                                height:
                                    18

                                text:
                                        trackDelegate.title.length > 0
                                    ? trackDelegate.title
                                    : qsTr("Без названия")

                                color:
                                    AppTheme.textPrimary

                                font.pixelSize:
                                    14

                                font.bold:
                                    true

                                elide:
                                    Text.ElideRight
                            }


                            // =========================================
                            // Artist
                            // =========================================

                            Item {
                                id: artistArea

                                width:
                                    artistLabel.width

                                height:
                                    artistLabel.height


                                Label {
                                    id: artistLabel

                                    text:
                                        trackDelegate.artist

                                    color:
                                        artistMouseArea.containsMouse
                                            ? AppTheme.accent
                                            : AppTheme.textSecondary

                                    font.pixelSize:
                                        12

                                    width:
                                        Math.min(
                                            implicitWidth,
                                            trackInfo.width
                                        )

                                    height:
                                        18

                                    elide:
                                        Text.ElideRight
                                }


                                MouseArea {
                                    id: artistMouseArea

                                    anchors.fill:
                                        artistLabel

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


                            // =========================================
                            // Album
                            // =========================================

                            Item {
                                id: albumArea

                                width:
                                    albumLabel.width

                                height:
                                    albumLabel.height


                                Label {
                                    id: albumLabel

                                    text:
                                        trackDelegate.album

                                    color:
                                        albumMouseArea.containsMouse
                                            ? AppTheme.accent
                                            : AppTheme.textMuted

                                    font.pixelSize:
                                        10

                                    width:
                                        Math.min(
                                            implicitWidth,
                                            trackInfo.width
                                        )

                                    height:
                                        16

                                    elide:
                                        Text.ElideRight
                                }


                                MouseArea {
                                    id: albumMouseArea

                                    anchors.fill:
                                        albumLabel

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


                        // =============================================
                        // Duration
                        // =============================================

                        Label {
                            id: durationLabel

                            anchors.right:
                                likeButton.left

                            anchors.rightMargin:
                                6

                            anchors.verticalCenter:
                                parent.verticalCenter

                            text:
                                root.formatDuration(
                                    trackDelegate.durationMs
                                )

                            color:
                                AppTheme.textSecondary

                            font.pixelSize:
                                11
                        }


                        // =============================================
                        // Like button
                        // =============================================

                        Item {
                            id: likeButton

                            width:
                                36

                            height:
                                36

                            anchors.right:
                                parent.right

                            anchors.rightMargin:
                                8

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


                // =====================================================
                // Empty / loading state
                // =====================================================

                Label {
                    anchors.centerIn:
                        parent

                    text:
                            root.controller &&
                        root.controller.loadingPlaylist
                        ? qsTr("Загрузка плейлиста...")
                        : qsTr("В плейлисте нет треков")

                    color:
                        AppTheme.textSecondary

                    font.pixelSize:
                        13

                    visible:
                        root.controller &&
                        (
                            root.controller.loadingPlaylist ||
                            (
                                root.controller.playlistModel !== null &&
                                root.controller.playlistModel.count === 0
                            )
                        )
                }
            }
        }
    }


    // =============================================================
    // Helpers
    // =============================================================

    function formatDuration(milliseconds)
    {
        if (
            !milliseconds ||
            milliseconds <= 0
        ) {
            return "0:00"
        }


        var totalSeconds =
            Math.floor(
                milliseconds / 1000
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