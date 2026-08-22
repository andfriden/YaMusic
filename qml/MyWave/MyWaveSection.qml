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
                    text: "Моя волна"

                    color: "#202020"

                    font.pixelSize: 18

                    font.bold: true

                    anchors.verticalCenter:
                        parent.verticalCenter
                }

                Button {
                    width: 110

                    text:
                        root.controller
                            .loadingMyWave
                            ? "Загрузка..."
                            : "Загрузить"

                    enabled:
                        !root.controller.loadingMyWave &&
                        !root.controller.loadingMoreMyWave

                    onClicked: {
                        root.controller.loadMyWave()
                    }
                }

                Label {
                    text:
                            root.controller.myWaveModel.count > 0
                        ? root.controller
                            .myWaveModel
                            .count +
                        " треков"
                        : ""

                    color: "#555555"

                    anchors.verticalCenter:
                        parent.verticalCenter
                }
            }

            ListView {
                id: waveView

                property bool autoLoadArmed: true

                width: parent.width

                height:
                    parent.height - 40

                model:
                    root.controller.myWaveModel

                clip: true

                spacing: 6

                ScrollBar.vertical:
                    ScrollBar {
                        policy:
                            ScrollBar.AsNeeded
                    }

                onContentYChanged: {
                    var distanceToBottom =
                        contentHeight -
                        (contentY + height)

                    if (
                        distanceToBottom > 450
                    ) {
                        autoLoadArmed = true
                    }

                    if (
                        autoLoadArmed &&
                        distanceToBottom <= 300 &&
                        !root.controller.loadingMyWave &&
                        !root.controller.loadingMoreMyWave &&
                        root.controller.myWaveModel.count > 0
                    ) {
                        autoLoadArmed = false

                        root.controller
                            .loadMoreMyWave()
                    }
                }

                delegate: Rectangle {
                    width:
                        waveView.width -
                        (
                            waveView
                                .ScrollBar
                                .vertical
                                .visible
                                ? 10
                                : 0
                        )

                    height: 68

                    radius: 8

                    color:
                        mouseArea.containsMouse
                            ? "#dcdcdc"
                            : "#f2f2f2"

                    border.width: 1

                    border.color:
                        mouseArea.containsMouse
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
                            anchors.fill: parent

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
                            width: parent.width

                            text: artist

                            color: "#555555"

                            font.pixelSize: 12

                            elide:
                                Text.ElideRight
                        }

                        Label {
                            width: parent.width

                            text: album

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
                        id: mouseArea

                        anchors.fill: parent

                        hoverEnabled: true

                        onClicked: {
                            root.controller
                                .selectMyWaveTrack(
                                index)
                        }
                    }
                }

                Label {
                    anchors.centerIn:
                        parent

                    text:
                        root.controller.loadingMyWave
                            ? "Загрузка моей волны..."
                            : "Моя волна пока пуста"

                    color: "#666666"

                    visible:
                        root.controller.myWaveModel.count === 0
                }

                Rectangle {
                    anchors.left:
                        parent.left

                    anchors.right:
                        parent.right

                    anchors.bottom:
                        parent.bottom

                    height: 30

                    radius: 6

                    color: "#e0e0e0"

                    visible:
                        root.controller.loadingMoreMyWave

                    Label {
                        anchors.centerIn:
                            parent

                        text:
                            "Загрузка следующей части..."

                        color: "#555555"

                        font.pixelSize: 11
                    }
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