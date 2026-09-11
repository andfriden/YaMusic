import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

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

    implicitHeight:
        pageColumn.implicitHeight + 48

    width:
        parent !== null
        ? parent.width
        : 0


    // =============================================================
    // Page
    // =============================================================

    Column {
        id: pageColumn

        x: 24
        y: 24

        width:
            Math.max(
                root.width - 48,
                0
            )

        spacing: 20


        // =========================================================
        // Album header
        // =========================================================

        Rectangle {
            id: albumHeader

            width: parent.width
            height: 260

            radius: 16

            color:
                AppTheme.surface

            Row {
                anchors.fill: parent
                anchors.margins: 24

                spacing: 24


                // -------------------------------------------------
                // Cover
                // -------------------------------------------------

                Rectangle {
                    width: 212
                    height: 212

                    radius: 12

                    color:
                        AppTheme.surfaceVariant

                    clip: true

                    Image {
                        anchors.fill: parent

                        source:
                            root.albumArtworkUri !== ""
                            ? "image://yandex/" +
                              root.albumArtworkUri
                            : ""

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous: true
                        cache: true
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

                    spacing: 8

                    Label {
                        text: "АЛЬБОМ"

                        color:
                            AppTheme.secondaryText

                        font.pixelSize: 13
                        font.bold: true
                    }

                    Label {
                        width: parent.width

                        text:
                            root.albumController !== null
                            ? root.albumController.currentAlbumTitle
                            : ""

                        font.pixelSize: 32
                        font.bold: true

                        color:
                            AppTheme.primaryText

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text:
                            root.trackCount > 0
                            ? root.trackCount +
                              (
                                  root.trackCount === 1
                                  ? " трек"
                                  : " трека"
                              )
                            : ""

                        font.pixelSize: 15

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
            width: parent.width

            implicitHeight:
                tracksColumn.implicitHeight + 32

            radius: 16

            color:
                AppTheme.surface


            Column {
                id: tracksColumn

                x: 16
                y: 16

                width:
                    parent.width - 32

                spacing: 8


                Label {
                    text: "Треки"

                    font.pixelSize: 20
                    font.bold: true

                    color:
                        AppTheme.primaryText
                }


                // -------------------------------------------------
                // Empty state
                // -------------------------------------------------

                Item {
                    width: parent.width

                    height:
                        root.trackCount === 0 &&
                        !root.loading
                        ? 68
                        : 0

                    visible:
                        height > 0

                    Label {
                        anchors.centerIn: parent

                        text:
                            "Треки не найдены"

                        font.pixelSize: 15

                        color:
                            AppTheme.secondaryText
                    }
                }


                // -------------------------------------------------
                // Track list
                // -------------------------------------------------

                ListView {
                    id: tracksView

                    width: parent.width

                    height:
                        root.trackCount > 0
                        ? root.trackCount * 82
                        : 0

                    visible:
                        root.trackCount > 0

                    model:
                        root.albumModel

                    interactive: false
                    clip: true

                    spacing: 8


                    delegate: Item {
                        id: trackDelegate

                        required property int index

                        required property string trackId
                        required property string title
                        required property string artist
                        required property string artistId
                        required property string coverUri
                        required property int durationMs
                        required property bool liked

                        width:
                            tracksView.width

                        height: 74


                        // =================================================
                        // Track background
                        // =================================================

                        Rectangle {
                            anchors.fill: parent

                            radius: 10

                            color:
                                trackMouseArea.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary
                        }


                        // =================================================
                        // Track content
                        // =================================================

                        Row {
                            anchors.fill: parent

                            anchors.leftMargin: 8
                            anchors.rightMargin: 52

                            spacing: 14


                            // -------------------------------------------------
                            // Track cover
                            // -------------------------------------------------

                            Rectangle {
                                width: 56
                                height: 56

                                anchors.verticalCenter:
                                    parent.verticalCenter

                                radius: 8

                                color:
                                    AppTheme.surfaceVariant

                                clip: true

                                Image {
                                    anchors.fill: parent

                                    source:
                                        trackDelegate.coverUri !== ""
                                        ? "image://yandex/" +
                                          trackDelegate.coverUri
                                        : ""

                                    fillMode:
                                        Image.PreserveAspectCrop

                                    asynchronous: true
                                    cache: true
                                }
                            }


                            // -------------------------------------------------
                            // Track information
                            // -------------------------------------------------

                            Column {
                                anchors.verticalCenter:
                                    parent.verticalCenter

                                width:
                                    parent.width -
                                    56 -
                                    14 -
                                    70

                                spacing: 3


                                Label {
                                    width: parent.width

                                    text:
                                        trackDelegate.title

                                    font.pixelSize: 15
                                    font.bold: true

                                    color:
                                        AppTheme.primaryText

                                    elide:
                                        Text.ElideRight
                                }


                                Label {
                                    width: parent.width

                                    text:
                                        trackDelegate.artist

                                    font.pixelSize: 13

                                    color:
                                        AppTheme.secondaryText

                                    elide:
                                        Text.ElideRight
                                }
                            }


                            // -------------------------------------------------
                            // Duration
                            // -------------------------------------------------

                            Label {
                                width: 56

                                anchors.verticalCenter:
                                    parent.verticalCenter

                                text:
                                    root.formatDuration(
                                        trackDelegate.durationMs
                                    )

                                horizontalAlignment:
                                    Text.AlignRight

                                font.pixelSize: 13

                                color:
                                    AppTheme.secondaryText
                            }
                        }


                        // =================================================
                        // Track click
                        // =================================================

                        MouseArea {
                            id: trackMouseArea

                            anchors.fill: parent

                            hoverEnabled: true

                            cursorShape:
                                Qt.PointingHandCursor

                            z: 0

                            onClicked: {
                                if (
                                    root.albumController !== null
                                ) {
                                    root.albumController.selectAlbumTrack(
                                        trackDelegate.index
                                    )
                                }
                            }
                        }


                        // =================================================
                        // Like button
                        // =================================================

                        Item {
                            id: likeButton

                            width: 36
                            height: 36

                            anchors.right:
                                parent.right

                            anchors.rightMargin: 8

                            anchors.verticalCenter:
                                parent.verticalCenter

                            z: 20


                            Label {
                                anchors.centerIn: parent

                                text:
                                    trackDelegate.liked
                                    ? "♥"
                                    : "♡"

                                color:
                                    trackDelegate.liked
                                    ? AppTheme.accent
                                    : AppTheme.textSecondary

                                font.pixelSize: 21
                            }


                            MouseArea {
                                anchors.fill: parent

                                cursorShape:
                                    Qt.PointingHandCursor

                                onClicked: {
                                    if (
                                        root.controller === null ||
                                        root.controller === undefined
                                    ) {
                                        return
                                    }

                                    root.controller.toggleLike(
                                        trackDelegate.trackId,
                                        trackDelegate.liked
                                    )
                                }
                            }
                        }
                    }
                }
            }
        }


        Item {
            width: parent.width
            height: 24
        }
    }


    // =============================================================
    // Loading overlay
    // =============================================================

    Rectangle {
        anchors.fill: parent

        visible:
            root.loading

        color:
            AppTheme.background

        opacity: 0.92

        z: 100

        BusyIndicator {
            anchors.centerIn: parent

            running:
                root.loading
        }
    }


    // =============================================================
    // Helpers
    // =============================================================

    function formatDuration(durationMs) {
        if (durationMs <= 0) {
            return ""
        }

        var totalSeconds =
            Math.floor(durationMs / 1000)

        var minutes =
            Math.floor(totalSeconds / 60)

        var seconds =
            totalSeconds % 60

        return minutes +
            ":" +
            (
                seconds < 10
                ? "0" + seconds
                : seconds
            )
    }
}
