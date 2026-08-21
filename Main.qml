import QtQuick
import QtQuick.Controls.Basic

import YaMusic.Core

ApplicationWindow {
    id: window

    width: 700
    height: 650

    minimumWidth: 450
    minimumHeight: 450

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
        spacing: 20

        Label {
            text: "YaMusic"

            font.pixelSize: 32

            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
            spacing: 10

            anchors.horizontalCenter: parent.horizontalCenter

            TextField {
                id: searchField

                width: 320

                placeholderText: "Search Yandex Music..."

                onAccepted: {
                    appController.testSearch(text)
                }
            }

            Button {
                text: "Search"

                onClicked: {
                    appController.testSearch(
                        searchField.text)
                }
            }
        }

        Label {
            id: statusLabel

            text: "Ready"

            anchors.horizontalCenter: parent.horizontalCenter
        }

        ListView {
            id: resultsView

            width: parent.width
            height: parent.height - 170

            model: appController.searchModel

            clip: true
            spacing: 6

            delegate: Rectangle {
                width: resultsView.width
                height: 80

                color: "#eeeeee"
                radius: 6

                Image {
                    id: coverImage

                    anchors.left: parent.left
                    anchors.leftMargin: 8

                    anchors.verticalCenter:
                        parent.verticalCenter

                    width: 64
                    height: 64

                    source:
                            "image://yandex/" +
                        coverUri

                    fillMode:
                        Image.PreserveAspectCrop

                    asynchronous: true
                    cache: true

                    Rectangle {
                        anchors.fill: parent

                        color: "#dddddd"
                        radius: 4

                        visible:
                            coverImage.status !==
                            Image.Ready

                        Label {
                            anchors.centerIn: parent

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

                    spacing: 4

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

                    anchors.right: parent.right
                    anchors.rightMargin: 15

                    anchors.verticalCenter:
                        parent.verticalCenter

                    text: formatDuration(durationMs)

                    font.pixelSize: 12

                    opacity: 0.6
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        appController.selectSearchResult(
                            index)
                    }
                }
            }
        }
    }

    // Converts milliseconds into a minutes:seconds string.
    function formatDuration(milliseconds) {
        var totalSeconds =
            Math.floor(milliseconds / 1000)

        var minutes =
            Math.floor(totalSeconds / 60)

        var seconds =
            totalSeconds % 60

        return minutes + ":" +
            (seconds < 10 ? "0" : "") +
            seconds
    }
}