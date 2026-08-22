import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    Rectangle {
        anchors.fill: parent

        radius: 10

        color: "#dedede"

        border.width: 1

        border.color: "#c8c8c8"

        Column {
            anchors.fill: parent

            anchors.margins: 12

            spacing: 8

            Row {
                width: parent.width

                height: 72

                spacing: 14

                Rectangle {
                    id: artworkContainer

                    width: 72
                    height: 72

                    radius: 6

                    color: "#cccccc"

                    clip: true

                    Image {
                        id: artwork

                        anchors.fill: parent

                        source:
                            root.controller
                                .currentTrackCoverUri
                                ? "image://yandex/" +
                                root.controller
                                    .currentTrackCoverUri
                                : ""

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous: true

                        cache: true

                        visible:
                            status === Image.Ready
                    }

                    Label {
                        anchors.centerIn:
                            parent

                        text: "♪"

                        color: "#666666"

                        font.pixelSize: 28

                        visible:
                            artwork.status !==
                            Image.Ready
                    }
                }

                Column {
                    id: trackInfo

                    width:
                        parent.width -
                        controls.width -
                        artworkContainer.width -
                        28

                    anchors.verticalCenter:
                        parent.verticalCenter

                    spacing: 5

                    Label {
                        width: parent.width

                        text:
                            root.controller
                                .currentTrackTitle ||
                            "Ничего не играет"

                        color: "#181818"

                        font.pixelSize: 16

                        font.bold: true

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text:
                            root.controller
                                .currentTrackArtist ||
                            "Выберите трек"

                        color: "#4f4f4f"

                        font.pixelSize: 13

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text:
                            playbackStateText(
                                root.controller
                                    .playbackState)

                        color: "#666666"

                        font.pixelSize: 11
                    }
                }

                Row {
                    id: controls

                    anchors.verticalCenter:
                        parent.verticalCenter

                    spacing: 6

                    Button {
                        width: 42
                        height: 42

                        text:
                            root.controller.playing
                                ? "❚❚"
                                : "▶"

                        enabled:
                            root.controller
                                .playbackState !== 1

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
                        width: 42
                        height: 42

                        text: "■"

                        enabled:
                            root.controller
                                .playbackState !== 0

                        onClicked: {
                            root.controller.stop()
                        }
                    }
                }
            }

            Row {
                width: parent.width

                height: 28

                spacing: 8

                Label {
                    id: currentTimeLabel

                    width: 42

                    anchors.verticalCenter:
                        parent.verticalCenter

                    text:
                        formatDuration(
                            root.controller.position)

                    color: "#555555"

                    font.pixelSize: 11
                }

                Slider {
                    id: positionSlider

                    width:
                        parent.width -
                        currentTimeLabel.width -
                        totalTimeLabel.width -
                        16

                    anchors.verticalCenter:
                        parent.verticalCenter

                    from: 0

                    to:
                        Math.max(
                            0,
                            root.controller.duration)

                    value:
                        root.controller.position

                    enabled:
                        root.controller.duration > 0

                    onMoved: {
                        root.controller.seek(
                            value)
                    }
                }

                Label {
                    id: totalTimeLabel

                    width: 42

                    anchors.verticalCenter:
                        parent.verticalCenter

                    text:
                        formatDuration(
                            root.controller.duration)

                    color: "#555555"

                    font.pixelSize: 11

                    horizontalAlignment:
                        Text.AlignRight
                }
            }
        }
    }

    function playbackStateText(state) {
        switch (state) {
            case 0:
                return "Готово"

            case 1:
                return "Загрузка..."

            case 2:
                return "Воспроизведение"

            case 3:
                return "Пауза"

            case 4:
                return "Ошибка"

            default:
                return "Готово"
        }
    }

    function formatDuration(milliseconds) {
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