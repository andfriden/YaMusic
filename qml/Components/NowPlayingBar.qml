import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    Rectangle {
        anchors.fill: parent

        radius: 12

        color: "#e9e9e9"

        border.width: 1
        border.color: "#d4d4d4"

        Row {
            id: contentRow

            anchors.fill: parent
            anchors.margins: 12

            spacing: 12

            Image {
                id: cover

                width: 82
                height: 82

                anchors.verticalCenter:
                    parent.verticalCenter

                source:
                    root.controller.currentTrackCoverUri
                        ? "image://yandex/" +
                        root.controller.currentTrackCoverUri
                        : ""

                fillMode:
                    Image.PreserveAspectCrop

                asynchronous: true
                cache: true

                Rectangle {
                    anchors.fill: parent

                    radius: 8

                    color: "#d0d0d0"

                    visible:
                        cover.status !==
                        Image.Ready

                    Label {
                        anchors.centerIn: parent

                        text: "♪"

                        color: "#666666"

                        font.pixelSize: 28
                    }
                }
            }

            Column {
                id: trackInfo

                width:
                    Math.max(
                        180,
                        contentRow.width -
                        cover.width -
                        controls.width -
                        48)

                anchors.verticalCenter:
                    parent.verticalCenter

                spacing: 4

                Label {
                    width: parent.width

                    text:
                        root.controller.currentTrackTitle ||
                        "Ничего не играет"

                    color: "#202020"

                    font.pixelSize: 15
                    font.bold: true

                    elide:
                        Text.ElideRight
                }

                Label {
                    id: artistLabel

                    width: parent.width

                    text:
                        root.controller.currentTrackArtist

                    color:
                        artistMouseArea.containsMouse
                            ? "#202020"
                            : "#666666"

                    font.pixelSize: 12

                    elide:
                        Text.ElideRight

                    MouseArea {
                        id: artistMouseArea

                        anchors.fill: parent

                        hoverEnabled: true

                        enabled:
                            root.controller
                                .currentTrackArtistId
                                .length > 0

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked: {
                            var artistId =
                                root.controller
                                    .currentTrackArtistId

                            if (artistId.length > 0) {
                                root.controller
                                    .loadArtist(
                                    artistId)
                            }
                        }
                    }
                }

                Row {
                    width: parent.width

                    spacing: 6

                    Label {
                        width: 32

                        text:
                            formatTime(
                                root.controller.position)

                        color: "#777777"

                        font.pixelSize: 10

                        horizontalAlignment:
                            Text.AlignLeft
                    }

                    Slider {
                        id: progressSlider

                        width:
                            Math.max(
                                80,
                                parent.width - 82)

                        from: 0

                        to:
                            Math.max(
                                1,
                                root.controller.duration)

                        value:
                            Math.min(
                                root.controller.position,
                                to)

                        enabled:
                            root.controller.duration > 0

                        onMoved: {
                            root.controller.seek(
                                value)
                        }
                    }
                }
            }

            Row {
                id: controls

                width: 282

                anchors.verticalCenter:
                    parent.verticalCenter

                spacing: 8

                Button {
                    width: 40
                    height: 40

                    text: "‹"

                    enabled:
                        root.controller
                            .currentTrackTitle
                            .length > 0

                    onClicked:
                        root.controller.previous()
                }

                Button {
                    width: 48
                    height: 40

                    text:
                        root.controller.playing
                            ? "Ⅱ"
                            : "▶"

                    enabled:
                        root.controller
                            .currentTrackTitle
                            .length > 0

                    onClicked: {
                        if (
                            root.controller.playing
                        ) {
                            root.controller.pause()
                        } else {
                            root.controller.play()
                        }
                    }
                }

                Button {
                    width: 40
                    height: 40

                    text: "›"

                    enabled:
                        root.controller
                            .currentTrackTitle
                            .length > 0

                    onClicked:
                        root.controller.next()
                }

                Button {
                    width: 40
                    height: 40

                    text:
                            root.controller.repeatMode === 0
                        ? "↻"
                        : root.controller.repeatMode === 1
                            ? "↻A"
                            : "↻1"

                    enabled:
                        root.controller
                            .currentTrackTitle
                            .length > 0

                    onClicked:
                        root.controller.cycleRepeat()
                }

                Button {
                    width: 40
                    height: 40

                    text:
                        root.controller.shuffleEnabled
                            ? "🔀"
                            : "⇄"

                    enabled:
                        root.controller
                            .currentTrackTitle
                            .length > 0

                    onClicked:
                        root.controller.toggleShuffle()
                }

                Button {
                    width: 40
                    height: 40

                    text: "■"

                    enabled:
                        root.controller
                            .currentTrackTitle
                            .length > 0

                    onClicked:
                        root.controller.stop()
                }
            }
        }
    }

    function formatTime(
        milliseconds)
    {
        if (
            !milliseconds ||
            milliseconds <= 0
        ) {
            return "0:00"
        }

        var totalSeconds =
            Math.floor(
                milliseconds / 1000)

        var minutes =
            Math.floor(
                totalSeconds / 60)

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