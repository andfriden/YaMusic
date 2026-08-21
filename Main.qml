import QtQuick
import QtQuick.Controls.Basic

import YaMusic.Core

ApplicationWindow {
    id: window

    width: 700
    height: 720

    minimumWidth: 500
    minimumHeight: 560

    visible: true
    title: "YaMusic"

    AppController {
        id: appController

        onStatusChanged: function(message) {
            statusLabel.text = message
        }
    }

    Column {
        anchors.fill: parent
        anchors.margins: 30

        spacing: 18

        Label {
            text: "YaMusic"

            font.pixelSize: 32
            font.bold: true

            anchors.horizontalCenter:
                parent.horizontalCenter
        }

        Row {
            width: parent.width

            spacing: 10

            TextField {
                id: searchField

                width:
                    parent.width -
                    searchButton.width -
                    10

                placeholderText:
                    "Search Yandex Music..."

                enabled:
                    !appController.searching

                onAccepted: {
                    search()
                }
            }

            Button {
                id: searchButton

                width: 90

                text:
                    appController.searching
                        ? "..."
                        : "Search"

                enabled:
                    !appController.searching &&
                    searchField.text.trim().length > 0

                onClicked: {
                    search()
                }
            }
        }

        Label {
            id: statusLabel

            width: parent.width

            text: "Ready"

            horizontalAlignment:
                Text.AlignHCenter

            elide:
                Text.ElideRight

            opacity: 0.7
        }

        ListView {
            id: resultsView

            width: parent.width

            height:
                parent.height -
                nowPlaying.height -
                145

            model:
                appController.searchModel

            clip: true

            spacing: 6

            ScrollBar.vertical:
                ScrollBar {
                    policy:
                        ScrollBar.AsNeeded
                }

            delegate: Rectangle {
                width:
                    resultsView.width -
                    (resultsView.ScrollBar.vertical.visible
                        ? 10
                        : 0)

                height: 80

                radius: 8

                color:
                    mouseArea.containsMouse
                        ? "#dddddd"
                        : "#eeeeee"

                border.width:
                    mouseArea.containsMouse
                        ? 1
                        : 0

                border.color:
                    "#cccccc"

                Image {
                    id: coverImage

                    anchors.left:
                        parent.left

                    anchors.leftMargin: 8

                    anchors.verticalCenter:
                        parent.verticalCenter

                    width: 64
                    height: 64

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

                        color: "#d8d8d8"

                        visible:
                            coverImage.status !==
                            Image.Ready

                        Label {
                            anchors.centerIn:
                                parent

                            text: "♪"

                            font.pixelSize: 24

                            opacity: 0.4
                        }
                    }
                }

                Column {
                    anchors.left:
                        coverImage.right

                    anchors.leftMargin: 12

                    anchors.right:
                        durationLabel.left

                    anchors.rightMargin: 12

                    anchors.verticalCenter:
                        parent.verticalCenter

                    spacing: 3

                    Label {
                        width: parent.width

                        text: title

                        font.pixelSize: 16
                        font.bold: true

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text: artist

                        font.pixelSize: 14

                        opacity: 0.7

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text: album

                        font.pixelSize: 12

                        opacity: 0.5

                        elide:
                            Text.ElideRight
                    }
                }

                Label {
                    id: durationLabel

                    anchors.right:
                        parent.right

                    anchors.rightMargin: 15

                    anchors.verticalCenter:
                        parent.verticalCenter

                    text:
                        formatDuration(durationMs)

                    font.pixelSize: 12

                    opacity: 0.6
                }

                MouseArea {
                    id: mouseArea

                    anchors.fill:
                        parent

                    hoverEnabled: true

                    onClicked: {
                        appController.selectSearchResult(
                            index)
                    }
                }
            }

            Label {
                anchors.centerIn:
                    parent

                text:
                    appController.searching
                        ? "Searching..."
                        : "No results"

                visible:
                    appController.searchModel.count === 0

                opacity: 0.45
            }
        }

        Rectangle {
            id: nowPlaying

            width: parent.width

            height: 142

            radius: 10

            color: "#e9e9e9"

            border.width: 1
            border.color: "#d5d5d5"

            Column {
                anchors.fill:
                    parent

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

                        color: "#d8d8d8"

                        clip: true

                        Image {
                            id: nowPlayingArtwork

                            anchors.fill:
                                parent

                            source:
                                appController.currentTrackCoverUri
                                    ? "image://yandex/" +
                                    appController.currentTrackCoverUri
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

                            font.pixelSize: 28

                            opacity: 0.4

                            visible:
                                nowPlayingArtwork.status !==
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
                                appController.currentTrackTitle ||
                                "Nothing playing"

                            font.pixelSize: 16
                            font.bold: true

                            elide:
                                Text.ElideRight
                        }

                        Label {
                            width: parent.width

                            text:
                                appController.currentTrackArtist ||
                                "Select a track"

                            font.pixelSize: 13

                            opacity: 0.65

                            elide:
                                Text.ElideRight
                        }

                        Label {
                            width: parent.width

                            text:
                                playbackStateText(
                                    appController.playbackState)

                            font.pixelSize: 11

                            opacity: 0.5
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
                                appController.playing
                                    ? "❚❚"
                                    : "▶"

                            enabled:
                                appController.playbackState !== 1

                            onClicked: {
                                if (appController.playing) {
                                    appController.pause()
                                } else {
                                    appController.play()
                                }
                            }
                        }

                        Button {
                            width: 42
                            height: 42

                            text: "■"

                            enabled:
                                appController.playbackState !== 0

                            onClicked: {
                                appController.stop()
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
                                appController.position)

                        font.pixelSize: 11

                        opacity: 0.6

                        horizontalAlignment:
                            Text.AlignLeft
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
                                appController.duration)

                        value:
                            appController.position

                        enabled:
                            appController.duration > 0

                        onMoved: {
                            appController.seek(
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
                                appController.duration)

                        font.pixelSize: 11

                        opacity: 0.6

                        horizontalAlignment:
                            Text.AlignRight
                    }
                }
            }
        }
    }

    function search() {
        var query =
            searchField.text.trim()

        if (query.length === 0) {
            return
        }

        appController.testSearch(
            query)
    }

    function playbackStateText(state) {
        switch (state) {
            case 0:
                return "Ready"

            case 1:
                return "Loading..."

            case 2:
                return "Playing"

            case 3:
                return "Paused"

            case 4:
                return "Error"

            default:
                return "Ready"
        }
    }

    function formatDuration(milliseconds) {
        if (!milliseconds ||
            milliseconds <= 0) {
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
            (seconds < 10
                ? "0"
                : "") +
            seconds
    }
}