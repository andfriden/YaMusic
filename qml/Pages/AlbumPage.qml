import QtQuick
import QtQuick.Controls.Basic

import YaMusic.Core


Item {
    id: root


    // =============================================================
    // Controller
    // =============================================================

    property var controller


    readonly property var albumController:
            root.controller !== null &&
        root.controller !== undefined &&
        root.controller.albumController !== null &&
        root.controller.albumController !== undefined
        ? root.controller.albumController
        : null


    readonly property var albumModel:
            root.albumController !== null &&
        root.albumController.albumModel !== null &&
        root.albumController.albumModel !== undefined
        ? root.albumController.albumModel
        : null


    // =============================================================
    // Album state
    // =============================================================

    readonly property bool loading:
            root.albumController !== null
        ? root.albumController.loading
        : false


    readonly property bool hasAlbum:
        root.albumController !== null &&
        root.albumController.albumId !== ""


    readonly property int trackCount:
            root.albumModel !== null
        ? root.albumModel.count
        : 0


    readonly property string albumArtworkUri:
            root.albumController !== null
        ? root.albumController.currentAlbumCoverUri
        : ""


    // =============================================================
    // Page size
    // =============================================================

    implicitHeight:
        pageColumn.implicitHeight + 48


    width:
            parent !== null
        ? parent.width
        : 0


    // =============================================================
    // Content
    // =============================================================

    Column {
        id: pageColumn

        x:
            24

        y:
            24

        width:
            Math.max(
                root.width - 48,
                0
            )

        spacing:
            20


        // =========================================================
        // Album header
        // =========================================================

        Rectangle {
            id: albumHeader

            width:
                parent.width

            height:
                260

            radius:
                16

            color:
                AppTheme.surface


            Row {
                anchors.fill:
                    parent

                anchors.margins:
                    24

                spacing:
                    24


                // -------------------------------------------------
                // Artwork
                // -------------------------------------------------

                Rectangle {
                    width:
                        212

                    height:
                        212

                    radius:
                        12

                    color:
                        AppTheme.surfaceVariant

                    clip:
                        true


                    Image {
                        anchors.fill:
                            parent

                        source:
                                root.albumArtworkUri !== ""
                            ? "image://yandex/" +
                            root.albumArtworkUri
                            : ""

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous:
                            true

                        cache:
                            true
                    }
                }


                // -------------------------------------------------
                // Album information
                // -------------------------------------------------

                Column {
                    anchors.verticalCenter:
                        parent.verticalCenter

                    width:
                        parent.width -
                        212 -
                        24

                    spacing:
                        8


                    Label {
                        text:
                            "АЛЬБОМ"

                        font.pixelSize:
                            13

                        font.bold:
                            true

                        color:
                            AppTheme.secondaryText
                    }


                    Label {
                        width:
                            parent.width

                        text:
                                root.albumController !== null
                            ? root.albumController.currentAlbumTitle
                            : ""

                        font.pixelSize:
                            32

                        font.bold:
                            true

                        color:
                            AppTheme.primaryText

                        elide:
                            Text.ElideRight
                    }


                    Label {
                        width:
                            parent.width

                        text:
                                root.trackCount > 0
                            ? root.trackCount +
                            (
                                    root.trackCount === 1
                                    ? " трек"
                                    : " трека"
                            )
                            : ""

                        font.pixelSize:
                            15

                        color:
                            AppTheme.secondaryText
                    }
                }
            }
        }


        // =========================================================
        // Tracks
        // =========================================================

        Rectangle {
            id: tracksCard

            width:
                parent.width

            implicitHeight:
                tracksColumn.implicitHeight + 32

            radius:
                16

            color:
                AppTheme.surface


            Column {
                id: tracksColumn

                x:
                    16

                y:
                    16

                width:
                    parent.width - 32

                spacing:
                    8


                Label {
                    text:
                        "Треки"

                    font.pixelSize:
                        20

                    font.bold:
                        true

                    color:
                        AppTheme.primaryText
                }


                // -------------------------------------------------
                // Empty state
                // -------------------------------------------------

                Item {
                    width:
                        parent.width

                    height:
                            root.trackCount === 0 &&
                        !root.loading
                        ? 68
                        : 0

                    visible:
                        height > 0


                    Label {
                        anchors.centerIn:
                            parent

                        text:
                            "Треки не найдены"

                        font.pixelSize:
                            15

                        color:
                            AppTheme.secondaryText
                    }
                }


                // -------------------------------------------------
                // Track list
                // -------------------------------------------------

                ListView {
                    id: tracksView

                    width:
                        parent.width

                    height:
                            root.trackCount > 0
                        ? root.trackCount * 74
                        : 0

                    visible:
                        root.trackCount > 0

                    model:
                        root.albumModel

                    interactive:
                        false

                    clip:
                        true


                    delegate: Item {
                        id: trackDelegate

                        required property int index
                        required property string trackId
                        required property string title
                        required property string artist
                        required property string artistId
                        required property string coverUri
                        required property int durationMs

                        width:
                            tracksView.width

                        height:
                            74


                        Rectangle {
                            anchors.fill:
                                parent

                            radius:
                                10

                            color:
                                trackMouseArea.containsMouse
                                    ? AppTheme.surfaceVariant
                                    : "transparent"


                            Row {
                                anchors.fill:
                                    parent

                                anchors.leftMargin:
                                    8

                                anchors.rightMargin:
                                    8

                                spacing:
                                    14


                                // ---------------------------------
                                // Cover
                                // ---------------------------------

                                Rectangle {
                                    width:
                                        56

                                    height:
                                        56

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    radius:
                                        8

                                    color:
                                        AppTheme.surfaceVariant

                                    clip:
                                        true


                                    Image {
                                        anchors.fill:
                                            parent

                                        source:
                                                trackDelegate.coverUri !== ""
                                            ? "image://yandex/" +
                                            trackDelegate.coverUri
                                            : ""

                                        fillMode:
                                            Image.PreserveAspectCrop

                                        asynchronous:
                                            true

                                        cache:
                                            true
                                    }
                                }


                                // ---------------------------------
                                // Track info
                                // ---------------------------------

                                Column {
                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    width:
                                        parent.width -
                                        56 -
                                        14 -
                                        70

                                    spacing:
                                        3


                                    Label {
                                        width:
                                            parent.width

                                        text:
                                            trackDelegate.title

                                        font.pixelSize:
                                            15

                                        font.bold:
                                            true

                                        color:
                                            AppTheme.primaryText

                                        elide:
                                            Text.ElideRight
                                    }


                                    Label {
                                        width:
                                            parent.width

                                        text:
                                            trackDelegate.artist

                                        font.pixelSize:
                                            13

                                        color:
                                            AppTheme.secondaryText

                                        elide:
                                            Text.ElideRight
                                    }
                                }


                                // ---------------------------------
                                // Duration
                                // ---------------------------------

                                Label {
                                    width:
                                        56

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    text:
                                        root.formatDuration(
                                            trackDelegate.durationMs
                                        )

                                    horizontalAlignment:
                                        Text.AlignRight

                                    font.pixelSize:
                                        13

                                    color:
                                        AppTheme.secondaryText
                                }
                            }
                        }


                        MouseArea {
                            id: trackMouseArea

                            anchors.fill:
                                parent

                            hoverEnabled:
                                true

                            cursorShape:
                                Qt.PointingHandCursor

                            onClicked:
                            {
                                if (
                                    root.albumController !== null
                                )
                                {
                                    root.albumController
                                        .selectAlbumTrack(
                                        trackDelegate.index
                                    )
                                }
                            }
                        }
                    }
                }
            }
        }


        // =========================================================
        // Bottom spacing
        // =========================================================

        Item {
            width:
                parent.width

            height:
                24
        }
    }


    // =============================================================
    // Loading
    // =============================================================

    Rectangle {
        anchors.fill:
            parent

        visible:
            root.loading

        color:
            AppTheme.background

        opacity:
            0.92

        z:
            100


        BusyIndicator {
            anchors.centerIn:
                parent

            running:
                root.loading
        }
    }


    // =============================================================
    // Helpers
    // =============================================================

    function formatDuration(durationMs)
    {
        if (
            durationMs === undefined ||
            durationMs === null ||
            durationMs <= 0
        )
        {
            return ""
        }


        var totalSeconds =
            Math.floor(durationMs / 1000)

        var minutes =
            Math.floor(totalSeconds / 60)

        var seconds =
            totalSeconds % 60


        return (
            minutes +
            ":" +
            (seconds < 10 ? "0" : "") +
            seconds
        )
    }
}