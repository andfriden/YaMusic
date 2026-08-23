import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

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
                    text: "Недавно слушали"

                    color: "#202020"

                    font.pixelSize: 18
                    font.bold: true

                    anchors.verticalCenter:
                        parent.verticalCenter
                }

                Label {
                    text:
                            root.controller
                                .recentListeningModel
                                .count > 0
                        ? root.controller
                            .recentListeningModel
                            .count +
                        " треков"
                        : ""

                    color: "#555555"

                    anchors.verticalCenter:
                        parent.verticalCenter
                }
            }

            ListView {
                id: tracksView

                width: parent.width

                height:
                    parent.height - 40

                model:
                    root.controller
                        .recentListeningModel

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
                            tracksView
                                .ScrollBar
                                .vertical
                                .visible
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

                        anchors.left:
                            parent.left

                        anchors.leftMargin: 8

                        anchors.verticalCenter:
                            parent.verticalCenter

                        width: 52
                        height: 52

                        source:
                            coverUri
                                ? "image://yandex/" +
                                coverUri
                                : ""

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous: true
                        cache: true

                        Rectangle {
                            anchors.fill:
                                parent

                            radius: 4

                            color: "#d0d0d0"

                            visible:
                                cover.status !==
                                Image.Ready

                            Label {
                                anchors.centerIn:
                                    parent

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

                            text:
                                title ||
                                "Без названия"

                            color: "#202020"

                            font.pixelSize: 14
                            font.bold: true

                            elide:
                                Text.ElideRight
                        }

                        Item {
                            id: artistArea

                            width: parent.width
                            height: 18
                        }

                        Label {
                            width: parent.width

                            text:
                                album ||
                                ""

                            color: "#777777"

                            font.pixelSize: 10

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
                            formatDuration(
                                durationMs)

                        color: "#555555"

                        font.pixelSize: 11
                    }

                    MouseArea {
                        id: rowMouseArea

                        anchors.fill:
                            parent

                        hoverEnabled: true

                        z: 0

                        onClicked: {
                            root.controller
                                .selectRecentListening(
                                index)
                        }
                    }

                    MouseArea {
                        id: artistMouseArea

                        x:
                            trackInfo.x

                        y:
                            trackInfo.y +
                            artistArea.y

                        width:
                            trackInfo.width

                        height: 18

                        z: 10

                        enabled:
                            (
                                artistId ||
                                ""
                            ).length > 0

                        hoverEnabled: true

                        cursorShape:
                            enabled
                                ? Qt.PointingHandCursor
                                : Qt.ArrowCursor

                        onClicked: {
                            var id =
                                artistId ||
                                ""

                            if (
                                id.length > 0
                            ) {
                                root.controller
                                    .loadArtist(
                                    id)
                            }
                        }

                        Label {
                            anchors.fill:
                                parent

                            text:
                                artist ||
                                ""

                            color:
                                artistMouseArea
                                    .containsMouse
                                    ? "#2468d7"
                                    : "#555555"

                            font.pixelSize: 12

                            elide:
                                Text.ElideRight
                        }
                    }
                }

                Item {
                    anchors.fill:
                        parent

                    visible:
                        root.controller
                            .recentListeningModel
                            .count === 0

                    Label {
                        anchors.centerIn:
                            parent

                        text:
                            "Недавно слушали пока пусто"

                        color: "#666666"

                        font.pixelSize: 12
                    }
                }
            }
        }
    }

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