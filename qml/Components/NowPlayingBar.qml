import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../Theme"

Rectangle {
    id: root

    property var controller

    signal expandedRequested()

    readonly property bool hasTrack:
        root.controller !== null &&
        root.controller !== undefined &&
        String(root.controller.currentTrackTitle || "").length > 0

    readonly property bool loading:
        root.controller !== null &&
        root.controller !== undefined &&
        root.controller.playbackState === 1

    readonly property bool playing:
        root.controller !== null &&
        root.controller !== undefined &&
        root.controller.playing

    readonly property bool hasPlayerAccent:
        root.hasTrack &&
        root.controller !== null &&
        root.controller !== undefined &&
        root.controller.playerAccent !== undefined &&
        root.controller.playerAccent !== null &&
        root.controller.playerAccent.valid

    readonly property color playerAccent:
        root.hasPlayerAccent
            ? root.controller.playerAccent
            : AppTheme.accent

    height: 124

    color:
        root.hasPlayerAccent
            ? Qt.rgba(
                root.playerAccent.r,
                root.playerAccent.g,
                root.playerAccent.b,
                0.09
            )
            : AppTheme.panel

    border.color:
        root.hasPlayerAccent
            ? Qt.rgba(
                root.playerAccent.r,
                root.playerAccent.g,
                root.playerAccent.b,
                0.14
            )
            : AppTheme.border

    border.width: 1
    radius: 12

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 18
        anchors.rightMargin: 18
        anchors.topMargin: 12
        anchors.bottomMargin: 12

        spacing: 0

        // =========================================================
        // Track information
        // =========================================================

        RowLayout {
            id: trackInfo

            Layout.fillWidth: true
            Layout.minimumWidth: 250
            Layout.preferredWidth: 360
            Layout.maximumWidth: 430

            spacing: 12

            Image {
                id: cover

                Layout.preferredWidth: 58
                Layout.preferredHeight: 58

                source:
                        root.hasTrack &&
                    root.controller.currentTrackCoverUri
                    ? "image://yandex/" +
                    root.controller.currentTrackCoverUri
                    : ""

                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: true

                Rectangle {
                    anchors.fill: parent

                    radius: 9

                    color: "transparent"

                    border.color:
                        AppTheme.borderSubtle

                    border.width: 1
                }
            }

            ColumnLayout {
                Layout.fillWidth: true

                spacing: 3

                Label {
                    Layout.fillWidth: true

                    text:
                        root.hasTrack
                            ? root.controller.currentTrackTitle
                            : "Нет воспроизводимого трека"

                    color:
                        AppTheme.textPrimary

                    font.pixelSize: 14
                    font.weight: Font.DemiBold

                    elide:
                        Text.ElideRight

                    maximumLineCount: 1
                }

                Label {
                    Layout.fillWidth: true

                    text:
                        root.hasTrack
                            ? root.controller.currentTrackArtist
                            : ""

                    color:
                        AppTheme.textSecondary

                    font.pixelSize: 12

                    elide:
                        Text.ElideRight

                    maximumLineCount: 1

                    MouseArea {
                        anchors.fill: parent

                        enabled:
                            root.hasTrack &&
                            String(
                                root.controller.currentTrackArtistId ||
                                ""
                            ).length > 0

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked: {
                            if (!root.controller)
                                return

                            root.controller.loadArtist(
                                root.controller.currentTrackArtistId
                            )
                        }
                    }
                }
            }

            // =====================================================
            // Like
            // =====================================================

            Item {
                id: likeButton

                Layout.preferredWidth: 36
                Layout.preferredHeight: 36

                visible:
                    root.hasTrack

                Rectangle {
                    anchors.fill: parent

                    radius: 18

                    color:
                        likeMouseArea.containsMouse
                            ? AppTheme.panelHover
                            : "transparent"

                    opacity:
                        likeMouseArea.containsMouse
                            ? 1
                            : 0

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 100
                        }
                    }
                }

                Label {
                    anchors.centerIn: parent

                    text:
                            root.controller &&
                        root.controller.currentTrackLiked
                        ? "♥"
                        : "♡"

                    color:
                            root.controller &&
                        root.controller.currentTrackLiked
                        ? AppTheme.accent
                        : AppTheme.textSecondary

                    font.pixelSize: 21

                    Behavior on color {
                        ColorAnimation {
                            duration: 120
                        }
                    }
                }

                MouseArea {
                    id: likeMouseArea

                    anchors.fill: parent

                    enabled:
                        root.hasTrack &&
                        root.controller !== null &&
                        root.controller !== undefined &&
                        String(
                            root.controller.currentTrackId || ""
                        ).length > 0

                    cursorShape:
                        enabled
                            ? Qt.PointingHandCursor
                            : Qt.ArrowCursor

                    onClicked: {
                        if (!root.controller)
                            return

                        const trackId =
                            String(
                                root.controller.currentTrackId ||
                                ""
                            )

                        if (trackId.length === 0)
                            return

                        root.controller.toggleLike(
                            trackId,
                            root.controller.currentTrackLiked
                        )
                    }
                }
            }
        }

        Item {
            Layout.preferredWidth: 18
        }

        // =========================================================
        // Center controls
        // =========================================================

        ColumnLayout {
            id: centerControls

            Layout.alignment:
                Qt.AlignVCenter

            Layout.preferredWidth:
                470

            spacing: 7

            RowLayout {
                Layout.alignment:
                    Qt.AlignHCenter

                spacing: 8

                // -------------------------------------------------
                // Shuffle
                // -------------------------------------------------

                Item {
                    width: 38
                    height: 38

                    Rectangle {
                        anchors.fill: parent

                        radius: 19

                        color:
                            shuffleMouseArea.containsMouse
                                ? AppTheme.panelHover
                                : "transparent"
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "⤨"

                        color:
                                root.controller &&
                            root.controller.shuffleEnabled
                            ? root.playerAccent
                            : AppTheme.textSecondary

                        font.pixelSize: 20
                    }

                    MouseArea {
                        id: shuffleMouseArea

                        anchors.fill: parent

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller)
                                root.controller.toggleShuffle()
                        }
                    }
                }

                // -------------------------------------------------
                // Previous
                // -------------------------------------------------

                Item {
                    width: 42
                    height: 42

                    Rectangle {
                        anchors.fill: parent

                        radius: 21

                        color:
                            previousMouseArea.containsMouse
                                ? AppTheme.panelHover
                                : "transparent"
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "‹"

                        color:
                            AppTheme.textPrimary

                        font.pixelSize: 32
                        font.weight: Font.Light
                    }

                    MouseArea {
                        id: previousMouseArea

                        anchors.fill: parent

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller)
                                root.controller.previous()
                        }
                    }
                }

                // -------------------------------------------------
                // Play / Pause
                // -------------------------------------------------

                Item {
                    width: 50
                    height: 50

                    Rectangle {
                        anchors.fill: parent

                        radius: 25

                        color:
                            root.playerAccent
                    }

                    Label {
                        anchors.centerIn: parent

                        text:
                            root.loading
                                ? "…"
                                : root.playing
                                    ? "Ⅱ"
                                    : "▶"

                        color:
                            AppTheme.textPrimary

                        font.pixelSize:
                            root.playing
                                ? 20
                                : 19

                        font.weight:
                            Font.DemiBold
                    }

                    MouseArea {
                        id: playMouseArea

                        anchors.fill: parent

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked: {
                            if (!root.controller)
                                return

                            if (root.loading)
                                return

                            if (root.playing)
                                root.controller.pause()
                            else
                                root.controller.play()
                        }
                    }
                }

                // -------------------------------------------------
                // Next
                // -------------------------------------------------

                Item {
                    width: 42
                    height: 42

                    Rectangle {
                        anchors.fill: parent

                        radius: 21

                        color:
                            nextMouseArea.containsMouse
                                ? AppTheme.panelHover
                                : "transparent"
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "›"

                        color:
                            AppTheme.textPrimary

                        font.pixelSize: 32
                        font.weight: Font.Light
                    }

                    MouseArea {
                        id: nextMouseArea

                        anchors.fill: parent

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller)
                                root.controller.next()
                        }
                    }
                }

                // -------------------------------------------------
                // Repeat
                // -------------------------------------------------

                Item {
                    width: 38
                    height: 38

                    Rectangle {
                        anchors.fill: parent

                        radius: 19

                        color:
                            repeatMouseArea.containsMouse
                                ? AppTheme.panelHover
                                : "transparent"
                    }

                    Label {
                        anchors.centerIn: parent

                        text:
                                root.controller &&
                            root.controller.repeatMode === 1
                            ? "↻1"
                            : "↻"

                        color:
                                root.controller &&
                            root.controller.repeatMode !== 0
                            ? root.playerAccent
                            : AppTheme.textSecondary

                        font.pixelSize: 18
                    }

                    MouseArea {
                        id: repeatMouseArea

                        anchors.fill: parent

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller)
                                root.controller.cycleRepeat()
                        }
                    }
                }
            }

            // =====================================================
            // Progress
            // =====================================================

            RowLayout {
                Layout.fillWidth: true

                spacing: 8

                Label {
                    Layout.preferredWidth: 38

                    text:
                        root.formatTime(
                            root.controller
                                ? root.controller.position
                                : 0
                        )

                    color:
                        AppTheme.textMuted

                    font.pixelSize: 10

                    horizontalAlignment:
                        Text.AlignRight
                }

                Item {
                    id: progressArea

                    Layout.fillWidth: true
                    Layout.preferredHeight: 12

                    Rectangle {
                        anchors.verticalCenter:
                            parent.verticalCenter

                        anchors.left:
                            parent.left

                        anchors.right:
                            parent.right

                        height: 4
                        radius: 2

                        color:
                            AppTheme.panelSecondary

                        Rectangle {
                            width:
                                    root.controller &&
                                root.controller.duration > 0
                                ? parent.width *
                                Math.max(
                                    0,
                                    Math.min(
                                        1,
                                        root.controller.position /
                                        root.controller.duration
                                    )
                                )
                                : 0

                            height:
                                parent.height

                            radius: 2

                            color:
                                root.playerAccent
                        }
                    }

                    MouseArea {
                        anchors.fill: parent

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked: function(mouse) {
                            if (!root.controller)
                                return

                            if (
                                root.controller.duration <= 0
                            ) {
                                return
                            }

                            const ratio =
                                Math.max(
                                    0,
                                    Math.min(
                                        1,
                                        mouse.x / width
                                    )
                                )

                            root.controller.seek(
                                Math.round(
                                    root.controller.duration *
                                    ratio
                                )
                            )
                        }
                    }
                }

                Label {
                    Layout.preferredWidth: 38

                    text:
                        root.formatTime(
                            root.controller
                                ? root.controller.duration
                                : 0
                        )

                    color:
                        AppTheme.textMuted

                    font.pixelSize: 10

                    horizontalAlignment:
                        Text.AlignLeft
                }
            }
        }

        Item {
            Layout.preferredWidth: 18
        }

        // =========================================================
        // Right controls
        // =========================================================

        RowLayout {
            Layout.fillWidth: true

            Layout.minimumWidth: 250
            Layout.preferredWidth: 360
            Layout.maximumWidth: 430

            spacing: 8

            Item {
                Layout.fillWidth: true
            }

            // -----------------------------------------------------
            // Volume button
            // -----------------------------------------------------

            Item {
                width: 38
                height: 38

                Rectangle {
                    anchors.fill: parent

                    radius: 19

                    color:
                        volumeMouseArea.containsMouse
                            ? AppTheme.panelHover
                            : "transparent"
                }

                Label {
                    anchors.centerIn: parent

                    text:
                            root.controller &&
                        root.controller.volume > 0
                        ? "◖"
                        : "×"

                    color:
                        AppTheme.textSecondary

                    font.pixelSize: 19
                }

                MouseArea {
                    id: volumeMouseArea

                    anchors.fill: parent

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked: {
                        if (!root.controller)
                            return

                        if (
                            root.controller.volume > 0
                        ) {
                            root.controller.setVolume(0)
                        } else {
                            root.controller.setVolume(1)
                        }
                    }
                }
            }

            // -----------------------------------------------------
            // Volume slider
            // -----------------------------------------------------

            Slider {
                id: volumeSlider

                Layout.preferredWidth: 105
                Layout.maximumWidth: 105

                from: 0
                to: 1

                value:
                    root.controller
                        ? root.controller.volume
                        : 0

                onMoved: {
                    if (root.controller)
                        root.controller.setVolume(value)
                }

                background: Rectangle {
                    x:
                        volumeSlider.leftPadding

                    y:
                        volumeSlider.topPadding +
                        volumeSlider.availableHeight / 2 -
                        height / 2

                    width:
                        volumeSlider.availableWidth

                    height: 4
                    radius: 2

                    color:
                        AppTheme.panelSecondary

                    Rectangle {
                        width:
                            volumeSlider.visualPosition *
                            parent.width

                        height:
                            parent.height

                        radius: 2

                        color:
                            root.playerAccent
                    }
                }

                handle: Rectangle {
                    x:
                        volumeSlider.leftPadding +
                        volumeSlider.visualPosition *
                        (
                            volumeSlider.availableWidth -
                            width
                        )

                    y:
                        volumeSlider.topPadding +
                        volumeSlider.availableHeight / 2 -
                        height / 2

                    width: 10
                    height: 10

                    radius: 5

                    color:
                        root.playerAccent
                }
            }

            // -----------------------------------------------------
            // Expand
            // -----------------------------------------------------

            Item {
                width: 38
                height: 38

                Rectangle {
                    anchors.fill: parent

                    radius: 19

                    color:
                        expandMouseArea.containsMouse
                            ? AppTheme.panelHover
                            : "transparent"
                }

                Label {
                    anchors.centerIn: parent

                    text: "↗"

                    color:
                        AppTheme.textSecondary

                    font.pixelSize: 18
                }

                MouseArea {
                    id: expandMouseArea

                    anchors.fill: parent

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked: {
                        root.expandedRequested()
                    }
                }
            }
        }
    }

    // =============================================================
    // Helpers
    // =============================================================

    function formatTime(ms) {
        if (!ms || ms < 0)
            return "0:00"

        const totalSeconds =
            Math.floor(ms / 1000)

        const minutes =
            Math.floor(totalSeconds / 60)

        const seconds =
            totalSeconds % 60

        return minutes +
            ":" +
            (seconds < 10 ? "0" : "") +
            seconds
    }
}