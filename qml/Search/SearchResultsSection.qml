import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    Column {
        anchors.fill: parent

        spacing: 8

        Label {
            text: "Результаты поиска"

            color: AppTheme.textPrimary

            font.pixelSize: 18
            font.bold: true
        }

        ListView {
            id: resultsView

            width: parent.width
            height: parent.height - 30

            model: root.controller.searchModel

            clip: true

            spacing: 6

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            delegate: Rectangle {
                id: resultItem

                width:
                    resultsView.width -
                    (
                        resultsView
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
                        ? AppTheme.panelActive
                        : "#e8e8e8"

                border.width: 1

                border.color:
                    rowMouseArea.containsMouse
                        ? "#c4c4c4"
                        : "#dedede"

                // Основной клик по результату:
                // воспроизведение трека.
                MouseArea {
                    id: rowMouseArea

                    anchors.fill: parent

                    hoverEnabled: true

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked: {
                        root.controller
                            .selectSearchResult(
                            index)
                    }
                }

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

                        color: AppTheme.surface

                        visible:
                            cover.status !==
                            Image.Ready

                        Label {
                            anchors.centerIn:
                                parent

                            text: "♪"

                            color: AppTheme.textSecondary

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

                        color: AppTheme.textPrimary

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
                            artistMouseArea.containsMouse
                                ? AppTheme.textPrimary
                                : AppTheme.textSecondary

                        font.pixelSize: 12

                        elide:
                            Text.ElideRight

                        MouseArea {
                            id: artistMouseArea

                            anchors.fill: parent

                            hoverEnabled: true

                            enabled:
                                artistId &&
                                artistId.length > 0

                            cursorShape:
                                Qt.PointingHandCursor

                            onClicked: {
                                root.controller
                                    .loadArtist(
                                    artistId)
                            }
                        }
                    }

                    Label {
                        id: albumLabel

                        width: parent.width

                        text: album

                        color:
                            albumMouseArea.containsMouse
                                ? AppTheme.textPrimary
                                : AppTheme.textMuted

                        font.pixelSize: 10

                        elide:
                            Text.ElideRight

                        MouseArea {
                            id: albumMouseArea

                            anchors.fill: parent

                            hoverEnabled: true

                            enabled:
                                albumId &&
                                albumId.length > 0

                            cursorShape:
                                Qt.PointingHandCursor

                            onClicked: {
                                root.controller
                                    .loadAlbum(
                                    albumId)
                            }
                        }
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

                    color: AppTheme.textSecondary

                    font.pixelSize: 11
                }
            }

            Label {
                anchors.centerIn:
                    parent

                text:
                    root.controller.searching
                        ? "Поиск..."
                        : "Ничего не найдено"

                color: AppTheme.textSecondary

                visible:
                    root.controller
                        .searchModel
                        .count === 0
            }
        }
    }

    function formatDuration(
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