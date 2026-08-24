import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller
    property int selectedTrackIndex: -1

    readonly property var artistController:
            controller !== null
        ? controller.artistController
        : null

    readonly property bool loaded:
        artistController !== null &&
        artistController.artistName.length > 0

    implicitHeight:
        pageColumn.implicitHeight + 48

    Rectangle {
        anchors.fill: parent
        color: "#f5f5f5"
    }

    Column {
        id: pageColumn

        width: parent.width

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        anchors.topMargin: 24
        anchors.leftMargin: 28
        anchors.rightMargin: 28

        spacing: 28

        Row {
            width: parent.width
            height: 220

            spacing: 24

            Image {
                id: artistCover

                width: 220
                height: 220

                source:
                        root.loaded &&
                    root.artistController.artistCoverUri.length > 0
                    ? "image://yandex/" +
                    root.artistController.artistCoverUri
                    : ""

                fillMode: Image.PreserveAspectCrop

                asynchronous: true
                cache: true

                Rectangle {
                    anchors.fill: parent

                    radius: 14
                    color: "#dddddd"

                    visible:
                        artistCover.status !== Image.Ready

                    Label {
                        anchors.centerIn: parent

                        text: "♪"

                        color: "#888888"

                        font.pixelSize: 48
                    }
                }
            }

            Column {
                width:
                    parent.width -
                    artistCover.width -
                    parent.spacing

                anchors.verticalCenter:
                    parent.verticalCenter

                spacing: 10

                Label {
                    text: qsTr("Исполнитель")

                    color: "#888888"
                    font.pixelSize: 13
                }

                Label {
                    width: parent.width

                    text:
                        root.loaded
                            ? root.artistController.artistName
                            : ""

                    color: "#202020"

                    font.pixelSize: 34
                    font.bold: true

                    elide: Text.ElideRight
                }

                Label {
                    visible:
                        root.loaded &&
                        root.artistController.artistGenres.length > 0

                    width: parent.width

                    text:
                        root.artistController.artistGenres

                    color: "#666666"
                    font.pixelSize: 13

                    wrapMode: Text.WordWrap
                }

                Label {
                    visible:
                        root.loaded &&
                        root.artistController.artistDescription.length > 0

                    width: parent.width
                    height: 64

                    text:
                        root.artistController.artistDescription

                    color: "#555555"
                    font.pixelSize: 13

                    wrapMode: Text.WordWrap
                    elide: Text.ElideRight
                }
            }
        }

        Row {
            width: parent.width
            height: 360

            spacing: 24

            Column {
                width: parent.width * 0.62
                height: parent.height

                spacing: 12

                Label {
                    text: qsTr("Популярные треки")

                    color: "#202020"

                    font.pixelSize: 22
                    font.bold: true
                }

                ListView {
                    id: tracksView

                    width: parent.width

                    height:
                        5 * 64 +
                        4 * 6

                    model:
                            root.artistController !== null
                        ? root.artistController.artistModel
                        : null

                    clip: true

                    spacing: 6

                    boundsBehavior:
                        Flickable.StopAtBounds

                    ScrollBar.vertical:
                        ScrollBar {
                            policy:
                                    tracksView.contentHeight >
                                tracksView.height
                                ? ScrollBar.AsNeeded
                                : ScrollBar.AlwaysOff
                        }

                    delegate: Rectangle {
                        required property int index
                        required property string trackId
                        required property string title
                        required property string artist
                        required property string album
                        required property string coverUri
                        required property int durationMs

                        width: tracksView.width
                        height: 64

                        radius: 9

                        color:
                                root.selectedTrackIndex === index
                            ? "#d6d6d6"
                            : (
                                mouseArea.containsMouse
                                    ? "#e2e2e2"
                                    : "#eeeeee"
                            )

                        border.width: 1

                        border.color:
                                root.selectedTrackIndex === index
                            ? "#b8b8b8"
                            : (
                                mouseArea.containsMouse
                                    ? "#c9c9c9"
                                    : "#dddddd"
                            )

                        Label {
                            anchors.left: parent.left
                            anchors.leftMargin: 12

                            anchors.verticalCenter:
                                parent.verticalCenter

                            width: 24

                            text:
                                (index + 1)
                                    .toString()
                                    .padStart(2, "0")

                            color:
                                    root.selectedTrackIndex === index
                                ? "#202020"
                                : "#999999"

                            font.pixelSize: 11

                            font.bold:
                                root.selectedTrackIndex === index
                        }

                        Image {
                            id: trackCover

                            anchors.left: parent.left
                            anchors.leftMargin: 42

                            anchors.verticalCenter:
                                parent.verticalCenter

                            width: 50
                            height: 50

                            source:
                                    coverUri.length > 0
                                ? "image://yandex/" +
                                coverUri
                                : ""

                            fillMode: Image.PreserveAspectCrop

                            asynchronous: true
                            cache: true

                            Rectangle {
                                anchors.fill: parent

                                radius: 6
                                color: "#d0d0d0"

                                visible:
                                    trackCover.status !== Image.Ready

                                Label {
                                    anchors.centerIn: parent

                                    text: "♪"
                                    color: "#777777"
                                    font.pixelSize: 18
                                }
                            }
                        }

                        Column {
                            anchors.left: trackCover.right
                            anchors.leftMargin: 12

                            anchors.right: durationLabel.left
                            anchors.rightMargin: 12

                            anchors.verticalCenter:
                                parent.verticalCenter

                            spacing: 2

                            Label {
                                width: parent.width

                                text:
                                        title.length > 0
                                    ? title
                                    : qsTr("Без названия")

                                color: "#202020"

                                font.pixelSize: 14
                                font.bold: true

                                elide: Text.ElideRight
                            }

                            Label {
                                width: parent.width

                                text: artist

                                color: "#555555"

                                font.pixelSize: 12

                                elide: Text.ElideRight
                            }

                            Label {
                                visible: album.length > 0

                                width: parent.width

                                text: album

                                color: "#888888"

                                font.pixelSize: 10

                                elide: Text.ElideRight
                            }
                        }

                        Label {
                            id: durationLabel

                            anchors.right: parent.right
                            anchors.rightMargin: 14

                            anchors.verticalCenter:
                                parent.verticalCenter

                            text:
                                root.formatDuration(
                                    durationMs
                                )

                            color: "#666666"
                            font.pixelSize: 11
                        }

                        MouseArea {
                            id: mouseArea

                            anchors.fill: parent

                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (
                                    root.artistController === null
                                ) {
                                    return
                                }

                                root.selectedTrackIndex =
                                    index

                                root.artistController.selectTrack(
                                    index
                                )
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width * 0.38
                height: parent.height

                radius: 12

                color: "#eeeeee"

                border.width: 1
                border.color: "#dddddd"

                visible:
                    root.artistController !== null &&
                    root.artistController.newReleaseTitle.length > 0

                Column {
                    anchors.fill: parent
                    anchors.margins: 18

                    spacing: 12

                    Label {
                        text: qsTr("Последний релиз")

                        color: "#202020"

                        font.pixelSize: 22
                        font.bold: true
                    }

                    Image {
                        id: releaseCover

                        width: parent.width

                        height:
                            Math.min(
                                parent.width,
                                220
                            )

                        source:
                                root.artistController !== null &&
                            root.artistController.newReleaseCoverUri.length > 0
                            ? "image://yandex/" +
                            root.artistController.newReleaseCoverUri
                            : ""

                        fillMode: Image.PreserveAspectCrop

                        asynchronous: true
                        cache: true

                        Rectangle {
                            anchors.fill: parent

                            radius: 10
                            color: "#d0d0d0"

                            visible:
                                releaseCover.status !== Image.Ready
                        }
                    }

                    Label {
                        width: parent.width

                        text:
                                root.artistController !== null
                            ? root.artistController.newReleaseTitle
                            : ""

                        color: "#202020"

                        font.pixelSize: 18
                        font.bold: true

                        elide: Text.ElideRight
                    }

                    Label {
                        visible:
                            root.artistController !== null &&
                            root.artistController.newReleaseYear > 0

                        text:
                                root.artistController !== null
                            ? root.artistController.newReleaseYear
                            : ""

                        color: "#777777"
                        font.pixelSize: 12
                    }
                }
            }
        }

        ArtistAlbumsSection {
            width: parent.width
            height: implicitHeight

            controller: root.controller
        }

        Item {
            width: 1
            height: 24
        }
    }

    Column {
        anchors.centerIn: parent

        spacing: 10

        visible:
            root.artistController !== null &&
            root.artistController.loading &&
            !root.loaded

        BusyIndicator {
            width: 30
            height: 30

            anchors.horizontalCenter:
                parent.horizontalCenter

            running: true
        }

        Label {
            text: qsTr("Загрузка исполнителя...")

            color: "#666666"
            font.pixelSize: 13
        }
    }

    Label {
        anchors.centerIn: parent

        visible:
            root.artistController !== null &&
            !root.artistController.loading &&
            !root.loaded

        text: qsTr("Выберите исполнителя")

        color: "#999999"
        font.pixelSize: 14
    }

    function formatDuration(milliseconds) {
        if (!milliseconds || milliseconds <= 0)
            return "0:00"

        var totalSeconds =
            Math.floor(milliseconds / 1000)

        var minutes =
            Math.floor(totalSeconds / 60)

        var seconds =
            totalSeconds % 60

        return minutes +
            ":" +
            (seconds < 10 ? "0" : "") +
            seconds
    }
}