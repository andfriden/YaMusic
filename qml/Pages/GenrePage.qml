import QtQuick
import QtQuick.Controls.Basic

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
    // Page size
    // =============================================================

    width:
        parent
            ? parent.width
            : 0

    implicitWidth:
        width

    implicitHeight:
        contentColumn.implicitHeight + 40

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

                        width:
                            root.cardWidth

                        height:
                            238

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
}