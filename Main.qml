import QtQuick
import QtQuick.Controls.Basic

import YaMusic.Core

ApplicationWindow {
    id: window

    width: 760
    height: 720

    minimumWidth: 520
    minimumHeight: 520

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
            font.bold: true

            anchors.horizontalCenter: parent.horizontalCenter
        }

        Rectangle {
            width: parent.width
            height: 150

            color: "#f3f3f3"
            radius: 10

            Column {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 12

                Label {
                    text: "Yandex Music"
                    font.pixelSize: 18
                    font.bold: true
                }

                Row {
                    width: parent.width
                    spacing: 10

                    TextField {
                        id: tokenField

                        width: parent.width - saveButton.width - 10
                        placeholderText: "OAuth token"
                        echoMode: TextInput.Password

                        onAccepted: {
                            saveToken()
                        }
                    }

                    Button {
                        id: saveButton

                        text: "Save"

                        onClicked: {
                            saveToken()
                        }
                    }
                }

                Row {
                    spacing: 10

                    Label {
                        text: appController.authenticated
                            ? "Authenticated"
                            : "Not authenticated"

                        font.pixelSize: 13
                        opacity: 0.7
                    }

                    Button {
                        text: "Clear"

                        enabled: appController.authenticated

                        onClicked: {
                            appController.clearToken()
                        }
                    }

                    Button {
                        text: "Test Account"

                        enabled: appController.authenticated

                        onClicked: {
                            appController.testYandexApi()
                        }
                    }
                }
            }
        }

        Row {
            width: parent.width
            spacing: 10

            TextField {
                id: searchField

                width: parent.width - searchButton.width - 10
                placeholderText: "Search Yandex Music..."

                onAccepted: {
                    search()
                }
            }

            Button {
                id: searchButton

                text: "Search"

                onClicked: {
                    search()
                }
            }
        }

        Label {
            id: statusLabel

            width: parent.width

            text: "Ready"

            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight

            opacity: 0.7
        }

        ListView {
            id: resultsView

            width: parent.width
            height: parent.height -
                150 -
                20 -
                50 -
                20 -
                40

            model: appController.searchModel

            clip: true
            spacing: 8

            delegate: Rectangle {
                width: resultsView.width
                height: 80

                color: "#eeeeee"
                radius: 8

                Image {
                    id: coverImage

                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.verticalCenter

                    width: 64
                    height: 64

                    source: "image://yandex/" + coverUri

                    fillMode: Image.PreserveAspectCrop

                    asynchronous: true
                    cache: true

                    Rectangle {
                        anchors.fill: parent

                        color: "#dddddd"
                        radius: 4

                        visible: coverImage.status !== Image.Ready

                        Label {
                            anchors.centerIn: parent

                            text: "♪"
                            font.pixelSize: 24
                            opacity: 0.4
                        }
                    }
                }

                Column {
                    anchors.left: coverImage.right
                    anchors.leftMargin: 12

                    anchors.right: durationLabel.left
                    anchors.rightMargin: 12

                    anchors.verticalCenter: parent.verticalCenter

                    spacing: 4

                    Label {
                        width: parent.width

                        text: title

                        font.pixelSize: 16
                        font.bold: true

                        elide: Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text: artist

                        font.pixelSize: 14
                        opacity: 0.7

                        elide: Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text: album

                        font.pixelSize: 12
                        opacity: 0.5

                        elide: Text.ElideRight
                    }
                }

                Label {
                    id: durationLabel

                    anchors.right: parent.right
                    anchors.rightMargin: 15
                    anchors.verticalCenter: parent.verticalCenter

                    text: formatDuration(durationMs)

                    font.pixelSize: 12
                    opacity: 0.6
                }
            }
        }
    }

    function saveToken() {
        if (tokenField.text.trim().length === 0) {
            statusLabel.text = "Enter Yandex Music token"
            return
        }

        if (appController.setToken(tokenField.text)) {
            tokenField.clear()
        }
    }

    function search() {
        var query = searchField.text.trim()

        if (query.length === 0) {
            return
        }

        appController.testSearch(query)
    }

    // Converts milliseconds into a minutes:seconds string.
    function formatDuration(milliseconds) {
        var totalSeconds = Math.floor(milliseconds / 1000)
        var minutes = Math.floor(totalSeconds / 60)
        var seconds = totalSeconds % 60

        return minutes + ":" +
            (seconds < 10 ? "0" : "") +
            seconds
    }
}