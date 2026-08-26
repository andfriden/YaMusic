import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller
    property bool compactMode: false
    property bool hasSelectedTrack: false

    readonly property bool hasController:
        root.controller !== null &&
        root.controller !== undefined

    readonly property bool hasTracks:
        root.hasController &&
        root.controller.myWaveModel !== null &&
        root.controller.myWaveModel !== undefined &&
        root.controller.myWaveModel.count > 0

    readonly property int margin:
        root.compactMode ? 12 : 24

    readonly property int headerHeight:
        root.compactMode ? 28 : 40

    readonly property int spacingValue:
        root.compactMode ? 10 : 16

    Rectangle {
        anchors.fill: parent

        visible:
            root.compactMode ||
            root.hasTracks

        radius:
            root.compactMode
                ? 10
                : 14

        color: AppTheme.panel

        border.width: 1
        border.color: "#d4d4d4"
    }

    /*
     * ============================================================
     * COMPACT MODE
     * ============================================================
     */

    Column {
        id: compactContent

        anchors.fill: parent
        anchors.margins: root.margin

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

                color: AppTheme.textPrimary

                font.pixelSize: 18
                font.bold: true

                anchors.verticalCenter:
                    parent.verticalCenter
            }

            Label {
                text:
                    root.controller.myWaveModel.count +
                    " треков"

                color: AppTheme.textMuted

                font.pixelSize: 11

                anchors.verticalCenter:
                    parent.verticalCenter
            }
        }

        ListView {
            id: compactList

            width: parent.width

            height:
                Math.max(
                    0,
                    parent.height -
                    root.headerHeight -
                    root.spacingValue)

            model:
                root.hasController
                    ? root.controller.myWaveModel
                    : null

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
                        compactList.ScrollBar.vertical.visible
                            ? 10
                            : 0
                    )

                height: 58

                radius: 8

                color:
                    compactMouseArea.containsMouse
                        ? AppTheme.panelActive
                        : "#f2f2f2"

                border.width: 1

                border.color:
                    compactMouseArea.containsMouse
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
                        ? "image://yandex/" + coverUri
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
                            compactCover.status !==
                            Image.Ready

                        Label {
                            anchors.centerIn:
                                parent

                            text: "♪"

                            color: AppTheme.textSecondary

                            font.pixelSize: 18
                        }
                    }
                }

                Column {
                    anchors.left:
                        compactCover.right

                    anchors.leftMargin: 10

                    anchors.right:
                        compactDuration.left

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

                        color: AppTheme.textPrimary

                        font.pixelSize: 13
                        font.bold: true

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text:
                            artist

                        color: AppTheme.textSecondary

                        font.pixelSize: 11

                        elide:
                            Text.ElideRight
                    }
                }

                Label {
                    id: compactDuration

                    anchors.right:
                        parent.right

                    anchors.rightMargin: 10

                    anchors.verticalCenter:
                        parent.verticalCenter

                    text:
                        root.formatDuration(
                            durationMs)

                    color: AppTheme.textSecondary

                    font.pixelSize: 10
                }

                MouseArea {
                    id: compactMouseArea

                    anchors.fill:
                        parent

                    hoverEnabled: true

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked: {
                        root.controller.selectMyWaveTrack(
                            index)
                    }
                }
            }
        }
    }

    /*
     * ============================================================
     * FULL MODE
     * ============================================================
     */

    Column {
        id: fullContent

        anchors.fill: parent
        anchors.margins: root.margin

        spacing: 16

        visible:
            !root.compactMode &&
            root.hasTracks

        /*
         * --------------------------------------------------------
         * Header
         * --------------------------------------------------------
         */

        Row {
            id: fullHeader

            width: parent.width
            height: root.headerHeight

            spacing: 12

            Label {
                text: "Моя волна"

                color: AppTheme.textPrimary

                font.pixelSize: 30
                font.bold: true

                anchors.verticalCenter:
                    parent.verticalCenter
            }

            Label {
                text:
                    root.controller.myWaveModel.count +
                    " треков"

                color: AppTheme.textMuted

                font.pixelSize: 13

                anchors.verticalCenter:
                    parent.verticalCenter
            }
        }

        /*
         * --------------------------------------------------------
         * Current track
         * --------------------------------------------------------
         */

        Rectangle {
            id: nowPlayingCard

            width: parent.width
            height: 220

            radius: 14

            color: AppTheme.panelSecondary

            border.width: 1
            border.color: "#d8d8d8"

            visible:
                root.hasSelectedTrack

            Row {
                anchors.fill: parent

                anchors.margins: 18

                spacing: 20

                /*
                 * Artwork
                 */

                Image {
                    id: currentCover

                    width: 184
                    height: 184

                    anchors.verticalCenter:
                        parent.verticalCenter

                    source:
                            root.hasController &&
                        root.controller.currentTrackCoverUri.length > 0
                        ? "image://yandex/" +
                        root.controller.currentTrackCoverUri
                        : ""

                    fillMode:
                        Image.PreserveAspectCrop

                    asynchronous: true
                    cache: true

                    Rectangle {
                        anchors.fill: parent

                        radius: 10

                        color: AppTheme.surface

                        visible:
                            currentCover.status !==
                            Image.Ready

                        Label {
                            anchors.centerIn:
                                parent

                            text: "♪"

                            color: AppTheme.textSecondary

                            font.pixelSize: 42
                        }
                    }
                }

                /*
                 * Track information
                 */

                Item {
                    id: trackInfoArea

                    width:
                        parent.width -
                        currentCover.width -
                        parent.spacing

                    height: parent.height

                    Label {
                        id: playingLabel

                        anchors.top:
                            parent.top

                        anchors.left:
                            parent.left

                        anchors.right:
                            parent.right

                        text: "Сейчас играет"

                        color: AppTheme.textMuted

                        font.pixelSize: 12
                    }

                    Label {
                        id: currentTitle

                        anchors.top:
                            playingLabel.bottom

                        anchors.topMargin: 8

                        anchors.left:
                            parent.left

                        anchors.right:
                            parent.right

                        text:
                            root.hasController
                                ? root.controller.currentTrackTitle
                                : ""

                        color: AppTheme.textPrimary

                        font.pixelSize: 24
                        font.bold: true

                        elide:
                            Text.ElideRight
                    }

                    /*
                     * Artist
                     */

                    Label {
                        id: currentArtist

                        anchors.top:
                            currentTitle.bottom

                        anchors.topMargin: 6

                        anchors.left:
                            parent.left

                        width:
                            Math.min(
                                implicitWidth,
                                parent.width)

                        text:
                            root.hasController
                                ? root.controller.currentTrackArtist
                                : ""

                        color:
                            artistMouseArea.containsMouse
                                ? AppTheme.textPrimary
                                : AppTheme.textSecondary

                        font.pixelSize: 16

                        elide:
                            Text.ElideRight

                        font.underline:
                            artistMouseArea.containsMouse

                        MouseArea {
                            id: artistMouseArea

                            anchors.fill:
                                parent

                            hoverEnabled: true

                            enabled:
                                root.hasController &&
                                root.controller.currentTrackArtistId.length > 0

                            cursorShape:
                                enabled
                                    ? Qt.PointingHandCursor
                                    : Qt.ArrowCursor

                            onClicked: {
                                root.controller.loadArtist(
                                    root.controller.currentTrackArtistId)
                            }
                        }
                    }

                    /*
                     * Album
                     */

                    Label {
                        id: currentAlbum

                        anchors.top:
                            currentArtist.bottom

                        anchors.topMargin: 4

                        anchors.left:
                            parent.left

                        width:
                            Math.min(
                                implicitWidth,
                                parent.width)

                        visible:
                            root.hasController &&
                            root.controller.currentTrackAlbumTitle.length > 0

                        text:
                            root.hasController
                                ? root.controller.currentTrackAlbumTitle
                                : ""

                        color:
                            albumMouseArea.containsMouse
                                ? AppTheme.textPrimary
                                : AppTheme.textMuted

                        font.pixelSize: 13

                        elide:
                            Text.ElideRight

                        font.underline:
                            albumMouseArea.containsMouse

                        MouseArea {
                            id: albumMouseArea

                            anchors.fill:
                                parent

                            hoverEnabled: true

                            enabled:
                                root.hasController &&
                                root.controller.currentTrackAlbumId.length > 0

                            cursorShape:
                                enabled
                                    ? Qt.PointingHandCursor
                                    : Qt.ArrowCursor

                            onClicked: {
                                root.controller.loadAlbum(
                                    root.controller.currentTrackAlbumId)
                            }
                        }
                    }

                    /*
                     * Duration
                     */

                    Label {
                        id: currentDuration

                        anchors.top:
                            currentAlbum.visible
                                ? currentAlbum.bottom
                                : currentArtist.bottom

                        anchors.topMargin: 6

                        anchors.left:
                            parent.left

                        text:
                                root.hasController &&
                            root.controller.duration > 0
                            ? root.formatDuration(
                                root.controller.duration)
                            : ""

                        color: AppTheme.textMuted

                        font.pixelSize: 12
                    }

                    /*
                     * Play / Pause
                     */

                    Rectangle {
                        id: playPauseButton

                        width: 80
                        height: 80

                        radius:
                            width / 2

                        anchors.horizontalCenter:
                            parent.horizontalCenter

                        anchors.verticalCenter:
                            parent.verticalCenter

                        color:
                            playPauseMouseArea.containsMouse
                                ? "#1f1f1f"
                                : "#2b2b2b"

                        scale:
                            playPauseMouseArea.pressed
                                ? 0.94
                                : 1.0

                        Behavior on scale {
                            NumberAnimation {
                                duration: 100
                            }
                        }

                        Label {
                            anchors.centerIn:
                                parent

                            text:
                                    root.hasController &&
                                root.controller.playing
                                ? "❚❚"
                                : "▶"

                            color: "#ffffff"

                            font.pixelSize:
                                    root.hasController &&
                                root.controller.playing
                                ? 28
                                : 30

                            font.bold: true

                            leftPadding:
                                    root.hasController &&
                                root.controller.playing
                                ? 0
                                : 5
                        }

                        MouseArea {
                            id: playPauseMouseArea

                            anchors.fill:
                                parent

                            hoverEnabled: true

                            cursorShape:
                                Qt.PointingHandCursor

                            enabled:
                                root.hasController &&
                                root.hasSelectedTrack

                            onClicked: {
                                if (
                                    root.controller.playing
                                ) {
                                    root.controller.pause()
                                } else {
                                    root.controller.play()
                                }
                            }
                        }
                    }
                }
            }
        }

        /*
         * --------------------------------------------------------
         * Track list title
         * --------------------------------------------------------
         */

        Label {
            id: nextTitle

            text:
                root.hasSelectedTrack
                    ? "Далее"
                    : "Треки"

            color: AppTheme.textPrimary

            font.pixelSize: 20
            font.bold: true

            height: 26
        }

        /*
         * --------------------------------------------------------
         * Track list
         * --------------------------------------------------------
         */

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
                root.hasController
                    ? root.controller.myWaveModel
                    : null

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
                        fullList.ScrollBar.vertical.visible
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
                        anchors.fill: parent

                        radius: 6

                        color: AppTheme.surface

                        visible:
                            trackCover.status !==
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

                        color: AppTheme.textPrimary

                        font.pixelSize: 14
                        font.bold: true

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        width: parent.width

                        text:
                            artist

                        color: AppTheme.textSecondary

                        font.pixelSize: 12

                        elide:
                            Text.ElideRight
                    }

                    Label {
                        visible:
                            album.length > 0

                        width: parent.width

                        text:
                            album

                        color: AppTheme.textMuted

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

                    color: AppTheme.textSecondary

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

                        root.controller.selectMyWaveTrack(
                            index)
                    }
                }
            }
        }
    }

    /*
     * ============================================================
     * HELPERS
     * ============================================================
     */

    function isCurrentTrack(
        title,
        artist)
    {
        if (
            !root.hasController
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