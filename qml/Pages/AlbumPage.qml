import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller: null

    readonly property var albumController:
            root.controller !== null &&
        root.controller.albumController !== undefined &&
        root.controller.albumController !== null
        ? root.controller.albumController
        : null

    readonly property var albumModel:
            root.albumController !== null &&
        root.albumController.albumModel !== undefined &&
        root.albumController.albumModel !== null
        ? root.albumController.albumModel
        : null

    readonly property bool loading:
        root.albumController !== null &&
        root.albumController.loading === true

    readonly property string albumTitle:
            root.albumController !== null
        ? String(root.albumController.currentAlbumTitle || "")
        : ""

    readonly property string albumCover:
            root.albumController !== null
        ? String(root.albumController.currentAlbumCoverUri || "")
        : ""

    readonly property bool hasAlbum:
        root.albumTitle.length > 0

    // -------------------------------------------------------------
    // Page contract
    // -------------------------------------------------------------

    readonly property int pageHeight: 700

    width: parent ? parent.width : 0
    height: pageHeight

    // -------------------------------------------------------------
    // Background
    // -------------------------------------------------------------

    Rectangle {
        anchors.fill: parent

        color: "#f5f5f5"
    }

    // -------------------------------------------------------------
    // Main scroll area
    // -------------------------------------------------------------

    ScrollView {
        id: pageScroll

        anchors.fill: parent

        clip: true

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        Column {
            id: pageColumn

            width: pageScroll.availableWidth

            spacing: 18

            // -----------------------------------------------------
            // Top spacing
            // -----------------------------------------------------

            Item {
                width: 1
                height: 8
            }

            // -----------------------------------------------------
            // Album header
            // -----------------------------------------------------

            Rectangle {
                width: parent.width
                height: 190

                radius: 14

                color: "#e9e9e9"

                border.width: 1
                border.color: "#d6d6d6"

                Row {
                    anchors.fill: parent
                    anchors.margins: 18

                    spacing: 20

                    // -------------------------------------------------
                    // Artwork
                    // -------------------------------------------------

                    Rectangle {
                        id: artworkBox

                        width: 154
                        height: 154

                        radius: 10

                        color: "#d2d2d2"

                        clip: true

                        anchors.verticalCenter:
                            parent.verticalCenter

                        Image {
                            id: albumImage

                            anchors.fill: parent

                            source:
                                    root.albumCover.length > 0
                                ? "image://yandex/" +
                                root.albumCover
                                : ""

                            sourceSize:
                                Qt.size(154, 154)

                            fillMode:
                                Image.PreserveAspectCrop

                            asynchronous: true
                            cache: true

                            visible:
                                status === Image.Ready
                        }

                        Label {
                            anchors.centerIn: parent

                            text:
                                    albumImage.status === Image.Error
                                ? "!"
                                : "♪"

                            color: "#777777"

                            font.pixelSize: 36

                            visible:
                                albumImage.status !== Image.Ready
                        }

                        Rectangle {
                            anchors.fill: parent

                            radius: 10

                            color: "transparent"

                            border.width: 1
                            border.color: "#c9c9c9"
                        }
                    }

                    // -------------------------------------------------
                    // Album information
                    // -------------------------------------------------

                    Column {
                        width:
                            parent.width -
                            artworkBox.width -
                            parent.spacing

                        anchors.verticalCenter:
                            parent.verticalCenter

                        spacing: 8

                        Label {
                            width: parent.width

                            text:
                                qsTr("АЛЬБОМ")

                            color: "#888888"

                            font.pixelSize: 11
                            font.bold: true
                        }

                        Label {
                            width: parent.width

                            text:
                                root.hasAlbum
                                    ? root.albumTitle
                                    : qsTr("Альбом")

                            color: "#202020"

                            font.pixelSize: 28
                            font.bold: true

                            elide:
                                Text.ElideRight
                        }

                        Label {
                            width: parent.width

                            text:
                                    tracksView.count > 0
                                ? qsTr("%1 треков")
                                    .arg(tracksView.count)
                                : ""

                            color: "#666666"

                            font.pixelSize: 13

                            visible:
                                tracksView.count > 0
                        }

                        Button {
                            width: 130
                            height: 36

                            text:
                                qsTr("▶  Слушать")

                            enabled:
                                !root.loading &&
                                tracksView.count > 0

                            onClicked: {
                                if (
                                    root.albumController === null
                                ) {
                                    return
                                }

                                root.albumController.playAlbum()
                            }
                        }
                    }
                }
            }

            // -----------------------------------------------------
            // Tracks
            // -----------------------------------------------------

            Rectangle {
                width: parent.width

                height:
                    Math.max(
                        74 +
                        tracksView.count * 68,
                        110
                    )

                radius: 14

                color: "#e9e9e9"

                border.width: 1
                border.color: "#d6d6d6"

                Column {
                    anchors.fill: parent

                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    anchors.topMargin: 12
                    anchors.bottomMargin: 12

                    spacing: 8

                    Label {
                        width: parent.width

                        text:
                                tracksView.count > 0
                            ? qsTr("Треки  (%1)")
                                .arg(tracksView.count)
                            : qsTr("Треки")

                        color: "#202020"

                        font.pixelSize: 20
                        font.bold: true
                    }

                    ListView {
                        id: tracksView

                        width: parent.width

                        height:
                            Math.max(
                                count * 64,
                                1
                            )

                        model:
                            root.albumModel

                        interactive: false

                        clip: true

                        spacing: 4

                        delegate: Rectangle {
                            id: trackRow

                            required property int index
                            required property string trackId
                            required property string title
                            required property string artist
                            required property string artistId
                            required property string coverUri
                            required property int durationMs

                            width: tracksView.width
                            height: 60

                            radius: 8

                            color:
                                rowMouse.containsMouse
                                    ? "#dddddd"
                                    : "#f1f1f1"

                            border.width: 1

                            border.color:
                                rowMouse.containsMouse
                                    ? "#c8c8c8"
                                    : "#dfdfdf"

                            // -------------------------------------------------
                            // Cover
                            // -------------------------------------------------

                            Rectangle {
                                id: trackCoverBox

                                width: 46
                                height: 46

                                anchors.left: parent.left
                                anchors.leftMargin: 7

                                anchors.verticalCenter:
                                    parent.verticalCenter

                                radius: 6

                                color: "#d0d0d0"

                                clip: true

                                Image {
                                    id: trackCoverImage

                                    anchors.fill: parent

                                    source:
                                            trackRow.coverUri.length > 0
                                        ? "image://yandex/" +
                                        trackRow.coverUri
                                        : ""

                                    sourceSize:
                                        Qt.size(46, 46)

                                    fillMode:
                                        Image.PreserveAspectCrop

                                    asynchronous: true
                                    cache: true

                                    visible:
                                        status === Image.Ready
                                }

                                Label {
                                    anchors.centerIn: parent

                                    text: "♪"

                                    color: "#777777"

                                    font.pixelSize: 17

                                    visible:
                                        trackCoverImage.status !==
                                        Image.Ready
                                }
                            }

                            // -------------------------------------------------
                            // Text
                            // -------------------------------------------------

                            Column {
                                id: textColumn

                                anchors.left:
                                    trackCoverBox.right

                                anchors.leftMargin: 11

                                anchors.right:
                                    durationLabel.left

                                anchors.rightMargin: 10

                                anchors.verticalCenter:
                                    parent.verticalCenter

                                spacing: 2

                                Label {
                                    width: parent.width

                                    text:
                                            trackRow.title.length > 0
                                        ? trackRow.title
                                        : qsTr("Без названия")

                                    color: "#202020"

                                    font.pixelSize: 13
                                    font.bold: true

                                    elide:
                                        Text.ElideRight
                                }

                                Label {
                                    id: artistLabel

                                    width: parent.width

                                    text:
                                            trackRow.artist.length > 0
                                        ? trackRow.artist
                                        : qsTr(
                                            "Неизвестный исполнитель"
                                        )

                                    color:
                                        artistMouse.containsMouse
                                            ? "#2468d7"
                                            : "#555555"

                                    font.pixelSize: 11

                                    elide:
                                        Text.ElideRight
                                }
                            }

                            // -------------------------------------------------
                            // Duration
                            // -------------------------------------------------

                            Label {
                                id: durationLabel

                                anchors.right: parent.right
                                anchors.rightMargin: 12

                                anchors.verticalCenter:
                                    parent.verticalCenter

                                text:
                                    root.formatDuration(
                                        trackRow.durationMs
                                    )

                                color: "#666666"

                                font.pixelSize: 11
                            }

                            // -------------------------------------------------
                            // Whole row
                            // -------------------------------------------------

                            MouseArea {
                                id: rowMouse

                                anchors.fill: parent

                                hoverEnabled: true

                                cursorShape:
                                    Qt.PointingHandCursor

                                onClicked: {
                                    if (
                                        root.albumController === null
                                    ) {
                                        return
                                    }

                                    root.albumController
                                        .selectAlbumTrack(
                                        trackRow.index
                                    )
                                }
                            }

                            // -------------------------------------------------
                            // Artist click
                            // -------------------------------------------------

                            MouseArea {
                                id: artistMouse

                                anchors.left:
                                    artistLabel.left

                                anchors.right:
                                    artistLabel.right

                                anchors.top:
                                    artistLabel.top

                                anchors.bottom:
                                    artistLabel.bottom

                                hoverEnabled: true

                                enabled:
                                    trackRow.artistId.length > 0

                                cursorShape:
                                    enabled
                                        ? Qt.PointingHandCursor
                                        : Qt.ArrowCursor

                                onClicked: {
                                    if (
                                        root.controller === null ||
                                        trackRow.artistId.length === 0
                                    ) {
                                        return
                                    }

                                    root.controller.loadArtist(
                                        trackRow.artistId
                                    )
                                }
                            }
                        }
                    }
                }
            }

            // -----------------------------------------------------
            // Loading / empty state
            // -----------------------------------------------------

            Label {
                width: parent.width

                text:
                    root.loading
                        ? qsTr("Загрузка альбома...")
                        : (
                                root.hasAlbum &&
                                tracksView.count === 0
                                ? qsTr(
                                    "В альбоме нет треков"
                                )
                                : qsTr("Выберите альбом")
                        )

                color: "#777777"

                font.pixelSize: 13

                horizontalAlignment:
                    Text.AlignHCenter

                visible:
                    root.loading ||
                    (
                        !root.loading &&
                        (
                            !root.hasAlbum ||
                            tracksView.count === 0
                        )
                    )
            }

            Item {
                width: 1
                height: 16
            }
        }
    }

    // -------------------------------------------------------------
    // Loading overlay
    // -------------------------------------------------------------

    Rectangle {
        anchors.fill: parent

        color: "#f5f5f5"

        opacity: 0.92

        visible:
            root.loading &&
            !root.hasAlbum

        Column {
            anchors.centerIn: parent

            spacing: 8

            BusyIndicator {
                width: 30
                height: 30

                anchors.horizontalCenter:
                    parent.horizontalCenter

                running: true
            }

            Label {
                text:
                    qsTr("Загрузка альбома...")

                color: "#666666"

                font.pixelSize: 13
            }
        }
    }

    Component.onCompleted: {
        console.log(
            "========== AlbumPage =========="
        )

        console.log(
            "controller:",
            root.controller
        )

        console.log(
            "albumController:",
            root.albumController
        )

        console.log(
            "albumModel:",
            root.albumModel
        )

        console.log(
            "pageHeight:",
            root.pageHeight
        )

        console.log(
            "================================"
        )
    }

    function formatDuration(milliseconds) {
        if (
            milliseconds === undefined ||
            milliseconds === null ||
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