import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    signal expandedRequested()

    readonly property bool hasTrack:
        controller !== null &&
        controller !== undefined &&
        (
            controller.currentTrackTitle || ""
        ).length > 0

    readonly property bool loading:
        controller !== null &&
        controller !== undefined &&
        controller.playbackState === 1

    readonly property bool playing:
        controller !== null &&
        controller !== undefined &&
        controller.playing

    readonly property bool paused:
        controller !== null &&
        controller !== undefined &&
        controller.playbackState === 3


    // =============================================================
    // Main background
    // =============================================================

    Rectangle {
        id: panel

        anchors.fill:
            parent

        radius:
            14

        color:
            AppTheme.panel

        border.width:
            1

        border.color:
            AppTheme.borderSubtle
    }


    // =============================================================
    // Artwork
    // =============================================================

    Rectangle {
        id: artworkFrame

        width:
            72

        height:
            72

        anchors.left:
            parent.left

        anchors.leftMargin:
            12

        anchors.verticalCenter:
            parent.verticalCenter

        radius:
            10

        color:
            AppTheme.surface

        clip:
            true


        Image {
            id: artwork

            anchors.fill:
                parent

            source:
                    root.hasTrack &&
                (
                    root.controller.currentTrackCoverUri ||
                    ""
                ).length > 0
                ? "image://yandex/" +
                root.controller.currentTrackCoverUri
                : ""

            sourceSize:
                Qt.size(
                    144,
                    144
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


        Rectangle {
            anchors.fill:
                parent

            color:
                AppTheme.surface

            visible:
                artwork.status !== Image.Ready


            Label {
                anchors.centerIn:
                    parent

                text:
                    root.loading
                        ? "..."
                        : "♪"

                color:
                    AppTheme.textSecondary

                font.pixelSize:
                    root.loading
                        ? 18
                        : 26
            }
        }
    }


    // =============================================================
    // Track information
    // =============================================================

    Item {
        id: infoArea

        anchors.left:
            artworkFrame.right

        anchors.leftMargin:
            14

        anchors.right:
            controlsArea.left

        anchors.rightMargin:
            18

        anchors.top:
            parent.top

        anchors.bottom:
            parent.bottom


        Column {
            anchors.left:
                parent.left

            anchors.right:
                parent.right

            anchors.verticalCenter:
                parent.verticalCenter

            spacing:
                3


            // -----------------------------------------------------
            // Title
            // -----------------------------------------------------

            Label {
                width:
                    parent.width

                text:
                    root.hasTrack
                        ? root.controller.currentTrackTitle
                        : qsTr("Ничего не играет")

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    14

                font.bold:
                    true

                elide:
                    Text.ElideRight
            }


            // -----------------------------------------------------
            // Artist
            // -----------------------------------------------------

            Item {
                width:
                    parent.width

                height:
                    18


                Label {
                    id: artistLabel

                    width:
                        Math.min(
                            implicitWidth,
                            parent.width
                        )

                    height:
                        18

                    text:
                        root.hasTrack
                            ? (
                                root.controller.currentTrackArtist ||
                                ""
                            )
                            : ""

                    color:
                        artistArea.containsMouse
                            ? AppTheme.accent
                            : AppTheme.textSecondary

                    font.pixelSize:
                        11

                    elide:
                        Text.ElideRight
                }


                MouseArea {
                    id: artistArea

                    anchors.fill:
                        artistLabel

                    hoverEnabled:
                        true

                    enabled:
                        root.hasTrack &&
                        (
                            root.controller.currentTrackArtistId ||
                            ""
                        ).length > 0

                    cursorShape:
                        enabled
                            ? Qt.PointingHandCursor
                            : Qt.ArrowCursor

                    onClicked: {
                        var artistId =
                            root.controller.currentTrackArtistId ||
                            ""

                        if (
                            artistId.length === 0
                        ) {
                            return
                        }

                        root.controller.loadArtist(
                            artistId
                        )
                    }
                }
            }


            // -----------------------------------------------------
            // Progress
            // -----------------------------------------------------

            Row {
                width:
                    parent.width

                height:
                    18

                spacing:
                    6


                Label {
                    width:
                        32

                    anchors.verticalCenter:
                        parent.verticalCenter

                    text:
                        root.hasTrack
                            ? root.formatTime(
                                root.controller.position
                            )
                            : "0:00"

                    color:
                        AppTheme.textMuted

                    font.pixelSize:
                        9

                    horizontalAlignment:
                        Text.AlignLeft
                }


                Slider {
                    id: progressSlider

                    width:
                        Math.max(
                            60,
                            parent.width - 76
                        )

                    anchors.verticalCenter:
                        parent.verticalCenter

                    height:
                        18

                    from:
                        0

                    to:
                        Math.max(
                            1,
                                root.controller !== null &&
                                root.controller !== undefined
                                ? root.controller.duration
                                : 0
                        )

                    value:
                        Math.min(
                                root.controller !== null &&
                                root.controller !== undefined
                                ? root.controller.position
                                : 0,
                            to
                        )

                    enabled:
                        root.hasTrack &&
                        root.controller.duration > 0

                    onMoved: {
                        root.controller.seek(
                            value
                        )
                    }
                }


                Label {
                    width:
                        32

                    anchors.verticalCenter:
                        parent.verticalCenter

                    text:
                        root.hasTrack
                            ? root.formatTime(
                                root.controller.duration
                            )
                            : "0:00"

                    color:
                        AppTheme.textMuted

                    font.pixelSize:
                        9

                    horizontalAlignment:
                        Text.AlignRight
                }
            }
        }
    }


    // =============================================================
    // Controls
    // =============================================================

    Row {
        id: controlsArea

        width:
            300

        anchors.right:
            parent.right

        anchors.rightMargin:
            12

        anchors.verticalCenter:
            parent.verticalCenter

        spacing:
            6


        // ---------------------------------------------------------
        // Previous
        // ---------------------------------------------------------

        Button {
            width:
                38

            height:
                38

            text:
                "‹"

            enabled:
                root.hasTrack

            onClicked:
                root.controller.previous()
        }


        // ---------------------------------------------------------
        // Play / Pause
        // ---------------------------------------------------------

        Button {
            id: playButton

            width:
                44

            height:
                38

            text:
                root.loading
                    ? "..."
                    : root.playing
                        ? "Ⅱ"
                        : "▶"

            enabled:
                root.hasTrack &&
                !root.loading

            onClicked: {
                if (
                    root.playing
                ) {
                    root.controller.pause()
                } else {
                    root.controller.play()
                }
            }
        }


        // ---------------------------------------------------------
        // Next
        // ---------------------------------------------------------

        Button {
            width:
                38

            height:
                38

            text:
                "›"

            enabled:
                root.hasTrack

            onClicked:
                root.controller.next()
        }


        // ---------------------------------------------------------
        // Repeat
        // ---------------------------------------------------------

        Button {
            id: repeatButton

            width:
                38

            height:
                38

            text:
                    root.controller.repeatMode === 0
                ? "↻"
                : root.controller.repeatMode === 1
                    ? "↻A"
                    : "↻1"

            enabled:
                root.hasTrack

            opacity:
                    root.controller.repeatMode === 0
                ? 0.65
                : 1.0

            onClicked:
                root.controller.cycleRepeat()
        }


        // ---------------------------------------------------------
        // Shuffle
        // ---------------------------------------------------------

        Button {
            id: shuffleButton

            width:
                38

            height:
                38

            text:
                root.controller.shuffleEnabled
                    ? "🔀"
                    : "⇄"

            enabled:
                root.hasTrack

            opacity:
                root.controller.shuffleEnabled
                    ? 1.0
                    : 0.65

            onClicked:
                root.controller.toggleShuffle()
        }


        // ---------------------------------------------------------
        // Expanded Now Playing
        // ---------------------------------------------------------

        Button {
            width:
                38

            height:
                38

            text:
                "↗"

            enabled:
                root.hasTrack

            onClicked:
                root.expandedRequested()
        }


        // ---------------------------------------------------------
        // Stop
        // ---------------------------------------------------------

        Button {
            width:
                38

            height:
                38

            text:
                "■"

            enabled:
                root.hasTrack

            onClicked:
                root.controller.stop()
        }
    }


    // =============================================================
    // Duration formatter
    // =============================================================

    function formatTime(milliseconds)
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