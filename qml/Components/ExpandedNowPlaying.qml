import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller
    property string playbackSourceTitle: ""
    property string playbackSourceType: ""

    signal closed()

    anchors.fill: parent


    // ============================================================
    // Dynamic Player Accent
    // ============================================================

    readonly property bool hasPlayerAccent:
        root.controller !== null &&
        root.controller !== undefined &&
        root.controller.playerAccent !== undefined &&
        root.controller.playerAccent !== null &&
        root.controller.playerAccent.valid

    readonly property color playerAccent:
        root.hasPlayerAccent
            ? root.controller.playerAccent
            : AppTheme.accent


    // ============================================================
    // Background
    // ============================================================

    Rectangle {
        anchors.fill: parent
        color: AppTheme.background
    }

    Rectangle {
        anchors.fill: parent

        color:
            root.hasPlayerAccent
                ? Qt.rgba(
                    root.playerAccent.r,
                    root.playerAccent.g,
                    root.playerAccent.b,
                    0.24
                )
                : "transparent"

        Behavior on color {
            ColorAnimation {
                duration: 450
                easing.type: Easing.OutCubic
            }
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        height: parent.height * 0.45

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color:
                    root.hasPlayerAccent
                        ? Qt.rgba(
                            root.playerAccent.r,
                            root.playerAccent.g,
                            root.playerAccent.b,
                            0.22
                        )
                        : "transparent"
            }

            GradientStop {
                position: 1.0
                color: "transparent"
            }
        }
    }


    // ============================================================
    // Header
    // ============================================================

    Item {
        id: header

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        anchors.leftMargin: 36
        anchors.rightMargin: 36
        anchors.topMargin: 22

        height: 40


        // --------------------------------------------------------
        // Collapse
        // --------------------------------------------------------

        Item {
            id: closeButton

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            width: 42
            height: 42


            Rectangle {
                anchors.fill: parent

                radius: width / 2

                color:
                    closeMouseArea.containsMouse
                        ? Qt.rgba(
                            root.playerAccent.r,
                            root.playerAccent.g,
                            root.playerAccent.b,
                            0.16
                        )
                        : Qt.rgba(
                            root.playerAccent.r,
                            root.playerAccent.g,
                            root.playerAccent.b,
                            0.12
                        )

                Behavior on color {
                    ColorAnimation {
                        duration: 140
                        easing.type: Easing.OutCubic
                    }
                }
            }


            Text {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -3

                text: "⌄"

                color:
                    Qt.rgba(
                        root.playerAccent.r,
                        root.playerAccent.g,
                        root.playerAccent.b,
                        0.85
                    )

                font.pixelSize: 24
                font.weight: Font.Normal
            }


            MouseArea {
                id: closeMouseArea

                anchors.fill: parent

                hoverEnabled: true

                cursorShape: Qt.PointingHandCursor

                onClicked:
                    root.closed()
            }
        }
    }


    // ============================================================
    // Main content
    // ============================================================

    Row {
        id: mainRow

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.bottom: parent.bottom

        anchors.leftMargin: 72
        anchors.rightMargin: 72
        anchors.topMargin: 20
        anchors.bottomMargin: 38

        spacing: 58


        // ========================================================
        // Player
        // ========================================================

        Item {
            id: playerPanel

            width: mainRow.width * 0.58
            height: mainRow.height


            // ----------------------------------------------------
            // Artwork
            // ----------------------------------------------------

            Image {
                id: artwork

                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter

                width:
                    Math.min(
                        440,
                        parent.width * 0.62,
                        parent.height * 0.58
                    )

                height: width

                fillMode: Image.PreserveAspectCrop

                asynchronous: true
                cache: true

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

                    if (uri.length === 0) {
                        return ""
                    }

                    return "image://yandex/" + uri
                }


                Rectangle {
                    anchors.fill: parent

                    visible:
                        artwork.status !== Image.Ready

                    radius: 12

                    color:
                        AppTheme.artworkPlaceholder


                    Text {
                        anchors.centerIn: parent

                        text: "♪"

                        color:
                            AppTheme.textMuted

                        font.pixelSize: 52
                    }
                }
            }


            // ----------------------------------------------------
            // Track info
            // ----------------------------------------------------

            Column {
                id: trackInfo

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: artwork.bottom

                anchors.topMargin: 18

                spacing: 4


                Text {
                    width: parent.width

                    text:
                            root.controller !== null &&
                        root.controller !== undefined
                        ? root.controller.currentTrackTitle
                        : ""

                    color:
                        AppTheme.textPrimary

                    font.pixelSize: 22
                    font.weight: Font.DemiBold

                    horizontalAlignment:
                        Text.AlignHCenter

                    elide:
                        Text.ElideRight
                }


                EntityLink {
                    anchors.horizontalCenter:
                        parent.horizontalCenter

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

                    entityType: "artist"

                    controller:
                        root.controller
                }


                EntityLink {
                    anchors.horizontalCenter:
                        parent.horizontalCenter

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

                    entityType: "album"

                    controller:
                        root.controller
                }
            }


            // ----------------------------------------------------
            // Progress
            // ----------------------------------------------------

            Item {
                id: progressArea

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: trackInfo.bottom

                anchors.topMargin: 14

                height: 30


                Rectangle {
                    id: progressBackground

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    height: 4
                    radius: 2

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
                                Math.min(1, ratio)
                            )

                        return progressBackground.width * ratio
                    }

                    height: 4
                    radius: 2

                    color:
                        root.playerAccent
                }


                MouseArea {
                    anchors.fill: parent

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
                                    Math.min(1, ratio)
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
                    anchors.left: parent.left
                    anchors.top: progressBackground.bottom
                    anchors.topMargin: 5

                    text:
                        formatTime(
                                root.controller !== null &&
                                root.controller !== undefined
                                ? root.controller.position
                                : 0
                        )

                    color:
                        AppTheme.textMuted

                    font.pixelSize: 11
                }


                Text {
                    anchors.right: parent.right
                    anchors.top: progressBackground.bottom
                    anchors.topMargin: 5

                    text:
                        formatTime(
                                root.controller !== null &&
                                root.controller !== undefined
                                ? root.controller.duration
                                : 0
                        )

                    color:
                        AppTheme.textMuted

                    font.pixelSize: 11
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

                anchors.topMargin: 14

                spacing: 14


                ControlButton {
                    width: 40
                    height: 40

                    text: "↶"

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
                    width: 44
                    height: 44

                    text: "‹"
                    fontSize: 26

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
                    width: 56
                    height: 56

                    radius: width / 2

                    color:
                        root.playerAccent


                    Text {
                        anchors.centerIn: parent

                        text:
                                root.controller !== null &&
                            root.controller !== undefined &&
                            root.controller.playing
                            ? "Ⅱ"
                            : "▶"

                        color:
                            AppTheme.background

                        font.pixelSize: 21
                    }


                    MouseArea {
                        anchors.fill: parent

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
                    width: 44
                    height: 44

                    text: "›"
                    fontSize: 26

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
                    width: 40
                    height: 40

                    text: "↻"

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
        // Queue
        // ========================================================

        Item {
            id: queuePanel

            width: mainRow.width * 0.42
            height: mainRow.height


            // ----------------------------------------------------
            // Playback source
            // ----------------------------------------------------

            Text {
                id: sourceTitle

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top

                text:
                    root.playbackSourceTitle

                color:
                    AppTheme.textPrimary

                font.pixelSize: 20
                font.weight: Font.DemiBold

                elide:
                    Text.ElideRight

                visible:
                    root.playbackSourceTitle.length > 0
            }


            // ----------------------------------------------------
            // Previous track
            // ----------------------------------------------------

            Rectangle {
                id: previousTrackCard

                anchors.left: parent.left
                anchors.right: parent.right

                anchors.top:
                    sourceTitle.visible
                        ? sourceTitle.bottom
                        : parent.top

                anchors.topMargin:
                    sourceTitle.visible
                        ? 16
                        : 0

                height: 68

                radius: 10

                color:
                    Qt.rgba(
                        root.playerAccent.r,
                        root.playerAccent.g,
                        root.playerAccent.b,
                        previousMouseArea.containsMouse
                            ? 0.12
                            : 0.055
                    )

                border.width: 1

                border.color:
                    Qt.rgba(
                        root.playerAccent.r,
                        root.playerAccent.g,
                        root.playerAccent.b,
                        0.10
                    )

                opacity: 0.72

                Behavior on color {
                    ColorAnimation {
                        duration: 140
                        easing.type: Easing.OutCubic
                    }
                }


                Image {
                    id: previousArtwork

                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 9

                    width: 48
                    height: 48

                    fillMode:
                        Image.PreserveAspectCrop

                    asynchronous: true
                    cache: true

                    source: {
                        var data =
                            root.previousTrackData()

                        var uri =
                            String(
                                data.coverUri || ""
                            )

                        return uri.length > 0
                            ? "image://yandex/" + uri
                            : ""
                    }


                    Rectangle {
                        anchors.fill: parent

                        visible:
                            previousArtwork.status !==
                            Image.Ready

                        radius: 6

                        color:
                            AppTheme.artworkPlaceholder


                        Text {
                            anchors.centerIn: parent

                            text: "♪"

                            color:
                                AppTheme.textMuted

                            font.pixelSize: 18
                        }
                    }
                }


                Column {
                    anchors.left:
                        previousArtwork.right

                    anchors.right: parent.right
                    anchors.verticalCenter:
                        parent.verticalCenter

                    anchors.leftMargin: 12
                    anchors.rightMargin: 12

                    spacing: 2


                    Text {
                        width: parent.width

                        text:
                            root.previousTrackData().title || ""

                        color:
                            AppTheme.textSecondary

                        font.pixelSize: 13

                        elide:
                            Text.ElideRight
                    }


                    Text {
                        width: parent.width

                        text:
                            root.previousTrackData().artist || ""

                        color:
                            AppTheme.textMuted

                        font.pixelSize: 12

                        elide:
                            Text.ElideRight
                    }
                }


                MouseArea {
                    id: previousMouseArea

                    anchors.fill: parent

                    hoverEnabled: true

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

                        var index =
                            root.currentTrackIndex()

                        if (index > 0) {
                            root.controller.selectPlaylistTrack(
                                index - 1
                            )
                        }
                    }
                }


                visible:
                    root.hasPreviousTrack()
            }


            // ----------------------------------------------------
            // Queue title
            // ----------------------------------------------------

            Text {
                id: queueTitle

                anchors.left: parent.left
                anchors.right: parent.right

                anchors.top:
                    previousTrackCard.visible
                        ? previousTrackCard.bottom
                        : (
                            sourceTitle.visible
                                ? sourceTitle.bottom
                                : parent.top
                        )

                anchors.topMargin:
                    previousTrackCard.visible
                        ? 22
                        : (
                            sourceTitle.visible
                                ? 22
                                : 0
                        )

                text:
                    "Далее в очереди"

                color:
                    AppTheme.textPrimary

                font.pixelSize: 19
                font.weight: Font.DemiBold
            }


            // ----------------------------------------------------
            // Up Next
            // ----------------------------------------------------

            ListView {
                id: queueView

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: queueTitle.bottom
                anchors.bottom: parent.bottom

                anchors.topMargin: 10

                spacing: 5

                clip: true

                model:
                    upNextModel


                delegate: Item {
                    id: queueItem

                    width: queueView.width
                    height: 60


                    Rectangle {
                        anchors.fill: parent

                        radius: 8

                        color:
                            queueMouse.containsMouse
                                ? Qt.rgba(
                                    root.playerAccent.r,
                                    root.playerAccent.g,
                                    root.playerAccent.b,
                                    0.14
                                )
                                : "transparent"

                        Behavior on color {
                            ColorAnimation {
                                duration: 140
                                easing.type: Easing.OutCubic
                            }
                        }
                    }


                    Image {
                        id: queueArtwork

                        anchors.left: parent.left
                        anchors.verticalCenter:
                            parent.verticalCenter

                        width: 46
                        height: 46

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous: true
                        cache: true

                        source:
                                String(
                                    model.coverUri || ""
                                ).length > 0
                            ? "image://yandex/" +
                            model.coverUri
                            : ""


                        Rectangle {
                            anchors.fill: parent

                            visible:
                                queueArtwork.status !==
                                Image.Ready

                            radius: 6

                            color:
                                AppTheme.artworkPlaceholder


                            Text {
                                anchors.centerIn: parent

                                text: "♪"

                                color:
                                    AppTheme.textMuted

                                font.pixelSize: 18
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

                        anchors.leftMargin: 12
                        anchors.rightMargin: 8

                        spacing: 2


                        Text {
                            width: parent.width

                            text:
                                model.title || ""

                            color:
                                AppTheme.textPrimary

                            font.pixelSize: 13

                            elide:
                                Text.ElideRight
                        }


                        EntityLink {
                            text:
                                model.artist || ""

                            entityId:
                                model.artistId || ""

                            entityType:
                                "artist"

                            controller:
                                root.controller
                        }
                    }


                    Text {
                        id: durationText

                        anchors.right: parent.right
                        anchors.rightMargin: 8
                        anchors.verticalCenter:
                            parent.verticalCenter

                        text:
                            formatTime(
                                Number(
                                    model.durationMs || 0
                                )
                            )

                        color:
                            AppTheme.textMuted

                        font.pixelSize: 11
                    }


                    MouseArea {
                        id: queueMouse

                        anchors.fill: parent

                        hoverEnabled: true

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

                            root.controller.selectPlaylistTrack(
                                Number(
                                    model.sourceIndex
                                )
                            )
                        }
                    }
                }
            }
        }
    }


    // ============================================================
    // Queue model
    // ============================================================

    ListModel {
        id: upNextModel
    }


    // ============================================================
    // Controller connections
    // ============================================================

    Connections {
        target:
            root.controller

        function onCurrentTrackChanged()
        {
            root.rebuildUpNext()
        }

        function onCurrentPlaylistChanged()
        {
            root.rebuildUpNext()
        }
    }


    Connections {
        target:
                root.controller !== null &&
            root.controller !== undefined
            ? root.controller.playlistModel
            : null

        function onRowsInserted()
        {
            root.rebuildUpNext()
        }

        function onRowsRemoved()
        {
            root.rebuildUpNext()
        }

        function onModelReset()
        {
            root.rebuildUpNext()
        }

        function onDataChanged()
        {
            root.rebuildUpNext()
        }
    }


    Component.onCompleted:
    {
        root.rebuildUpNext()
    }


    // ============================================================
    // Queue helpers
    // ============================================================

    function currentTrackIndex()
    {
        if (
            root.controller === null ||
            root.controller === undefined
        ) {
            return -1
        }

        var playlist =
            root.controller.playlistModel

        if (
            playlist === null ||
            playlist === undefined
        ) {
            return -1
        }

        var count =
            playlist.rowCount()

        var currentId =
            String(
                root.controller.currentTrackId ||
                ""
            )

        for (
            var i = 0;
            i < count;
            ++i
        ) {
            var index =
                playlist.index(i, 0)

            var id =
                String(
                    playlist.data(
                        index,
                        Qt.UserRole + 1
                    )
                )

            if (id === currentId) {
                return i
            }
        }

        return -1
    }


    function hasPreviousTrack()
    {
        return root.currentTrackIndex() > 0
    }


    function previousTrackData()
    {
        var empty = {
            title: "",
            artist: "",
            artistId: "",
            coverUri: "",
            durationMs: 0
        }

        if (
            !root.hasPreviousTrack()
        ) {
            return empty
        }

        var playlist =
            root.controller.playlistModel

        var index =
            root.currentTrackIndex()

        if (
            playlist === null ||
            playlist === undefined ||
            index <= 0
        ) {
            return empty
        }

        var modelIndex =
            playlist.index(
                index - 1,
                0
            )

        return {
            title:
                String(
                    playlist.data(
                        modelIndex,
                        Qt.UserRole + 2
                    ) || ""
                ),

            artist:
                String(
                    playlist.data(
                        modelIndex,
                        Qt.UserRole + 3
                    ) || ""
                ),

            artistId:
                String(
                    playlist.data(
                        modelIndex,
                        Qt.UserRole + 4
                    ) || ""
                ),

            coverUri:
                String(
                    playlist.data(
                        modelIndex,
                        Qt.UserRole + 7
                    ) || ""
                ),

            durationMs:
                Number(
                    playlist.data(
                        modelIndex,
                        Qt.UserRole + 8
                    ) || 0
                )
        }
    }


    function rebuildUpNext()
    {
        upNextModel.clear()

        if (
            root.controller === null ||
            root.controller === undefined
        ) {
            return
        }

        var playlist =
            root.controller.playlistModel

        if (
            playlist === null ||
            playlist === undefined
        ) {
            return
        }

        var count =
            playlist.rowCount()

        if (
            count <= 0
        ) {
            return
        }

        var currentIndex =
            root.currentTrackIndex()

        if (
            currentIndex < 0
        ) {
            return
        }

        for (
            var j = currentIndex + 1;
            j < count;
            ++j
        ) {
            var sourceIndex =
                playlist.index(j, 0)

            var title =
                String(
                    playlist.data(
                        sourceIndex,
                        Qt.UserRole + 2
                    ) || ""
                )

            var artist =
                String(
                    playlist.data(
                        sourceIndex,
                        Qt.UserRole + 3
                    ) || ""
                )

            var artistId =
                String(
                    playlist.data(
                        sourceIndex,
                        Qt.UserRole + 4
                    ) || ""
                )

            var coverUri =
                String(
                    playlist.data(
                        sourceIndex,
                        Qt.UserRole + 7
                    ) || ""
                )

            var durationMs =
                Number(
                    playlist.data(
                        sourceIndex,
                        Qt.UserRole + 8
                    ) || 0
                )

            upNextModel.append({
                sourceIndex: j,
                title: title,
                artist: artist,
                artistId: artistId,
                coverUri: coverUri,
                durationMs: durationMs
            })
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

        return minutes +
            ":" +
            (
                    seconds < 10
                    ? "0"
                    : ""
            ) +
            seconds
    }


    // ============================================================
    // Control button
    // ============================================================

    component ControlButton: Rectangle {

        property string text: ""
        property int fontSize: 21

        signal clicked()


        radius:
            width / 2

        color:
            mouseArea.containsMouse
                ? Qt.rgba(
                    root.playerAccent.r,
                    root.playerAccent.g,
                    root.playerAccent.b,
                    0.14
                )
                : "transparent"

        Behavior on color {
            ColorAnimation {
                duration: 140
                easing.type: Easing.OutCubic
            }
        }


        Text {
            anchors.centerIn: parent

            text:
                parent.text

            color:
                AppTheme.textPrimary

            font.pixelSize:
                parent.fontSize
        }


        MouseArea {
            id: mouseArea

            anchors.fill: parent

            hoverEnabled: true

            cursorShape:
                Qt.PointingHandCursor

            onClicked:
                parent.clicked()
        }
    }
}