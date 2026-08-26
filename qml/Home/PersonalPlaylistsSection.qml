import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    Rectangle {
        anchors.fill: parent

        radius: 10

        color: AppTheme.panel

        border.width: 1

        border.color: "#d4d4d4"

        Column {
            anchors.fill: parent

            anchors.margins: 12

            spacing: 8

            Label {
                text: "Собираем для вас"

                color: AppTheme.textPrimary

                font.pixelSize: 18
                font.bold: true
            }

            ListView {
                id: playlistView

                width: parent.width

                height:
                    parent.height - 34

                orientation:
                    ListView.Horizontal

                spacing: 12

                clip: true

                model:
                    root.controller
                        .personalPlaylistsModel

                ScrollBar.horizontal:
                    ScrollBar {
                        policy:
                            ScrollBar.AsNeeded
                    }

                delegate: Rectangle {
                    width: 150
                    height: 125

                    radius: 8

                    color:
                        mouseArea.containsMouse
                            ? AppTheme.panelActive
                            : "#f2f2f2"

                    border.width: 1

                    border.color:
                        mouseArea.containsMouse
                            ? "#c4c4c4"
                            : "#e0e0e0"

                    Column {
                        anchors.fill: parent

                        anchors.margins: 8

                        spacing: 6

                        Image {
                            id: cover

                            width: 134
                            height: 82

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

                                color: AppTheme.surface

                                visible:
                                    cover.status !==
                                    Image.Ready

                                Label {
                                    anchors.centerIn:
                                        parent

                                    text: "♪"

                                    color: AppTheme.textSecondary

                                    font.pixelSize: 26
                                }
                            }
                        }

                        Label {
                            width: parent.width

                            text: title

                            color: AppTheme.textPrimary

                            font.pixelSize: 13

                            font.bold: true

                            elide:
                                Text.ElideRight
                        }

                        Label {
                            width: parent.width

                            text:
                                    trackCount > 0
                                ? trackCount +
                                " треков"
                                : ""

                            color: AppTheme.textSecondary

                            font.pixelSize: 11

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
                                .selectPersonalPlaylist(
                                index)
                        }
                    }
                }

                Label {
                    anchors.centerIn:
                        parent

                    text:
                        root.controller
                            .loadingRecommendations
                            ? "Загрузка рекомендаций..."
                            : "Рекомендации пока недоступны"

                    color: AppTheme.textSecondary

                    visible:
                        root.controller
                            .personalPlaylistsModel
                            .count === 0
                }
            }
        }
    }
}