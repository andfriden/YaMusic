import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller
    property var queueModel: null

    property string playbackSourceTitle: ""
    property string playbackSourceType: ""

    signal closed()

    anchors.fill: parent

    // ============================================================
    // Navigation
    // ============================================================

    Connections {
        target: root.controller

        function onCurrentArtistChanged() {
            root.closed()
        }

        function onCurrentAlbumChanged() {
            root.closed()
        }
    }

    // ============================================================
    // Background
    // ============================================================

    Rectangle {
        anchors.fill: parent

        color:
            AppTheme.background
    }


    // ============================================================
    // Close
    // ============================================================

    Rectangle {
        id: closeButton

        width: 42
        height: 42

        anchors.top:
            parent.top

        anchors.right:
            parent.right

        anchors.topMargin:
            24

        anchors.rightMargin:
            24

        radius:
            width / 2

        color:
            closeMouseArea.containsMouse
                ? AppTheme.panelHover
                : AppTheme.panel

        border.width:
            1

        border.color:
            AppTheme.borderSubtle


        Text {
            anchors.centerIn:
                parent

            text:
                "×"

            color:
                AppTheme.textPrimary

            font.pixelSize:
                28

            font.weight:
                Font.Light
        }


        MouseArea {
            id: closeMouseArea

            anchors.fill:
                parent

            hoverEnabled:
                true

            cursorShape:
                Qt.PointingHandCursor

            onClicked:
                root.closed()
        }
    }


    // ============================================================
    // Main content
    // ============================================================

    Row {
        id: contentRow

        anchors.left:
            parent.left

        anchors.right:
            parent.right

        anchors.top:
            parent.top

        anchors.bottom:
            parent.bottom

        anchors.leftMargin:
            64

        anchors.rightMargin:
            64

        anchors.topMargin:
            64

        anchors.bottomMargin:
            48

        spacing:
            64


        // ========================================================
        // Current track
        // ========================================================

        Item {
            id: currentTrackPanel

            width:
                contentRow.width * 0.56

            height:
                contentRow.height


            // ----------------------------------------------------
            // Artwork
            // ----------------------------------------------------

            Image {
                id: artwork

                anchors.top:
                    parent.top

                anchors.horizontalCenter:
                    parent.horizontalCenter

                width:
                    Math.min(
                        parent.width * 0.72,
                        parent.height * 0.62
                    )

                height:
                    width

                fillMode:
                    Image.PreserveAspectFit

                asynchronous:
                    true

                cache:
                    true

                source: {
                    if (
                        root.controller === null ||
                        root.controller === undefined
                    ) {
                        return ""
                    }

                    var uri =
                        String(
                            root.controller.currentTrackCoverUri ||
                            ""
                        )

                    if (
                        uri.length === 0
                    ) {
                        return ""
                    }

                    return "image://yandex/" + uri
                }


                Rectangle {
                    anchors.fill:
                        parent

                    visible:
                        artwork.status !== Image.Ready

                    color:
                        AppTheme.artworkPlaceholder

                    radius:
                        8


                    Text {
                        anchors.centerIn:
                            parent

                        text:
                            "♪"

                        color:
                            AppTheme.textMuted

                        font.pixelSize:
                            56
                    }
                }
            }


            // ----------------------------------------------------
            // Track metadata
            // ----------------------------------------------------

            Column {
                id: trackInfo

                anchors.left:
                    parent.left

                anchors.right:
                    parent.right

                anchors.top:
                    artwork.bottom

                anchors.topMargin:
                    28

                spacing:
                    7


                Text {
                    width:
                        parent.width

                    text:
                            root.controller !== null &&
                        root.controller !== undefined
                        ? root.controller.currentTrackTitle
                        : ""

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        28

                    font.weight:
                        Font.DemiBold

                    elide:
                        Text.ElideRight
                }


                // ------------------------------------------------
                // Artist
                // ------------------------------------------------

                EntityLink {
                    text:
                            root.controller !== null &&
                        root.controller !== undefined
                        ? root.controller.currentTrackArtist
                        : ""

                    entityId:
                            root.controller !== null &&
                        root.controller !== undefined
                        ? root.controller.currentTrackArtistId
                        : ""

                    entityType:
                        "artist"

                    controller:
                        root.controller
                }


                // ------------------------------------------------
                // Album
                // ------------------------------------------------

                EntityLink {
                    text:
                            root.controller !== null &&
                        root.controller !== undefined
                        ? root.controller.currentTrackAlbumTitle
                        : ""

                    entityId:
                            root.controller !== null &&
                        root.controller !== undefined
                        ? root.controller.currentTrackAlbumId
                        : ""

                    entityType:
                        "album"

                    controller:
                        root.controller
                }
            }


            // ----------------------------------------------------
            // Progress
            // ----------------------------------------------------

            Item {
                id: progressArea

                anchors.left:
                    parent.left

                anchors.right:
                    parent.right

                anchors.top:
                    trackInfo.bottom

                anchors.topMargin:
                    28

                height:
                    38


                Rectangle {
                    id: progressBackground

                    anchors.left:
                        parent.left

                    anchors.right:
                        parent.right

                    anchors.verticalCenter:
                        parent.verticalCenter

                    height:
                        4

                    radius:
                        2

                    color:
                        AppTheme.divider
                }


                Rectangle {
                    anchors.left:
                        progressBackground.left

                    anchors.verticalCenter:
                        progressBackground.verticalCenter

                    width: {
                        if (
                            root.controller === null ||
                            root.controller === undefined ||
                            Number(root.controller.duration) <= 0
                        ) {
                            return 0
                        }

                        var ratio =
                            Number(root.controller.position) /
                            Number(root.controller.duration)

                        ratio =
                            Math.max(
                                0,
                                Math.min(
                                    1,
                                    ratio
                                )
                            )

                        return progressBackground.width *
                            ratio
                    }

                    height:
                        4

                    radius:
                        2

                    color:
                        AppTheme.accent
                }


                MouseArea {
                    anchors.fill:
                        parent

                    cursorShape:
                        Qt.PointingHandCursor

                    onClicked:
                            function(mouse)
                        {
                            if (
                                root.controller === null ||
                                root.controller === undefined ||
                                Number(root.controller.duration) <= 0
                            ) {
                                return
                            }

                            var ratio =
                                mouse.x / width

                            ratio =
                                Math.max(
                                    0,
                                    Math.min(
                                        1,
                                        ratio
                                    )
                                )

                            root.controller.seek(
                                Math.round(
                                    Number(
                                        root.controller.duration
                                    ) * ratio
                                )
                            )
                        }
                }


                Text {
                    anchors.left:
                        parent.left

                    anchors.top:
                        progressBackground.bottom

                    anchors.topMargin:
                        8

                    text:
                        formatTime(
                                root.controller !== null &&
                                root.controller !== undefined
                                ? root.controller.position
                                : 0
                        )

                    color:
                        AppTheme.textMuted

                    font.pixelSize:
                        12
                }


                Text {
                    anchors.right:
                        parent.right

                    anchors.top:
                        progressBackground.bottom

                    anchors.topMargin:
                        8

                    text:
                        formatTime(
                                root.controller !== null &&
                                root.controller !== undefined
                                ? root.controller.duration
                                : 0
                        )

                    color:
                        AppTheme.textMuted

                    font.pixelSize:
                        12
                }
            }


            // ----------------------------------------------------
            // Playback controls
            // ----------------------------------------------------

            Row {
                id: controls

                anchors.horizontalCenter:
                    parent.horizontalCenter

                anchors.top:
                    progressArea.bottom

                anchors.topMargin:
                    26

                spacing:
                    26


                ControlButton {
                    width:
                        42

                    height:
                        42

                    text:
                        "↶"

                    onClicked:
                    {
                        if (
                            root.controller !== null &&
                            root.controller !== undefined
                        ) {
                            root.controller.cycleRepeat()
                        }
                    }
                }


                ControlButton {
                    width:
                        50

                    height:
                        50

                    text:
                        "‹"

                    fontSize:
                        28

                    onClicked:
                    {
                        if (
                            root.controller !== null &&
                            root.controller !== undefined
                        ) {
                            root.controller.previous()
                        }
                    }
                }


                Rectangle {
                    width:
                        64

                    height:
                        64

                    radius:
                        width / 2

                    color:
                        AppTheme.accent


                    Text {
                        anchors.centerIn:
                            parent

                        text:
                                root.controller !== null &&
                            root.controller !== undefined &&
                            root.controller.playing
                            ? "Ⅱ"
                            : "▶"

                        color:
                            AppTheme.background

                        font.pixelSize:
                            25
                    }


                    MouseArea {
                        anchors.fill:
                            parent

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked:
                        {
                            if (
                                root.controller === null ||
                                root.controller === undefined
                            ) {
                                return
                            }

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


                ControlButton {
                    width:
                        50

                    height:
                        50

                    text:
                        "›"

                    fontSize:
                        28

                    onClicked:
                    {
                        if (
                            root.controller !== null &&
                            root.controller !== undefined
                        ) {
                            root.controller.next()
                        }
                    }
                }


                ControlButton {
                    width:
                        42

                    height:
                        42

                    text:
                        "↻"

                    onClicked:
                    {
                        if (
                            root.controller !== null &&
                            root.controller !== undefined
                        ) {
                            root.controller.toggleShuffle()
                        }
                    }
                }
            }
        }


        // ========================================================
        // Right side
        // ========================================================

        Item {
            id: queuePanel

            width:
                contentRow.width * 0.44

            height:
                contentRow.height


            // ----------------------------------------------------
            // Playback source
            // ----------------------------------------------------

            Column {
                id: sourceHeader

                anchors.left:
                    parent.left

                anchors.right:
                    parent.right

                anchors.top:
                    parent.top

                spacing:
                    6


                Text {
                    text:
                            root.playbackSourceType.length > 0
                        ? "Сейчас играет из "
                        + root.playbackSourceType
                        : "Сейчас играет"

                    color:
                        AppTheme.textTertiary

                    font.pixelSize:
                        14
                }


                Text {
                    width:
                        parent.width

                    text:
                        root.playbackSourceTitle

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        22

                    font.weight:
                        Font.DemiBold

                    elide:
                        Text.ElideRight
                }
            }


            // ----------------------------------------------------
            // Current track card
            // ----------------------------------------------------

            Rectangle {
                id: currentTrackCard

                anchors.left:
                    parent.left

                anchors.right:
                    parent.right

                anchors.top:
                    sourceHeader.bottom

                anchors.topMargin:
                    24

                height:
                    82

                radius:
                    10

                color:
                    AppTheme.panel

                border.width:
                    1

                border.color:
                    AppTheme.borderSubtle


                Image {
                    id: currentTrackArtwork

                    anchors.left:
                        parent.left

                    anchors.verticalCenter:
                        parent.verticalCenter

                    anchors.leftMargin:
                        12

                    width:
                        58

                    height:
                        58

                    fillMode:
                        Image.PreserveAspectCrop

                    asynchronous:
                        true

                    cache:
                        true

                    source: {
                        if (
                            root.controller === null ||
                            root.controller === undefined
                        ) {
                            return ""
                        }

                        var uri =
                            String(
                                root.controller.currentTrackCoverUri ||
                                ""
                            )

                        if (
                            uri.length === 0
                        ) {
                            return ""
                        }

                        return "image://yandex/" + uri
                    }


                    Rectangle {
                        anchors.fill:
                            parent

                        visible:
                            currentTrackArtwork.status !==
                            Image.Ready

                        radius:
                            7

                        color:
                            AppTheme.artworkPlaceholder


                        Text {
                            anchors.centerIn:
                                parent

                            text:
                                "♪"

                            color:
                                AppTheme.textMuted

                            font.pixelSize:
                                22
                        }
                    }
                }


                Column {
                    anchors.left:
                        currentTrackArtwork.right

                    anchors.right:
                        parent.right

                    anchors.verticalCenter:
                        parent.verticalCenter

                    anchors.leftMargin:
                        14

                    anchors.rightMargin:
                        14

                    spacing:
                        4


                    Text {
                        width:
                            parent.width

                        text:
                                root.controller !== null &&
                            root.controller !== undefined
                            ? root.controller.currentTrackTitle
                            : ""

                        color:
                            AppTheme.textPrimary

                        font.pixelSize:
                            15

                        font.weight:
                            Font.DemiBold

                        elide:
                            Text.ElideRight
                    }


                    EntityLink {
                        text:
                                root.controller !== null &&
                            root.controller !== undefined
                            ? root.controller.currentTrackArtist
                            : ""

                        entityId:
                                root.controller !== null &&
                            root.controller !== undefined
                            ? root.controller.currentTrackArtistId
                            : ""

                        entityType:
                            "artist"

                        controller:
                            root.controller
                    }
                }
            }


            // ----------------------------------------------------
            // Queue title
            // ----------------------------------------------------

            Text {
                id: queueTitle

                anchors.left:
                    parent.left

                anchors.right:
                    parent.right

                anchors.top:
                    currentTrackCard.bottom

                anchors.topMargin:
                    28

                text:
                    "Далее в очереди"

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    20

                font.weight:
                    Font.DemiBold
            }


            // ----------------------------------------------------
            // Queue
            // ----------------------------------------------------

            ListView {
                id: queueView

                anchors.left:
                    parent.left

                anchors.right:
                    parent.right

                anchors.top:
                    queueTitle.bottom

                anchors.bottom:
                    parent.bottom

                anchors.topMargin:
                    18

                spacing:
                    8

                clip:
                    true

                model:
                        root.controller !== null &&
                    root.controller !== undefined &&
                    root.controller.playlistModel !== null &&
                    root.controller.playlistModel !== undefined
                    ? root.controller.playlistModel
                    : root.queueModel


                delegate: Item {
                    id: queueItem

                    width:
                        queueView.width

                    height:
                        66


                    property string itemTitle:
                            model.title !== undefined
                        ? String(model.title)
                        : ""


                    property string itemArtist:
                            model.artist !== undefined
                        ? String(model.artist)
                        : ""


                    property string itemArtistId:
                            model.artistId !== undefined
                        ? String(model.artistId)
                        : ""


                    property string itemCover:
                            model.coverUri !== undefined
                        ? String(model.coverUri)
                        : ""


                    property int itemDurationMs:
                            model.durationMs !== undefined
                        ? Number(model.durationMs)
                        : 0


                    property int currentIndex: {
                        if (
                            root.controller === null ||
                            root.controller === undefined ||
                            root.controller.playlistModel === null ||
                            root.controller.playlistModel === undefined
                        ) {
                            return -1
                        }


                        var modelObject =
                            root.controller.playlistModel


                        var count =
                            modelObject.rowCount()


                        var currentId =
                            String(
                                root.controller.currentTrackId ||
                                ""
                            )


                        if (
                            currentId.length === 0
                        ) {
                            return -1
                        }


                        for (
                            var i = 0;
                            i < count;
                            ++i
                        ) {
                            var modelIndex =
                                modelObject.index(
                                    i,
                                    0
                                )


                            var id =
                                String(
                                    modelObject.data(
                                        modelIndex,
                                        Qt.UserRole + 1
                                    )
                                )


                            if (
                                id === currentId
                            ) {
                                return i
                            }
                        }


                        return -1
                    }


                    visible:
                        currentIndex >= 0 &&
                        index > currentIndex


                    Rectangle {
                        anchors.fill:
                            parent

                        radius:
                            8

                        color:
                            queueMouse.containsMouse
                                ? AppTheme.panelHover
                                : "transparent"
                    }


                    Image {
                        id: queueArtwork

                        anchors.left:
                            parent.left

                        anchors.verticalCenter:
                            parent.verticalCenter

                        width:
                            52

                        height:
                            52

                        source:
                                queueItem.itemCover.length > 0
                            ? "image://yandex/" +
                            queueItem.itemCover
                            : ""

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous:
                            true

                        cache:
                            true


                        Rectangle {
                            anchors.fill:
                                parent

                            visible:
                                queueArtwork.status !==
                                Image.Ready

                            radius:
                                6

                            color:
                                AppTheme.artworkPlaceholder


                            Text {
                                anchors.centerIn:
                                    parent

                                text:
                                    "♪"

                                color:
                                    AppTheme.textMuted

                                font.pixelSize:
                                    20
                            }
                        }
                    }


                    Column {
                        anchors.left:
                            queueArtwork.right

                        anchors.right:
                            durationText.left

                        anchors.verticalCenter:
                            parent.verticalCenter

                        anchors.leftMargin:
                            14

                        anchors.rightMargin:
                            10

                        spacing:
                            3


                        Text {
                            width:
                                parent.width

                            text:
                                queueItem.itemTitle

                            color:
                                AppTheme.textPrimary

                            font.pixelSize:
                                14

                            elide:
                                Text.ElideRight
                        }


                        EntityLink {
                            text:
                                queueItem.itemArtist

                            entityId:
                                queueItem.itemArtistId

                            entityType:
                                "artist"

                            controller:
                                root.controller
                        }
                    }


                    Text {
                        id: durationText

                        anchors.right:
                            parent.right

                        anchors.rightMargin:
                            10

                        anchors.verticalCenter:
                            parent.verticalCenter

                        text:
                            formatTime(
                                queueItem.itemDurationMs
                            )

                        color:
                            AppTheme.textMuted

                        font.pixelSize:
                            12
                    }


                    MouseArea {
                        id: queueMouse

                        anchors.fill:
                            parent

                        hoverEnabled:
                            true

                        cursorShape:
                            Qt.PointingHandCursor

                        onClicked:
                        {
                            if (
                                root.controller === null ||
                                root.controller === undefined
                            ) {
                                return
                            }


                            root.controller
                                .selectPlaylistTrack(
                                index
                            )
                        }
                    }
                }
            }
        }
    }


    // ============================================================
    // Helpers
    // ============================================================

    function formatTime(milliseconds)
    {
        var totalSeconds =
            Math.floor(
                Number(milliseconds) / 1000
            )


        if (
            !isFinite(totalSeconds) ||
            totalSeconds < 0
        ) {
            totalSeconds = 0
        }


        var minutes =
            Math.floor(
                totalSeconds / 60
            )


        var seconds =
            totalSeconds % 60


        return minutes
            + ":"
            + (
                    seconds < 10
                    ? "0"
                    : ""
            )
            + seconds
    }


    // ============================================================
    // Local control component
    // ============================================================

    component ControlButton: Rectangle {

        property string text: ""
        property int fontSize: 22

        signal clicked()

        radius:
            width / 2

        color:
            mouseArea.containsMouse
                ? AppTheme.panelHover
                : "transparent"


        Text {
            anchors.centerIn:
                parent

            text:
                parent.text

            color:
                AppTheme.textPrimary

            font.pixelSize:
                parent.fontSize
        }


        MouseArea {
            id: mouseArea

            anchors.fill:
                parent

            hoverEnabled:
                true

            cursorShape:
                Qt.PointingHandCursor

            onClicked:
                parent.clicked()
        }
    }
}