import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    visible:
        controller.loadingAlbum ||
        controller.currentAlbumTitle.length > 0 ||
        controller.albumModel.count > 0

    Rectangle {
        anchors.fill: parent

        radius: 10

        color: "#e9e9e9"

        border.width: 1

        border.color: "#d4d4d4"

        Column {
            anchors.fill: parent

            anchors.margins: 12

            spacing: 8

            Row {
                width: parent.width

                spacing: 10

                Label {
                    id: titleLabel

                    text:
                        root.controller.currentAlbumTitle ||
                        "Альбом"

                    color: "#202020"

                    font.pixelSize: 18

                    font.bold: true

                    anchors.verticalCenter:
                        parent.verticalCenter
                }

                Label {
                    text:
                            root.controller.currentAlbumTrackCount > 0
                        ? root.controller.currentAlbumTrackCount +
                        " треков"
                        : ""

                    color: "#666666"

                    anchors.verticalCenter:
                        parent.verticalCenter
                }

                Item {
                    width:
                        Math.max(
                            0,
                            parent.width -
                            titleLabel.width -
                            100 -
                            30)

                    height: 1
                }

                Button {
                    width: 80

                    text: "Закрыть"

                    enabled:
                        !root.controller.loadingAlbum

                    onClicked: {
                        root.visible = false
                    }
                }
            }

            Row {
                width: parent.width

                height: 96

                spacing: 12

                visible:
                    root.controller.currentAlbumCoverUri.length > 0

                Image {
                    width: 96

                    height: 96

                    source:
                        root.controller.currentAlbumCoverUri
                            ? "image://yandex/" +
                            root.controller.currentAlbumCoverUri
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
                            parent.status !== Image.Ready

                        Label {
                            anchors.centerIn: parent

                            text: "♪"

                            color: "#666666"

                            font.pixelSize: 28
                        }
                    }
                }
            }

            ListView {
                id: tracksView

                width: parent.width

                height:
                    parent.height -
                    (
                            root.controller.currentAlbumCoverUri.length > 0
                            ? 151
                            : 45
                    )

                model:
                    root.controller.albumModel

                clip: true

                spacing: 6

                ScrollBar.vertical:
                    ScrollBar {
                        policy:
                            ScrollBar.AsNeeded
                    }

                delegate: Rectangle {
                    id: trackDelegate

                    width:
                        tracksView.width -
                        (
                            tracksView.ScrollBar.vertical.visible
                                ? 10
                                : 0
                        )

                    height: 68

                    radius: 8

                    color:
                        rowMouseArea.containsMouse
                            ? "#dcdcdc"
                            : "#f2f2f2"

                    border.width: 1

                    border.color:
                        rowMouseArea.containsMouse
                            ? "#c4c4c4"
                            : "#e0e0e0"

                    Image {
                        id: cover

                        anchors.left: parent.left

                        anchors.leftMargin: 8

                        anchors.verticalCenter:
                            parent.verticalCenter

                        width: 52

                        height: 52

                        source:
                            coverUri
                                ? "image://yandex/" + coverUri
                                : ""

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous: true

                        cache: true

                        Rectangle {
                            anchors.fill: parent

                            radius: 4

                            color: "#d0d0d0"

                            visible:
                                cover.status !== Image.Ready

                            Label {
                                anchors.centerIn: parent

                                text: "♪"

                                color: "#666666"

                                font.pixelSize: 20
                            }
                        }
                    }

                    Column {
                        id: trackInfo

                        anchors.left:
                            cover.right

                        anchors.leftMargin: 12

                        anchors.right:
                            durationLabel.left

                        anchors.rightMargin: 12

                        anchors.verticalCenter:
                            parent.verticalCenter

                        spacing: 2

                        Label {
                            width: parent.width

                            text: title

                            color: "#202020"

                            font.pixelSize: 14

                            font.bold: true

                            elide:
                                Text.ElideRight
                        }

                        Label {
                            id: artistLabel

                            width: parent.width

                            text: artist

                            color:
                                    artistId &&
                                artistId.length > 0
                                ? "#555555"
                                : "#777777"

                            font.pixelSize: 12

                            elide:
                                Text.ElideRight
                        }
                    }

                    Label {
                        id: durationLabel

                        anchors.right:
                            parent.right

                        anchors.rightMargin: 14

                        anchors.verticalCenter:
                            parent.verticalCenter

                        text:
                            formatDuration(durationMs)

                        color: "#555555"

                        font.pixelSize: 11
                    }

                    /*
                     * Обычный клик по строке.
                     */

                    MouseArea {
                        id: rowMouseArea

                        anchors.fill: parent

                        hoverEnabled: true

                        z: 0

                        onClicked: {
                            root.controller
                                .selectAlbumTrack(index)
                        }
                    }

                    /*
                     * Имя артиста находится поверх
                     * основной зоны строки.
                     */

                    MouseArea {
                        id: artistMouseArea

                        x:
                            trackInfo.x

                        y:
                            trackInfo.y + 20

                        width:
                            trackInfo.width

                        height: 18

                        z: 10

                        enabled:
                            artistId &&
                            artistId.length > 0

                        hoverEnabled: true

                        cursorShape:
                            enabled
                                ? Qt.PointingHandCursor
                                : Qt.ArrowCursor

                        onClicked: {
                            root.controller
                                .loadArtist(artistId)
                        }

                        Label {
                            anchors.fill: parent

                            text: artist

                            color:
                                artistMouseArea.containsMouse
                                    ? "#2468d7"
                                    : "#555555"

                            font.pixelSize: 12

                            elide:
                                Text.ElideRight
                        }
                    }
                }

                Label {
                    anchors.centerIn: parent

                    text:
                        root.controller.loadingAlbum
                            ? "Загрузка альбома..."
                            : "В альбоме нет треков"

                    color: "#666666"

                    visible:
                        root.controller.loadingAlbum ||
                        root.controller.albumModel.count === 0
                }
            }
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
            Math.floor(milliseconds / 1000)

        var minutes =
            Math.floor(totalSeconds / 60)

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