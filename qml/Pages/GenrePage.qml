import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root


    // =============================================================
    // Controller
    // =============================================================

    property var controller: null


    readonly property var genreController:
            root.controller !== null &&
        root.controller !== undefined &&
        root.controller.genreController !== null &&
        root.controller.genreController !== undefined
        ? root.controller.genreController
        : null


    // =============================================================
    // Genre
    // =============================================================

    property string genreId: ""
    property string genreTitle: ""
    property string genreImage: ""
    property string genreColor: ""


    // =============================================================
    // Radio button theme colors
    // =============================================================

    readonly property color radioButtonColor:
        AppTheme.dark
            ? AppTheme.accent
            : Qt.rgba(0, 0, 0, 0.84)

    readonly property color radioButtonHoverColor:
        AppTheme.dark
            ? AppTheme.accentHover
            : Qt.rgba(0.15, 0.15, 0.15, 1.0)


    // =============================================================
    // Page size
    // =============================================================

    width:
        parent
            ? parent.width
            : 0

    implicitWidth:
        width

    implicitHeight:
        contentColumn.implicitHeight + 48

    height:
        implicitHeight


    // =============================================================
    // Background
    // =============================================================

    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.backgroundPrimary
    }


    // =============================================================
    // Layout
    // =============================================================

    readonly property int margin:
        20

    readonly property int spacing:
        12

    readonly property int columns:
        5

    readonly property int cardWidth:
        Math.floor(
            (
                contentColumn.width -
                spacing * (columns - 1)
            ) /
            columns
        )


    // =============================================================
    // Content
    // =============================================================

    Column {
        id: contentColumn

        width:
            root.width -
            root.margin * 2

        anchors.left:
            parent.left

        anchors.right:
            parent.right

        anchors.top:
            parent.top

        anchors.margins:
            root.margin

spacing:
                28


            // =========================================================
            // Genre header
            // =========================================================

            Row {
                width:
                    parent.width

                height:
                    180

                spacing:
                    24


                Rectangle {
                    width:
                        180

                    height:
                        180

                    radius:
                        14

                    color:
                            root.genreColor.length > 0
                        ? root.genreColor
                        : AppTheme.panel

                    clip:
                        true


                    Image {
                        anchors.fill:
                            parent

                        source:
                                root.genreImage.length > 0
                            ? "image://yandex/" +
                            root.genreImage
                            : ""

                        fillMode:
                            Image.PreserveAspectCrop

                        asynchronous:
                            true

                        cache:
                            true
                    }
                }


                Column {
                    anchors.verticalCenter:
                        parent.verticalCenter

                    width:
                        parent.width - 204

                    spacing:
                        8


                    Label {
                        width:
                            parent.width

                        text:
                            root.genreTitle

                        color:
                            AppTheme.textPrimary

                        font.pixelSize:
                            32

                        font.bold:
                            true

                        elide:
                            Text.ElideRight
                    }


                    Label {
                        text:
                                root.genreController &&
                            root.genreController.genreLoading
                            ? "Загрузка плейлистов..."
                            : root.genreController &&
                                root.genreController.genrePlaylists &&
                                root.genreController.genrePlaylists.length > 0
                                ? root.genreController.genrePlaylists.length +
                                " плейлистов"
                                : ""

                        color:
                            AppTheme.textSecondary

                        font.pixelSize:
                            15
                    }


                    // =====================================================
                    // Radio button
                    // =====================================================

                    Rectangle {
                        id: radioButton

                        visible:
                            root.genreId.length > 0

                        width:
                            radioLabel.implicitWidth + 32

                        height:
                            36

                        radius:
                            8

                        color:
                            radioMouse.containsMouse
                                ? root.radioButtonHoverColor
                                : root.radioButtonColor

                        Behavior on color {
                            ColorAnimation {
                                duration: 140
                                easing.type: Easing.OutCubic
                            }
                        }

                        enabled:
                            !root.genreController ||
                            !root.genreController.stationLoading

                        opacity:
                            enabled ? 1.0 : 0.5

                        Label {
                            id: radioLabel

                            anchors.centerIn:
                                parent

                            text:
                                root.genreController &&
                                root.genreController.stationLoading
                                ? "Загрузка..."
                                : "▶  Радио"

                            color:
                                "#ffffff"

                            font.pixelSize:
                                13

                            font.bold:
                                true
                        }

                        MouseArea {
                            id: radioMouse

                            anchors.fill:
                                parent

                            hoverEnabled:
                                true

                            cursorShape:
                                Qt.PointingHandCursor

                            enabled:
                                parent.enabled

                            onClicked: {
                                if (
                                    root.genreController
                                ) {
                                    root.genreController.loadGenreStation(
                                        root.genreId
                                    )
                                }
                            }
                        }
                    }
                }
            }


            // =========================================================
            // Station tracks
            // =========================================================

            Rectangle {
                width:
                    parent.width

                height:
                        root.genreController !== null &&
                        root.genreController.stationModel !== null &&
                        root.genreController.stationModel.count > 0
                    ? 60 + Math.min(
                        root.genreController.stationModel.count,
                        6
                    ) * 68
                    : 0

                radius:
                    12

                color:
                    AppTheme.panel

                border.width:
                    1

                border.color:
                    AppTheme.borderSubtle

                visible:
                    height > 0


                Column {
                    anchors.fill:
                        parent

                    anchors.margins:
                        14

                    spacing:
                        10


                    Label {
                        width:
                            parent.width

                        text:
                            "Радиостанция"

                        color:
                            AppTheme.textPrimary

                        font.pixelSize:
                            22

                        font.bold:
                            true
                    }


                    ListView {
                        id:
                            stationView

                        width:
                            parent.width

                        height:
                            Math.min(
                                count,
                                6
                            ) * 68

                        model:
                            root.genreController !== null
                                ? root.genreController.stationModel
                                : null

                        clip:
                            true

                        spacing:
                            4

                        interactive:
                            count > 6

                        ScrollBar.vertical:
                            ScrollBar {
                                policy:
                                        stationView.count > 6
                                    ? ScrollBar.AsNeeded
                                    : ScrollBar.AlwaysOff
                            }


                        delegate:
                            Rectangle {
                                id:
                                    stationRow

                                required property int index
                                required property string trackId
                                required property string title
                                required property string artist
                                required property string artistId
                                required property string coverUri
                                required property int durationMs

                                width:
                                    stationView.width

                                height:
                                    64

                                radius:
                                    8

                                color:
                                    root.controller &&
                                    root.controller.currentTrackId !== "" &&
                                    stationRow.trackId ===
                                        root.controller.currentTrackId
                                        ? AppTheme.panelActive
                                        : (
                                            stationMouse.containsMouse
                                                ? AppTheme.panelActive
                                                : AppTheme.panelSecondary
                                        )

                                border.width:
                                    root.controller &&
                                    root.controller.currentTrackId !== "" &&
                                    stationRow.trackId ===
                                        root.controller.currentTrackId
                                        ? 1
                                        : 0

                                border.color:
                                    AppTheme.accent


                                Rectangle {
                                    width:
                                        48

                                    height:
                                        48

                                    anchors.left:
                                        parent.left

                                    anchors.leftMargin:
                                        8

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    radius:
                                        6

                                    color:
                                        AppTheme.artworkPlaceholder

                                    clip:
                                        true


                                    Image {
                                        anchors.fill:
                                            parent

                                        source:
                                                stationRow.coverUri.length > 0
                                            ? "image://yandex/" +
                                            stationRow.coverUri
                                            : ""

                                        fillMode:
                                            Image.PreserveAspectCrop

                                        asynchronous:
                                            true

                                        cache:
                                            true

                                        visible:
                                            status === Image.Ready
                                    }


                                    Label {
                                        anchors.centerIn:
                                            parent

                                        text:
                                            "♪"

                                        color:
                                            AppTheme.textMuted

                                        font.pixelSize:
                                            18

                                        visible:
                                            parent.children[0].status !==
                                            Image.Ready
                                    }
                                }


                                Column {
                                    anchors.left:
                                        parent.left

                                    anchors.leftMargin:
                                        68

                                    anchors.right:
                                        durationLabel.left

                                    anchors.rightMargin:
                                        12

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    spacing:
                                        2


                                    Label {
                                        width:
                                            parent.width

                                        text:
                                                stationRow.title.length > 0
                                            ? stationRow.title
                                            : "Без названия"

                                        color:
                                            AppTheme.textPrimary

                                        font.pixelSize:
                                            14

                                        font.bold:
                                            true

                                        elide:
                                            Text.ElideRight
                                    }


                                    Label {
                                        width:
                                            Math.min(
                                                implicitWidth,
                                                parent.width
                                            )

                                        text:
                                                stationRow.artist.length > 0
                                            ? stationRow.artist
                                            : ""

                                        color:
                                            AppTheme.textSecondary

                                        font.pixelSize:
                                            12

                                        elide:
                                            Text.ElideRight
                                    }
                                }


                                Label {
                                    id:
                                        durationLabel

                                    width:
                                        44

                                    anchors.right:
                                        parent.right

                                    anchors.rightMargin:
                                        14

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    text:
                                        root.formatDuration(
                                            stationRow.durationMs
                                        )

                                    color:
                                        AppTheme.textMuted

                                    font.pixelSize:
                                        11

                                    horizontalAlignment:
                                        Text.AlignRight
                                }


                                MouseArea {
                                    id:
                                        stationMouse

                                    anchors.fill:
                                        parent

                                    hoverEnabled:
                                        true

                                    cursorShape:
                                        Qt.PointingHandCursor

                                    acceptedButtons:
                                        Qt.LeftButton | Qt.RightButton

                                    z:
                                        0

                                    onClicked: {
                                        if (
                                            root.controller
                                        ) {
                                            root.controller.selectGenreStationTrack(
                                                stationRow.index
                                            )
                                        }
                                    }

                                    onPressed: {
                                        if (
                                            mouse.button !== Qt.RightButton ||
                                            !root.controller
                                        ) {
                                            return
                                        }

                                        root.controller.copyTrack(
                                            stationRow.title,
                                            stationRow.artist
                                        )
                                    }
                                }
                            }
                    }
                }
            }


        // =========================================================
        // Playlists title
        // =========================================================

        Label {
            width:
                parent.width

            text:
                "Плейлисты"

            color:
                AppTheme.textPrimary

            font.pixelSize:
                24

            font.bold:
                true
        }


        // =========================================================
        // Playlists grid
        // =========================================================

        Grid {
            id: playlistGrid

            width:
                parent.width

            columns:
                root.columns

            columnSpacing:
                root.spacing

            rowSpacing:
                16


            Repeater {
                id: playlistRepeater

                model:
                        root.genreController !== null
                    ? root.genreController.genrePlaylists
                    : []


                delegate:
                    Rectangle {
                        id: playlistCard

                        required property var modelData


                        // =================================================
                        // Card geometry
                        // =================================================

                        width:
                            root.cardWidth

                        height:
                            artworkBox.height +
                            58


                        radius:
                            10

                        color:
                            playlistMouseArea.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary

                        border.width:
                            1

                        border.color:
                            playlistMouseArea.containsMouse
                                ? AppTheme.border
                                : AppTheme.borderSubtle


                        Behavior on color {
                            ColorAnimation {
                                duration:
                                    140

                                easing.type:
                                    Easing.OutCubic
                            }
                        }


                        // =================================================
                        // Artwork
                        // =================================================

                        Rectangle {
                            id: artworkBox

                            x:
                                8

                            y:
                                8

                            width:
                                parent.width - 16

                            height:
                                width

                            radius:
                                8

                            color:
                                AppTheme.panelHover

                            clip:
                                true


                            Image {
                                id: cover

                                anchors.fill:
                                    parent

                                source:
                                        playlistCard.modelData.coverUri &&
                                    String(
                                        playlistCard.modelData.coverUri
                                    ).length > 0
                                    ? "image://yandex/" +
                                    String(
                                        playlistCard.modelData.coverUri
                                    )
                                    : ""

                                sourceSize:
                                    Qt.size(
                                        Math.max(
                                            1,
                                            artworkBox.width * 2
                                        ),
                                        Math.max(
                                            1,
                                            artworkBox.height * 2
                                        )
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


                            Label {
                                anchors.centerIn:
                                    parent

                                text:
                                    "♪"

                                color:
                                    AppTheme.textSecondary

                                font.pixelSize:
                                    42

                                visible:
                                    cover.status !==
                                    Image.Ready
                            }


                            Rectangle {
                                anchors.fill:
                                    parent

                                radius:
                                    8

                                color:
                                    "transparent"

                                border.width:
                                    1

                                border.color:
                                    playlistMouseArea.containsMouse
                                        ? AppTheme.border
                                        : AppTheme.borderSubtle
                            }
                        }


                        // =================================================
                        // Title
                        // =================================================

                        Label {
                            id: titleLabel

                            x:
                                8

                            width:
                                parent.width - 16

                            anchors.top:
                                artworkBox.bottom

                            anchors.topMargin:
                                8

                            height:
                                18

                            text:
                                String(
                                    playlistCard.modelData.title ||
                                    ""
                                )

                            color:
                                AppTheme.textPrimary

                            font.pixelSize:
                                13

                            font.bold:
                                true

                            verticalAlignment:
                                Text.AlignVCenter

                            elide:
                                Text.ElideRight

                            maximumLineCount:
                                1
                        }


                        // =================================================
                        // Track count
                        // =================================================

                        Label {
                            id: trackCountLabel

                            x:
                                8

                            width:
                                parent.width - 16

                            anchors.top:
                                titleLabel.bottom

                            anchors.topMargin:
                                3

                            height:
                                16

                            text:
                                    Number(
                                        playlistCard.modelData.trackCount ||
                                        0
                                    ) > 0
                                ? qsTr("%1 треков")
                                    .arg(
                                    Number(
                                        playlistCard.modelData.trackCount
                                    )
                                )
                                : ""

                            color:
                                AppTheme.textSecondary

                            font.pixelSize:
                                11

                            verticalAlignment:
                                Text.AlignVCenter

                            elide:
                                Text.ElideRight

                            maximumLineCount:
                                1

                            visible:
                                text.length > 0
                        }


                        // =================================================
                        // Click
                        // =================================================

                        MouseArea {
                            id: playlistMouseArea

                            anchors.fill:
                                parent

                            hoverEnabled:
                                true

                            cursorShape:
                                Qt.PointingHandCursor

                            onClicked: {
                                if (
                                    root.controller === null ||
                                    root.controller === undefined
                                ) {
                                    return
                                }


                                const uid =
                                    String(
                                        playlistCard.modelData.uid ||
                                        ""
                                    ).trim()


                                const kind =
                                    Number(
                                        playlistCard.modelData.kind ||
                                        0
                                    )


                                if (
                                    uid.length === 0 ||
                                    kind <= 0
                                ) {
                                    return
                                }


                                root.controller.selectPersonalPlaylist(
                                    uid,
                                    kind
                                )
                            }
                        }
                    }
            }
        }


        // =========================================================
        // Empty state
        // =========================================================

        Label {
            visible:
                root.genreController !== null &&
                !root.genreController.genreLoading &&
                (
                    !root.genreController.genrePlaylists ||
                    root.genreController.genrePlaylists.length === 0
                )

            width:
                parent.width

            text:
                "Плейлисты не найдены"

            color:
                AppTheme.textSecondary

            font.pixelSize:
                16

            horizontalAlignment:
                Text.AlignHCenter
        }


        // =========================================================
        // Loading state
        // =========================================================

        Label {
            visible:
                root.genreController !== null &&
                root.genreController.genreLoading

            width:
                parent.width

            text:
                "Загрузка плейлистов..."

            color:
                AppTheme.textSecondary

            font.pixelSize:
                16

            horizontalAlignment:
                Text.AlignHCenter
        }
    }


    // =============================================================
    // Load genre
    // =============================================================

    onGenreIdChanged: {
        if (
            root.genreController !== null &&
            root.genreId.length > 0
        ) {
            root.genreController.loadGenre(
                root.genreId
            )
        }
    }

    function formatDuration(milliseconds) {
        var value = Number(milliseconds)
        if (!isFinite(value) || value <= 0)
            return "0:00"
        var totalSeconds = Math.floor(value / 1000)
        var minutes = Math.floor(totalSeconds / 60)
        var seconds = totalSeconds % 60
        return minutes + ":" + (seconds < 10 ? "0" : "") + seconds
    }
}