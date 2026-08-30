import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller: null
    property bool compactMode: false

    // =============================================================
    // State
    // =============================================================

    readonly property bool hasController:
        root.controller !== null &&
        root.controller !== undefined

    readonly property var waveModel:
        root.hasController
            ? root.controller.myWaveModel
            : null

    readonly property int trackCount:
            root.waveModel !== null &&
        root.waveModel !== undefined
        ? Number(root.waveModel.count)
        : 0

    readonly property bool hasTracks:
        root.trackCount > 0


    // =============================================================
    // Layout
    // =============================================================

    readonly property int horizontalMargin:
        root.compactMode ? 12 : 20

    readonly property int verticalMargin:
        root.compactMode ? 12 : 20

    readonly property int contentSpacing:
        root.compactMode ? 10 : 14

    readonly property int headerHeight:
        root.compactMode ? 28 : 38

    readonly property int trackHeight:
        root.compactMode ? 58 : 68

    readonly property int trackSpacing:
        6

    readonly property int maxVisibleTracks:
        root.compactMode ? 3 : 8

    readonly property int visibleTrackCount:
        Math.min(
            root.trackCount,
            root.maxVisibleTracks
        )

    readonly property int listHeight:
            root.visibleTrackCount > 0
        ? (
            root.visibleTrackCount *
            root.trackHeight
        ) +
        (
            Math.max(
                0,
                root.visibleTrackCount - 1
            ) *
            root.trackSpacing
        )
        : 0

    readonly property int contentHeight:
        root.hasTracks
            ? root.headerHeight +
            root.contentSpacing +
            root.listHeight
            : root.headerHeight

    implicitHeight:
        root.hasTracks
            ? root.verticalMargin * 2 +
            root.contentHeight
            : 0


    // =============================================================
    // Background
    // =============================================================

    Rectangle {
        anchors.fill:
            parent

        visible:
            root.hasTracks

        radius:
            root.compactMode ? 10 : 14

        color:
            AppTheme.panel

        border.width:
            1

        border.color:
            AppTheme.borderSubtle
    }


    // =============================================================
    // Content
    // =============================================================

    Column {
        id: contentColumn

        x:
            root.horizontalMargin

        y:
            root.verticalMargin

        width:
            Math.max(
                0,
                root.width -
                root.horizontalMargin * 2
            )

        spacing:
            root.contentSpacing


        // =========================================================
        // Header
        // =========================================================

        Row {
            width:
                parent.width

            height:
                root.headerHeight

            spacing:
                8

            Label {
                width:
                    parent.width

                height:
                    parent.height

                text:
                    qsTr("Моя волна")

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    root.compactMode ? 18 : 26

                font.bold:
                    true

                verticalAlignment:
                    Text.AlignVCenter

                elide:
                    Text.ElideRight

                maximumLineCount:
                    1
            }
        }


        // =========================================================
        // Track list
        // =========================================================

        ListView {
            id: trackList

            width:
                parent.width

            height:
                root.listHeight

            model:
                root.waveModel

            clip:
                true

            spacing:
                root.trackSpacing

            boundsBehavior:
                Flickable.StopAtBounds

            interactive:
                root.trackCount > root.maxVisibleTracks


            ScrollBar.vertical:
                ScrollBar {
                    policy:
                            root.trackCount > root.maxVisibleTracks
                        ? ScrollBar.AsNeeded
                        : ScrollBar.AlwaysOff
                }


            // =====================================================
            // Track delegate
            // =====================================================

            delegate:
                Rectangle {
                    id: trackDelegate

                    required property int index
                    required property string title
                    required property string artist
                    required property string artistId
                    required property string album
                    required property string albumId
                    required property string coverUri
                    required property int durationMs

                    width:
                        trackList.width -
                        (
                            trackList.ScrollBar.vertical.visible
                                ? 10
                                : 0
                        )

                    height:
                        root.trackHeight

                    radius:
                        root.compactMode ? 8 : 9

                    color:
                        root.isCurrentTrack(
                            trackDelegate.title,
                            trackDelegate.artist
                        )
                            ? AppTheme.panelActive
                            : (
                                rowMouseArea.containsMouse
                                    ? AppTheme.panelHover
                                    : AppTheme.panelSecondary
                            )

                    border.width:
                        1

                    border.color:
                        root.isCurrentTrack(
                            trackDelegate.title,
                            trackDelegate.artist
                        )
                            ? AppTheme.border
                            : AppTheme.borderSubtle


                    Behavior on color {
                        ColorAnimation {
                            duration:
                                120

                            easing.type:
                                Easing.OutCubic
                        }
                    }


                    // =================================================
                    // Artwork
                    // =================================================

                    Rectangle {
                        id: coverContainer

                        width:
                            root.compactMode ? 44 : 52

                        height:
                            width

                        anchors.left:
                            parent.left

                        anchors.leftMargin:
                            root.compactMode ? 7 : 9

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
                                root.compactMode ? 18 : 20

                            visible:
                                cover.status !== Image.Ready
                        }
                    }


                    // =================================================
                    // Track information
                    // =================================================

                    Column {
                        id: trackInfo

                        anchors.left:
                            coverContainer.right

                        anchors.right:
                            durationLabel.left

                        anchors.leftMargin:
                            12

                        anchors.rightMargin:
                            8

                        anchors.verticalCenter:
                            parent.verticalCenter

                        spacing:
                            2


                        Label {
                            width:
                                parent.width

                            height:
                                root.compactMode ? 18 : 20

                            text:
                                    trackDelegate.title.length > 0
                                ? trackDelegate.title
                                : qsTr("Без названия")

                            color:
                                AppTheme.textPrimary

                            font.pixelSize:
                                root.compactMode ? 13 : 14

                            font.bold:
                                true

                            elide:
                                Text.ElideRight

                            maximumLineCount:
                                1
                        }


                        EntityLink {
                            width:
                                parent.width

                            text:
                                trackDelegate.artist

                            entityId:
                                trackDelegate.artistId

                            entityType:
                                "artist"

                            controller:
                                root.controller
                        }


                        EntityLink {
                            width:
                                parent.width

                            visible:
                                !root.compactMode &&
                                trackDelegate.album.length > 0

                            text:
                                trackDelegate.album

                            entityId:
                                trackDelegate.albumId

                            entityType:
                                "album"

                            controller:
                                root.controller
                        }
                    }


                    // =================================================
                    // Duration
                    // =================================================

                    Label {
                        id: durationLabel

                        width:
                            root.compactMode ? 40 : 44

                        anchors.right:
                            parent.right

                        anchors.rightMargin:
                            root.compactMode ? 10 : 14

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

                        horizontalAlignment:
                            Text.AlignRight
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
                            if (
                                !root.hasController
                            ) {
                                return
                            }

                            root.controller.selectMyWaveTrack(
                                trackDelegate.index
                            )
                        }
                    }
                }
        }
    }


    // =============================================================
    // Current track
    // =============================================================

    function isCurrentTrack(
        title,
        artist
    ) {
        if (
            !root.hasController
        ) {
            return false
        }

        return (
            root.controller.currentTrackTitle === title &&
            root.controller.currentTrackArtist === artist
        )
    }


    // =============================================================
    // Duration
    // =============================================================

    function formatDuration(
        milliseconds
    ) {
        var value =
            Number(milliseconds)

        if (
            !isFinite(value) ||
            value <= 0
        ) {
            return "0:00"
        }

        var totalSeconds =
            Math.floor(
                value / 1000
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