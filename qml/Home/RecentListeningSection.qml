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

            Label {
                text: "Вы недавно слушали"

                color: "#202020"

                font.pixelSize: 18
                font.bold: true
            }

            GridView {
                id: recentView

                width: parent.width
                height: parent.height - 34

                model:
                    root.controller
                        .recentListeningModel

                cellWidth: 175
                cellHeight: 125

                clip: true

                interactive: true

                ScrollBar.vertical:
                    ScrollBar {
                        policy: ScrollBar.AsNeeded
                    }

                delegate: Rectangle {
                    width: 165
                    height: 115

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

                    Column {
                        anchors.fill: parent

                        anchors.margins: 8

                        spacing: 5

                        Image {
                            id: cover

                            width: 149
                            height: 72

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

                                radius: 6

                                color: "#d0d0d0"

                                visible:
                                    cover.status !==
                                    Image.Ready

                                Label {
                                    anchors.centerIn:
                                        parent

                                    text:
                                            itemType === "play-context" &&
                                        context === "artist"
                                        ? "♫"
                                        : "♪"

                                    color: "#666666"

                                    font.pixelSize: 24
                                }
                            }
                        }

                        Label {
                            width: parent.width

                            text: title

                            color: "#202020"

                            font.pixelSize: 13
                            font.bold: true

                            elide:
                                Text.ElideRight
                        }

                        Label {
                            width: parent.width

                            text:
                                    context === "artist"
                                ? "Исполнитель"
                                : subtitle

                            color: "#666666"

                            font.pixelSize: 10

                            elide:
                                Text.ElideRight
                        }
                    }

                    MouseArea {
                        id: mouseArea

                        anchors.fill: parent

                        hoverEnabled: true

                        onClicked: {
                            root.controller
                                .selectRecentListening(
                                index)
                        }
                    }
                }

                Label {
                    anchors.centerIn:
                        parent

                    text: "История пока пуста"

                    color: "#666666"

                    visible:
                        root.controller
                            .recentListeningModel
                            .count === 0
                }
            }
        }
    }
}