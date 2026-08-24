import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller
    property bool compactMode: false
    property bool hasSelectedTrack: false

    readonly property bool hasTracks:
        root.controller !== null &&
        root.controller.myWaveModel.count > 0

    readonly property int margin:
        compactMode ? 12 : 24

    readonly property int headerHeight:
        compactMode ? 28 : 40

    readonly property int spacingValue:
        compactMode ? 10 : 16

    /*
     * Background
     */

    Rectangle {
        anchors.fill: parent

        visible:
            root.compactMode ||
            root.hasTracks

        radius:
            compactMode
                ? 10
                : 14

        color: "#e9e9e9"

        border.width: 1
        border.color: "#d4d4d4"
    }

    /*
     * Compact Home
     */

    Column {
        id: compactContent

        anchors.fill: parent

        anchors.margins:
            root.margin

        spacing:
            root.spacingValue

        visible:
            root.compactMode &&
            root.hasTracks

        Row {
            width: parent.width
            height: root.headerHeight

            spacing: 10

            Label {
                text: "Моя волна"

                color: "#202020"

                font.pixelSize: 18
                font.bold: true

                anchors.verticalCenter:
                    parent.verticalCenter
            }

            Label {
                text:
                    root.controller.myWaveModel.count +
                    " треков"

                color: "#777777"

                font.pixelSize: 11

                anchors.verticalCenter:
                    parent.verticalCenter
            }
        }

        ListView {
            id: compactList

            width: parent.width

            height:
                parent.height -
                root.headerHeight -
                root.spacingValue

            model:
                root.controller.myWaveModel

            clip: true

            spacing: 6

            boundsBehavior:
                Flickable.StopAtBounds

            ScrollBar.vertical:
                ScrollBar {
                    policy:
                        ScrollBar.AsNeeded
                }

            delegate: Rectangle {
                required property int index
                required property string title
                required property string artist
                required property string coverUri
                required property int durationMs

                width:
                    compactList.width -
                    (
                        compactList
                            .ScrollBar
                            .vertical
                            .visible
                            ? 10
                            : 0
                    )

                height: 58

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
                    id: compactCover

                    anchors.left:
                        parent.left

                    anchors.leftMargin: 7

                    anchors.verticalCenter:
                        parent.verticalCenter

                    width: 44
                    height: 44

                    source:
                            coverUri.length > 0
                        ? "image://yandex/" +
                        coverUri
                        : ""

                    fillMode:
                        Image.PreserveAspectCrop

                    asynchronous: true
                    cache: true
                }

                Column {
                    anchors.left:
                        compactCover.right

                    anchors.leftMargin: 10

                    anchors.right:
                        durationLabel.left

                    anchors.rightMargin: 10

                    anchors.verticalCenter:
                        parent.verticalCenter

                    spacing: 1

                    Label {
                        width: parent.width

                        text:
                                title.length > 0
                            ? title
                            : "Без названия"

                        color: "#202020"

                        font.pixelSize: 13
                        font.bold: true

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text: artist

                        color: "#555555"

                        font.pixelSize: 11

                        elide:
                            Text.ElideRight
                    }
                }

                Label {
                    id: durationLabel

                    anchors.right:
                        parent.right

                    anchors.rightMargin: 10

                    anchors.verticalCenter:
                        parent.verticalCenter

                    text:
                        root.formatDuration(
                            durationMs)

                    color: "#666666"

                    font.pixelSize: 10
                }

                MouseArea {
                    id: mouseArea

                    anchors.fill: parent

                    hoverEnabled: true

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked: {
                        root.controller
                            .selectMyWaveTrack(
                            index)
                    }
                }
            }
        }
    }

    /*
     * Full My Wave
     */

    Column {
        id: fullContent

        anchors.fill: parent

        anchors.margins:
            root.margin

        spacing: 16

        visible:
            !root.compactMode &&
            root.hasTracks

        /*
         * Header
         */

        Row {
            id: fullHeader

            width: parent.width

            height: root.headerHeight

            spacing: 12

            Label {
                text: "Моя волна"

                color: "#202020"

                font.pixelSize: 30
                font.bold: true

                anchors.verticalCenter:
                    parent.verticalCenter
            }

            Label {
                text:
                    root.controller.myWaveModel.count +
                    " треков"

                color: "#777777"

                font.pixelSize: 13

                anchors.verticalCenter:
                    parent.verticalCenter
            }
        }

        /*
         * Current track
         *
         * Appears only after the user
         * selects a track.
         */

        Rectangle {
            id: nowPlayingCard

            width: parent.width

            height: 210

            radius: 14

            color: "#f1f1f1"

            border.width: 1
            border.color: "#d8d8d8"

            visible:
                root.hasSelectedTrack

            Row {
                anchors.fill: parent

                anchors.margins: 18

                spacing: 18

                Image {
                    id: currentCover

                    width: 174
                    height: 174

                    source:
                            root.controller.currentTrackCoverUri.length > 0
                        ? "image://yandex/" +
                        root.controller.currentTrackCoverUri
                        : ""

                    fillMode:
                        Image.PreserveAspectCrop

                    asynchronous: true
                    cache: true

                    Rectangle {
                        anchors.fill:
                            parent

                        radius: 10

                        color: "#d0d0d0"

                        visible:
                            currentCover.status !==
                            Image.Ready

                        Label {
                            anchors.centerIn:
                                parent

                            text: "♪"

                            color: "#666666"

                            font.pixelSize: 40
                        }
                    }
                }

                Column {
                    width:
                        parent.width -
                        currentCover.width -
                        18

                    anchors.verticalCenter:
                        parent.verticalCenter

                    spacing: 8

                    Label {
                        text: "Сейчас играет"

                        color: "#888888"

                        font.pixelSize: 12
                    }

                    Label {
                        width: parent.width

                        text:
                            root.controller.currentTrackTitle

                        color: "#202020"

                        font.pixelSize: 24

                        font.bold: true

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text:
                            root.controller.currentTrackArtist

                        color: "#555555"

                        font.pixelSize: 16

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        visible:
                            root.controller.duration > 0

                        text:
                            root.formatDuration(
                                root.controller.duration)

                        color: "#888888"

                        font.pixelSize: 12
                    }
                }
            }
        }

        /*
         * Track list
         */

        Label {
            id: nextTitle

            text:
                root.hasSelectedTrack
                    ? "Далее"
                    : "Треки"

            color: "#202020"

            font.pixelSize: 20

            font.bold: true

            height: 26
        }

        ListView {
            id: fullList

            width: parent.width

            height:
                Math.max(
                    0,
                    parent.height -
                    fullHeader.height -
                    (
                        root.hasSelectedTrack
                            ? nowPlayingCard.height
                            : 0
                    ) -
                    nextTitle.height -
                    (
                        fullContent.spacing * 3
                    )
                )

            model:
                root.controller.myWaveModel

            clip: true

            spacing: 6

            boundsBehavior:
                Flickable.StopAtBounds

            ScrollBar.vertical:
                ScrollBar {
                    policy:
                        ScrollBar.AsNeeded
                }

            delegate: Rectangle {
                required property int index
                required property string trackId
                required property string title
                required property string artist
                required property string album
                required property string coverUri
                required property int durationMs

                width:
                    fullList.width -
                    (
                        fullList
                            .ScrollBar
                            .vertical
                            .visible
                            ? 10
                            : 0
                    )

                height: 68

                radius: 9

                color:
                        root.hasSelectedTrack &&
                    root.isCurrentTrack(
                        title,
                        artist)
                    ? "#d9d9d9"
                    : (
                        fullMouseArea.containsMouse
                            ? "#e1e1e1"
                            : "#f2f2f2"
                    )

                border.width: 1

                border.color:
                        root.hasSelectedTrack &&
                    root.isCurrentTrack(
                        title,
                        artist)
                    ? "#bdbdbd"
                    : "#e0e0e0"

                Image {
                    id: trackCover

                    anchors.left:
                        parent.left

                    anchors.leftMargin: 9

                    anchors.verticalCenter:
                        parent.verticalCenter

                    width: 52
                    height: 52

                    source:
                            coverUri.length > 0
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

                        radius: 6

                        color: "#d0d0d0"

                        visible:
                            trackCover.status !==
                            Image.Ready
                    }
                }

                Column {
                    anchors.left:
                        trackCover.right

                    anchors.leftMargin: 14

                    anchors.right:
                        durationText.left

                    anchors.rightMargin: 14

                    anchors.verticalCenter:
                        parent.verticalCenter

                    spacing: 2

                    Label {
                        width: parent.width

                        text:
                                title.length > 0
                            ? title
                            : "Без названия"

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
                        visible:
                            album.length > 0

                        width: parent.width

                        text: album

                        color: "#888888"

                        font.pixelSize: 10

                        elide:
                            Text.ElideRight
                    }
                }

                Label {
                    id: durationText

                    anchors.right:
                        parent.right

                    anchors.rightMargin: 14

                    anchors.verticalCenter:
                        parent.verticalCenter

                    text:
                        root.formatDuration(
                            durationMs)

                    color: "#666666"

                    font.pixelSize: 11
                }

                MouseArea {
                    id: fullMouseArea

                    anchors.fill:
                        parent

                    hoverEnabled: true

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked: {
                        root.hasSelectedTrack = true

                        root.controller
                            .selectMyWaveTrack(
                            index)
                    }
                }
            }
        }
    }

    function isCurrentTrack(
        title,
        artist)
    {
        if (
            root.controller === null
        ) {
            return false
        }

        return (
            root.controller.currentTrackTitle === title &&
            root.controller.currentTrackArtist === artist
        )
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