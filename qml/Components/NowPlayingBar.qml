import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Shapes


Item {
    id: root

    property var controller

    signal expandedRequested()


    // =============================================================
    // Player state
    // =============================================================

    readonly property bool hasTrack:
        root.controller !== null &&
        root.controller !== undefined &&
        String(
            root.controller.currentTrackTitle || ""
        ).length > 0

    readonly property bool loading:
        root.controller !== null &&
        root.controller !== undefined &&
        root.controller.playbackState === 1

    readonly property bool playing:
        root.controller !== null &&
        root.controller !== undefined &&
        root.controller.playing

    readonly property bool paused:
        root.controller !== null &&
        root.controller !== undefined &&
        root.controller.playbackState === 3


    // =============================================================
    // Dynamic Player Accent
    // =============================================================

    readonly property bool hasPlayerAccent:
        root.hasTrack &&
        root.controller !== null &&
        root.controller !== undefined &&
        root.controller.playerAccent !== undefined &&
        root.controller.playerAccent !== null &&
        root.controller.playerAccent.valid

    readonly property color playerAccent:
        root.hasPlayerAccent
            ? root.controller.playerAccent
            : AppTheme.accent


    // =============================================================
    // Volume
    // =============================================================

    readonly property bool hasVolume:
        root.controller !== null &&
        root.controller !== undefined

    readonly property real currentVolume:
        root.hasVolume
            ? Math.max(
                0,
                Math.min(
                    1,
                    Number(
                        root.controller.volume
                    )
                )
            )
            : 1


    // =============================================================
    // Main panel
    // =============================================================

    Rectangle {
        id: panel

        anchors.fill:
            parent

        radius:
            17

        color:
            root.hasPlayerAccent
                ? Qt.rgba(
                    root.playerAccent.r,
                    root.playerAccent.g,
                    root.playerAccent.b,
                    0.42
                )
                : AppTheme.panel

        border.width:
            1

        border.color:
            root.hasPlayerAccent
                ? Qt.rgba(
                    root.playerAccent.r,
                    root.playerAccent.g,
                    root.playerAccent.b,
                    0.18
                )
                : AppTheme.borderSubtle

        Behavior on color {
            ColorAnimation {
                duration:
                    350

                easing.type:
                    Easing.OutCubic
            }
        }

        Behavior on border.color {
            ColorAnimation {
                duration:
                    350

                easing.type:
                    Easing.OutCubic
            }
        }
    }


    // =============================================================
    // Artwork
    // =============================================================

    Rectangle {
        id: artworkFrame

        width:
            62

        height:
            62

        anchors.left:
            parent.left

        anchors.leftMargin:
            12

        anchors.verticalCenter:
            parent.verticalCenter

        radius:
            11

        color:
            AppTheme.surface

        clip:
            true


        Image {
            id: artwork

            anchors.fill:
                parent

            source:
                    root.hasTrack &&
                String(
                    root.controller.currentTrackCoverUri || ""
                ).length > 0
                ? "image://yandex/" +
                root.controller.currentTrackCoverUri
                : ""

            sourceSize:
                Qt.size(
                    124,
                    124
                )

            fillMode:
                Image.PreserveAspectCrop

            asynchronous:
                true

            cache:
                true

            smooth:
                true

            visible:
                status === Image.Ready
        }


        Rectangle {
            anchors.fill:
                parent

            color:
                AppTheme.surface

            visible:
                artwork.status !== Image.Ready


            Text {
                anchors.centerIn:
                    parent

                text:
                    root.loading
                        ? "..."
                        : "♪"

                color:
                    AppTheme.textSecondary

                font.pixelSize:
                    root.loading
                        ? 16
                        : 22
            }
        }
    }


    // =============================================================
    // Track information
    // =============================================================

    Item {
        id: trackInfo

        anchors.left:
            artworkFrame.right

        anchors.leftMargin:
            14

        anchors.right:
            playbackControls.left

        anchors.rightMargin:
            18

        anchors.verticalCenter:
            parent.verticalCenter

        height:
            40


        Text {
            id: titleLabel

            anchors.left:
                parent.left

            anchors.right:
                parent.right

            anchors.top:
                parent.top

            height:
                20

            text:
                root.hasTrack
                    ? String(
                        root.controller.currentTrackTitle || ""
                    )
                    : qsTr("Ничего не играет")

            color:
                AppTheme.textPrimary

            font.pixelSize:
                14

            font.weight:
                Font.DemiBold

            elide:
                Text.ElideRight

            verticalAlignment:
                Text.AlignVCenter
        }


        Item {
            id: artistArea

            anchors.left:
                parent.left

            anchors.right:
                parent.right

            anchors.top:
                titleLabel.bottom

            height:
                18


            Text {
                id: artistLabel

                width:
                    Math.min(
                        implicitWidth,
                        parent.width
                    )

                height:
                    parent.height

                text:
                    root.hasTrack
                        ? String(
                            root.controller.currentTrackArtist || ""
                        )
                        : ""

                color:
                    artistMouseArea.containsMouse
                        ? root.playerAccent
                        : AppTheme.textSecondary

                font.pixelSize:
                    11

                elide:
                    Text.ElideRight

                verticalAlignment:
                    Text.AlignVCenter
            }


            MouseArea {
                id: artistMouseArea

                anchors.left:
                    artistLabel.left

                anchors.top:
                    artistLabel.top

                width:
                    artistLabel.width

                height:
                    artistLabel.height

                hoverEnabled:
                    true

                enabled:
                    root.hasTrack &&
                    String(
                        root.controller.currentTrackArtistId || ""
                    ).length > 0

                cursorShape:
                    enabled
                        ? Qt.PointingHandCursor
                        : Qt.ArrowCursor

                onClicked: {
                    var artistId =
                        String(
                            root.controller.currentTrackArtistId || ""
                        )

                    if (
                        artistId.length === 0
                    ) {
                        return
                    }

                    root.controller.loadArtist(
                        artistId
                    )
                }
            }
        }
    }


    // =============================================================
    // Main playback controls
    // =============================================================

    Row {
        id: playbackControls

        anchors.horizontalCenter:
            parent.horizontalCenter

        anchors.verticalCenter:
            parent.verticalCenter

        anchors.verticalCenterOffset:
            -9

        spacing:
            5


        PlayerButton {
            width:
                44

            height:
                44

            text:
                "‹"

            fontSize:
                30

            enabled:
                root.hasTrack

            onClicked: {
                root.controller.previous()
            }
        }


        Rectangle {
            id: playButton

            width:
                52

            height:
                52

            radius:
                16

            color:
                root.hasTrack
                    ? root.playerAccent
                    : AppTheme.surface

            opacity:
                root.hasTrack
                    ? 1
                    : 0.65

            Behavior on color {
                ColorAnimation {
                    duration:
                        250

                    easing.type:
                        Easing.OutCubic
                }
            }


            Text {
                anchors.centerIn:
                    parent

                text:
                    root.loading
                        ? "..."
                        : root.playing
                            ? "Ⅱ"
                            : "▶"

                color:
                    root.hasTrack
                        ? AppTheme.background
                        : AppTheme.textMuted

                font.pixelSize:
                    root.loading
                        ? 12
                        : 17

                font.weight:
                    Font.DemiBold
            }


            MouseArea {
                anchors.fill:
                    parent

                enabled:
                    root.hasTrack &&
                    !root.loading

                hoverEnabled:
                    true

                cursorShape:
                    enabled
                        ? Qt.PointingHandCursor
                        : Qt.ArrowCursor

                onClicked: {
                    if (
                        root.playing
                    ) {
                        root.controller.pause()
                    } else {
                        root.controller.play()
                    }
                }
            }
        }


        PlayerButton {
            width:
                44

            height:
                44

            text:
                "›"

            fontSize:
                30

            enabled:
                root.hasTrack

            onClicked: {
                root.controller.next()
            }
        }
    }


    // =============================================================
    // Shuffle
    // =============================================================

    PlayerButton {
        id: shuffleButton

        width:
            40

        height:
            40

        anchors.right:
            playbackControls.left

        anchors.rightMargin:
            10

        anchors.verticalCenter:
            playbackControls.verticalCenter

        text:
            root.shuffleText()

        fontSize:
            17

        enabled:
            root.hasTrack

        active:
            root.controller !== null &&
            root.controller !== undefined &&
            root.controller.shuffleEnabled

        onClicked: {
            root.controller.toggleShuffle()
        }
    }


    // =============================================================
    // Repeat
    // =============================================================

    PlayerButton {
        id: repeatButton

        width:
            40

        height:
            40

        anchors.left:
            playbackControls.right

        anchors.leftMargin:
            10

        anchors.verticalCenter:
            playbackControls.verticalCenter

        text:
            root.repeatText()

        fontSize:
            17

        enabled:
            root.hasTrack

        active:
            root.controller !== null &&
            root.controller !== undefined &&
            Number(
                root.controller.repeatMode
            ) !== 0

        onClicked: {
            root.controller.cycleRepeat()
        }
    }


    // =============================================================
    // Progress
    // =============================================================

    Item {
        id: progressArea

        anchors.left:
            artworkFrame.right

        anchors.leftMargin:
            18

        anchors.right:
            rightControls.left

        anchors.rightMargin:
            10

        anchors.bottom:
            parent.bottom

        anchors.bottomMargin:
            7

        height:
            18

        z:
            5


        Text {
            id: positionLabel

            anchors.left:
                parent.left

            anchors.verticalCenter:
                parent.verticalCenter

            width:
                30

            text:
                root.hasTrack
                    ? root.formatTime(
                        root.controller.position
                    )
                    : "0:00"

            color:
                AppTheme.textMuted

            font.pixelSize:
                9
        }


        Rectangle {
            id: progressBackground

            anchors.left:
                positionLabel.right

            anchors.right:
                durationLabel.left

            anchors.verticalCenter:
                parent.verticalCenter

            anchors.leftMargin:
                6

            anchors.rightMargin:
                6

            height:
                5

            radius:
                2.5

            color:
                Qt.rgba(
                    AppTheme.textPrimary.r,
                    AppTheme.textPrimary.g,
                    AppTheme.textPrimary.b,
                    0.18
                )


            Rectangle {
                id: progressValue

                anchors.left:
                    parent.left

                anchors.top:
                    parent.top

                anchors.bottom:
                    parent.bottom

                width: {
                    if (
                        root.controller === null ||
                        root.controller === undefined ||
                        Number(root.controller.duration) <= 0
                    ) {
                        return 0
                    }

                    var ratio =
                        Number(
                            root.controller.position
                        ) /
                        Number(
                            root.controller.duration
                        )

                    ratio =
                        Math.max(
                            0,
                            Math.min(
                                1,
                                ratio
                            )
                        )

                    return parent.width * ratio
                }

                radius:
                    2.5

                color:
                    root.playerAccent
            }


            Rectangle {
                id: progressHandle

                width:
                    9

                height:
                    9

                radius:
                    4.5

                anchors.verticalCenter:
                    parent.verticalCenter

                x:
                    Math.max(
                        0,
                        Math.min(
                            parent.width - width,
                            progressValue.width -
                            width / 2
                        )
                    )

                color:
                    root.playerAccent

                visible:
                    root.hasTrack
            }
        }


        Text {
            id: durationLabel

            anchors.right:
                parent.right

            anchors.verticalCenter:
                parent.verticalCenter

            width:
                30

            text:
                root.hasTrack
                    ? root.formatTime(
                        root.controller.duration
                    )
                    : "0:00"

            color:
                AppTheme.textMuted

            font.pixelSize:
                9

            horizontalAlignment:
                Text.AlignRight
        }


        MouseArea {
            anchors.left:
                progressBackground.left

            anchors.right:
                progressBackground.right

            anchors.verticalCenter:
                progressBackground.verticalCenter

            height:
                20

            enabled:
                root.hasTrack &&
                Number(
                    root.controller.duration
                ) > 0

            cursorShape:
                enabled
                    ? Qt.PointingHandCursor
                    : Qt.ArrowCursor

            onClicked: function(mouse) {
                if (
                    !enabled ||
                    root.controller === null ||
                    root.controller === undefined
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
                        ) *
                        ratio
                    )
                )
            }
        }
    }


    // =============================================================
    // Right controls
    // =============================================================

    Row {
        id: rightControls

        anchors.right:
            parent.right

        anchors.rightMargin:
            10

        anchors.verticalCenter:
            playbackControls.verticalCenter

        spacing:
            4


        // =========================================================
        // Volume
        // =========================================================

        Item {
            id: volumeArea

            width:
                92

            height:
                40

            anchors.verticalCenter:
                parent.verticalCenter


            PlayerButton {
                id: volumeButton

                width:
                    40

                height:
                    40

                enabled:
                    root.hasVolume

                anchors.left:
                    parent.left

                anchors.verticalCenter:
                    parent.verticalCenter

                onClicked: {
                    if (
                        root.currentVolume > 0
                    ) {
                        root.controller.setVolume(
                            0.0
                        )
                    } else {
                        root.controller.setVolume(
                            1.0
                        )
                    }
                }


                VolumeIcon {
                    anchors.centerIn:
                        parent

                    width:
                        20

                    height:
                        20

                    volume:
                        root.currentVolume

                    enabled:
                        root.hasVolume
                }
            }


            Slider {
                id: volumeSlider

                anchors.left:
                    volumeButton.right

                anchors.right:
                    parent.right

                anchors.verticalCenter:
                    parent.verticalCenter

                anchors.leftMargin:
                    3

                height:
                    24

                from:
                    0.0

                to:
                    1.0

                value:
                    root.currentVolume

                onMoved: {
                    if (
                        root.hasVolume
                    ) {
                        root.controller.setVolume(
                            value
                        )
                    }
                }


                background: Rectangle {
                    x:
                        volumeSlider.leftPadding

                    y:
                        volumeSlider.topPadding +
                        volumeSlider.availableHeight / 2 -
                        height / 2

                    width:
                        volumeSlider.availableWidth

                    height:
                        4

                    radius:
                        2

                    color:
                        Qt.rgba(
                            AppTheme.textPrimary.r,
                            AppTheme.textPrimary.g,
                            AppTheme.textPrimary.b,
                            0.12
                        )


                    Rectangle {
                        width:
                            volumeSlider.visualPosition *
                            parent.width

                        height:
                            parent.height

                        radius:
                            2

                        color:
                            root.playerAccent
                    }
                }


                handle: Rectangle {
                    width:
                        8

                    height:
                        8

                    radius:
                        4

                    x:
                        volumeSlider.leftPadding +
                        volumeSlider.visualPosition *
                        (
                            volumeSlider.availableWidth -
                            width
                        )

                    y:
                        volumeSlider.topPadding +
                        volumeSlider.availableHeight / 2 -
                        height / 2

                    color:
                        root.playerAccent
                }
            }
        }


        // =========================================================
        // Expanded Now Playing / Queue
        // =========================================================

        PlayerButton {
            id: expandedButton

            width:
                40

            height:
                40

            enabled:
                root.hasTrack

            onClicked: {
                root.expandedRequested()
            }


            Column {
                anchors.centerIn:
                    parent

                spacing:
                    3

                Repeater {
                    model:
                        3

                    Rectangle {
                        width:
                            17

                        height:
                            2

                        radius:
                            1

                        color:
                            AppTheme.textPrimary
                    }
                }
            }
        }
    }


    // =============================================================
    // Player button
    // =============================================================

    component PlayerButton: Rectangle {

        property string text: ""
        property int fontSize: 20
        property bool active: false

        signal clicked()


        radius:
            12

        color:
            !enabled
                ? "transparent"
                : mouseArea.containsMouse
                    ? Qt.rgba(
                        root.playerAccent.r,
                        root.playerAccent.g,
                        root.playerAccent.b,
                        0.16
                    )
                    : active
                        ? Qt.rgba(
                            root.playerAccent.r,
                            root.playerAccent.g,
                            root.playerAccent.b,
                            0.10
                        )
                        : "transparent"

        border.width:
                mouseArea.containsMouse && enabled
            ? 1
            : 0

        border.color:
            Qt.rgba(
                root.playerAccent.r,
                root.playerAccent.g,
                root.playerAccent.b,
                0.18
            )

        opacity:
            enabled
                ? 1
                : 0.35


        Behavior on color {
            ColorAnimation {
                duration:
                    140

                easing.type:
                    Easing.OutCubic
            }
        }


        Text {
            anchors.centerIn:
                parent

            text:
                parent.text

            color:
                parent.active
                    ? root.playerAccent
                    : AppTheme.textPrimary

            font.pixelSize:
                parent.fontSize

            font.weight:
                Font.Normal

            visible:
                parent.text.length > 0
        }


        MouseArea {
            id: mouseArea

            anchors.fill:
                parent

            hoverEnabled:
                true

            enabled:
                parent.enabled

            cursorShape:
                enabled
                    ? Qt.PointingHandCursor
                    : Qt.ArrowCursor

            onClicked: {
                parent.clicked()
            }
        }
    }


    // =============================================================
    // Volume icon
    // =============================================================

    component VolumeIcon: Item {

        property real volume: 1.0
        property bool enabled: true

        readonly property bool muted:
            volume <= 0.001

        readonly property bool low:
            volume > 0.001 &&
            volume < 0.5


        // ---------------------------------------------------------
        // Speaker body
        // ---------------------------------------------------------

        Shape {
            anchors.fill:
                parent

            visible:
                true

            ShapePath {
                fillColor:
                    root.hasTrack
                        ? root.playerAccent
                        : AppTheme.textPrimary

                strokeColor:
                    "transparent"

                strokeWidth:
                    0

                startX:
                    2

                startY:
                    7

                PathLine {
                    x:
                        6

                    y:
                        7
                }

                PathLine {
                    x:
                        11

                    y:
                        3
                }

                PathLine {
                    x:
                        11

                    y:
                        17
                }

                PathLine {
                    x:
                        6

                    y:
                        13
                }

                PathLine {
                    x:
                        2

                    y:
                        13
                }

                PathLine {
                    x:
                        2

                    y:
                        7
                }
            }
        }


        // ---------------------------------------------------------
        // Sound waves
        // ---------------------------------------------------------

        Shape {
            anchors.fill:
                parent

            visible:
                !muted

            ShapePath {
                fillColor:
                    "transparent"

                strokeColor:
                    root.hasTrack
                        ? root.playerAccent
                        : AppTheme.textPrimary

                strokeWidth:
                    1.5

                capStyle:
                    ShapePath.RoundCap

                joinStyle:
                    ShapePath.RoundJoin

                startX:
                    14

                startY:
                    7

                PathCubic {
                    control1X:
                        17

                    control1Y:
                        9

                    control2X:
                        17

                    control2Y:
                        11

                    x:
                        14

                    y:
                        13
                }
            }
        }


        Shape {
            anchors.fill:
                parent

            visible:
                !muted &&
                !low

            ShapePath {
                fillColor:
                    "transparent"

                strokeColor:
                    root.hasTrack
                        ? root.playerAccent
                        : AppTheme.textPrimary

                strokeWidth:
                    1.5

                capStyle:
                    ShapePath.RoundCap

                joinStyle:
                    ShapePath.RoundJoin

                startX:
                    15.5

                startY:
                    4.5

                PathCubic {
                    control1X:
                        20

                    control1Y:
                        7

                    control2X:
                        20

                    control2Y:
                        13

                    x:
                        15.5

                    y:
                        15.5
                }
            }
        }


        // ---------------------------------------------------------
        // Mute slash
        // ---------------------------------------------------------

        Shape {
            anchors.fill:
                parent

            visible:
                muted

            ShapePath {
                fillColor:
                    "transparent"

                strokeColor:
                    root.hasTrack
                        ? root.playerAccent
                        : AppTheme.textPrimary

                strokeWidth:
                    2

                capStyle:
                    ShapePath.RoundCap

                startX:
                    3

                startY:
                    4

                PathLine {
                    x:
                        17

                    y:
                        16
                }
            }
        }
    }


    // =============================================================
    // Icons
    // =============================================================

    function repeatText()
    {
        if (
            root.controller === null ||
            root.controller === undefined
        ) {
            return "↻"
        }

        var mode =
            Number(
                root.controller.repeatMode
            )

        if (
            mode === 1
        ) {
            return "↻A"
        }

        if (
            mode === 2
        ) {
            return "↻1"
        }

        return "↻"
    }


    function shuffleText()
    {
        if (
            root.controller !== null &&
            root.controller !== undefined &&
            root.controller.shuffleEnabled
        ) {
            return "🔀"
        }

        return "⇄"
    }


    // =============================================================
    // Duration
    // =============================================================

    function formatTime(
        milliseconds)
    {
        if (
            !isFinite(
                Number(milliseconds)
            ) ||
            Number(milliseconds) <= 0
        ) {
            return "0:00"
        }

        var totalSeconds =
            Math.floor(
                Number(milliseconds) /
                1000
            )

        var minutes =
            Math.floor(
                totalSeconds /
                60
            )

        var seconds =
            totalSeconds %
            60

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