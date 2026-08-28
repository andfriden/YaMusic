import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    readonly property int pageHeight:
        Math.max(
            700,
            contentColumn.implicitHeight + 40
        )

    width:
        parent
            ? parent.width
            : 0

    height:
        pageHeight

    implicitWidth:
        width

    implicitHeight:
        pageHeight


    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.backgroundPrimary
    }


    ScrollView {
        id: pageScroll

        anchors.fill:
            parent

        clip:
            true


        ScrollBar.vertical:
            ScrollBar {
                policy:
                    ScrollBar.AsNeeded
            }


        Column {
            id: contentColumn

            width:
                pageScroll.availableWidth

            anchors.leftMargin:
                20

            anchors.rightMargin:
                20

            anchors.topMargin:
                20

            anchors.bottomMargin:
                20

            spacing:
                28


            // =========================================================
            // Header
            // =========================================================

            Label {
                width:
                    parent.width

                text:
                    "Медиатека"

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    28

                font.bold:
                    true
            }


            // =========================================================
            // My playlists
            // =========================================================

            Column {
                width:
                    parent.width

                spacing:
                    12


                Label {
                    width:
                        parent.width

                    text:
                        "Мои плейлисты"

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        20

                    font.bold:
                        true
                }


                Label {
                    width:
                        parent.width

                    text:
                        "Загрузка плейлистов..."

                    color:
                        AppTheme.textSecondary

                    font.pixelSize:
                        13

                    visible:
                        root.controller !== null &&
                        root.controller !== undefined &&
                        root.controller.loadingLibraryPlaylists
                }


                ListView {
                    id: playlistsView

                    width:
                        parent.width

                    height:
                        220

                    orientation:
                        ListView.Horizontal

                    spacing:
                        14

                    clip:
                        true

                    boundsBehavior:
                        Flickable.StopAtBounds


                    model:
                            root.controller !== null &&
                        root.controller !== undefined
                        ? root.controller.libraryPlaylistsModel
                        : null


                    ScrollBar.horizontal:
                        ScrollBar {
                            policy:
                                ScrollBar.AsNeeded
                        }


                    delegate:
                        Rectangle {
                            id: playlistCard

                            required property int index
                            required property string uid
                            required property int kind
                            required property string title
                            required property string coverUri
                            required property int trackCount


                            width:
                                170

                            height:
                                200

                            radius:
                                12

                            color:
                                playlistMouseArea.containsMouse
                                    ? AppTheme.panelActive
                                    : AppTheme.panelSecondary

                            border.width:
                                1

                            border.color:
                                AppTheme.borderSubtle

                            clip:
                                true


                            Rectangle {
                                id: playlistArtworkContainer

                                anchors.top:
                                    parent.top

                                anchors.left:
                                    parent.left

                                anchors.right:
                                    parent.right

                                height:
                                    140

                                color:
                                    AppTheme.artworkPlaceholder


                                Image {
                                    id: playlistArtwork

                                    anchors.fill:
                                        parent

                                    source:
                                            playlistCard.coverUri.length > 0
                                        ? "image://yandex/" +
                                        playlistCard.coverUri
                                        : ""

                                    sourceSize:
                                        Qt.size(
                                            280,
                                            280
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
                                        32

                                    visible:
                                        playlistArtwork.status !==
                                        Image.Ready
                                }
                            }


                            Label {
                                id: playlistTitle

                                anchors.left:
                                    parent.left

                                anchors.right:
                                    parent.right

                                anchors.top:
                                    playlistArtworkContainer.bottom

                                anchors.leftMargin:
                                    10

                                anchors.rightMargin:
                                    10

                                anchors.topMargin:
                                    8

                                text:
                                        playlistCard.title.length > 0
                                    ? playlistCard.title
                                    : qsTr("Плейлист")

                                color:
                                    AppTheme.textPrimary

                                font.pixelSize:
                                    13

                                font.bold:
                                    true

                                elide:
                                    Text.ElideRight
                            }


                            Label {
                                anchors.left:
                                    parent.left

                                anchors.right:
                                    parent.right

                                anchors.top:
                                    playlistTitle.bottom

                                anchors.leftMargin:
                                    10

                                anchors.rightMargin:
                                    10

                                anchors.topMargin:
                                    3

                                text:
                                        playlistCard.trackCount > 0
                                    ? qsTr("%1 треков")
                                        .arg(
                                        playlistCard.trackCount
                                    )
                                    : ""

                                color:
                                    AppTheme.textSecondary

                                font.pixelSize:
                                    11

                                elide:
                                    Text.ElideRight
                            }


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


                                    root.controller
                                        .selectLibraryPlaylist(
                                        playlistCard.index
                                    )
                                }
                            }
                        }


                    Label {
                        anchors.centerIn:
                            parent

                        text:
                            "Плейлистов нет"

                        color:
                            AppTheme.textSecondary

                        font.pixelSize:
                            13

                        visible:
                            root.controller !== null &&
                            root.controller !== undefined &&
                            !root.controller.loadingLibraryPlaylists &&
                            root.controller.libraryPlaylistsModel !== null &&
                            root.controller.libraryPlaylistsModel.count === 0
                    }
                }
            }


            // =========================================================
            // Liked tracks
            // =========================================================

            Column {
                id: likedSection

                width:
                    parent.width

                spacing:
                    12


                Label {
                    width:
                        parent.width

                    text:
                        "Мне нравится"

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        20

                    font.bold:
                        true
                }


                Label {
                    width:
                        parent.width

                    text:
                        "Загрузка понравившихся треков..."

                    color:
                        AppTheme.textSecondary

                    font.pixelSize:
                        13

                    visible:
                        root.controller !== null &&
                        root.controller !== undefined &&
                        root.controller.loadingLikedTracks
                }


                Rectangle {
                    id: likedTracksContainer

                    width:
                        parent.width

                    height:
                            likedTracksView.count > 0
                        ? likedTracksView.count * 74 + 20
                        : 120

                    radius:
                        12

                    color:
                        AppTheme.panel

                    border.width:
                        1

                    border.color:
                        AppTheme.borderSubtle


                    ListView {
                        id: likedTracksView

                        anchors.fill:
                            parent

                        anchors.leftMargin:
                            10

                        anchors.rightMargin:
                            10

                        anchors.topMargin:
                            10

                        anchors.bottomMargin:
                            10

                        spacing:
                            6

                        clip:
                            true

                        interactive:
                            false

                        model:
                                root.controller !== null &&
                            root.controller !== undefined
                            ? root.controller.likedTracksModel
                            : null


                        delegate:
                            Rectangle {
                                id: likedTrackDelegate

                                required property int index
                                required property string trackId
                                required property string title
                                required property string artist
                                required property string artistId
                                required property string album
                                required property string albumId
                                required property string coverUri
                                required property int durationMs


                                width:
                                    likedTracksView.width

                                height:
                                    68

                                radius:
                                    8

                                color:
                                    rowMouseArea.containsMouse
                                        ? AppTheme.panelActive
                                        : AppTheme.panelSecondary

                                border.width:
                                    1

                                border.color:
                                    AppTheme.borderSubtle


                                // =================================================
                                // Track click
                                // =================================================

                                MouseArea {
                                    id: rowMouseArea

                                    anchors.fill:
                                        parent

                                    hoverEnabled:
                                        true

                                    cursorShape:
                                        Qt.PointingHandCursor

                                    z:
                                        0

                                    onClicked: {
                                        if (
                                            root.controller === null ||
                                            root.controller === undefined
                                        ) {
                                            return
                                        }


                                        root.controller
                                            .selectLikedTrack(
                                            likedTrackDelegate.index
                                        )
                                    }
                                }


                                // =================================================
                                // Artwork
                                // =================================================

                                Rectangle {
                                    id: likedCoverContainer

                                    width:
                                        52

                                    height:
                                        52

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
                                        id: likedCover

                                        anchors.fill:
                                            parent

                                        source:
                                                likedTrackDelegate.coverUri.length > 0
                                            ? "image://yandex/" +
                                            likedTrackDelegate.coverUri
                                            : ""

                                        sourceSize:
                                            Qt.size(
                                                104,
                                                104
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

                                        visible:
                                            likedCover.status !==
                                            Image.Ready
                                    }
                                }


                                // =================================================
                                // Info
                                // =================================================

                                Column {
                                    id: likedTrackInfo

                                    anchors.left:
                                        likedCoverContainer.right

                                    anchors.leftMargin:
                                        12

                                    anchors.right:
                                        likedDuration.left

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
                                                likedTrackDelegate.title.length > 0
                                            ? likedTrackDelegate.title
                                            : qsTr("Без названия")

                                        color:
                                            AppTheme.textPrimary

                                        font.pixelSize:
                                            14

                                        font.bold:
                                            true

                                        elide:
                                            Text.ElideRight
                                    }


                                    // -------------------------------------------------
                                    // Artist
                                    // -------------------------------------------------

                                    Item {
                                        id: artistArea

                                        width:
                                            artistLabel.width

                                        height:
                                            artistLabel.height


                                        Label {
                                            id: artistLabel

                                            text:
                                                likedTrackDelegate.artist

                                            color:
                                                artistMouseArea.containsMouse
                                                    ? AppTheme.accent
                                                    : AppTheme.textSecondary

                                            font.pixelSize:
                                                12

                                            width:
                                                Math.min(
                                                    implicitWidth,
                                                    likedTrackInfo.width
                                                )

                                            height:
                                                18

                                            elide:
                                                Text.ElideRight
                                        }


                                        MouseArea {
                                            id: artistMouseArea

                                            anchors.fill:
                                                artistLabel

                                            hoverEnabled:
                                                true

                                            enabled:
                                                likedTrackDelegate.artistId.length > 0

                                            cursorShape:
                                                enabled
                                                    ? Qt.PointingHandCursor
                                                    : Qt.ArrowCursor

                                            z:
                                                10

                                            onClicked: {
                                                root.controller
                                                    .loadArtist(
                                                    likedTrackDelegate.artistId
                                                )
                                            }
                                        }
                                    }


                                    // -------------------------------------------------
                                    // Album
                                    // -------------------------------------------------

                                    Item {
                                        id: albumArea

                                        width:
                                            albumLabel.width

                                        height:
                                            albumLabel.height


                                        Label {
                                            id: albumLabel

                                            text:
                                                likedTrackDelegate.album

                                            color:
                                                albumMouseArea.containsMouse
                                                    ? AppTheme.accent
                                                    : AppTheme.textMuted

                                            font.pixelSize:
                                                10

                                            width:
                                                Math.min(
                                                    implicitWidth,
                                                    likedTrackInfo.width
                                                )

                                            height:
                                                16

                                            elide:
                                                Text.ElideRight
                                        }


                                        MouseArea {
                                            id: albumMouseArea

                                            anchors.fill:
                                                albumLabel

                                            hoverEnabled:
                                                true

                                            enabled:
                                                likedTrackDelegate.albumId.length > 0

                                            cursorShape:
                                                enabled
                                                    ? Qt.PointingHandCursor
                                                    : Qt.ArrowCursor

                                            z:
                                                10

                                            onClicked: {
                                                root.controller
                                                    .loadAlbum(
                                                    likedTrackDelegate.albumId
                                                )
                                            }
                                        }
                                    }
                                }


                                // =================================================
                                // Duration
                                // =================================================

                                Label {
                                    id: likedDuration

                                    anchors.right:
                                        parent.right

                                    anchors.rightMargin:
                                        14

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    text:
                                        root.formatDuration(
                                            likedTrackDelegate.durationMs
                                        )

                                    color:
                                        AppTheme.textSecondary

                                    font.pixelSize:
                                        11
                                }
                            }


                        Label {
                            anchors.centerIn:
                                parent

                            text:
                                    root.controller !== null &&
                                root.controller !== undefined &&
                                root.controller.loadingLikedTracks
                                ? qsTr("Загрузка...")
                                : qsTr("Нет понравившихся треков")

                            color:
                                AppTheme.textSecondary

                            font.pixelSize:
                                13

                            visible:
                                root.controller !== null &&
                                root.controller !== undefined &&
                                !root.controller.loadingLikedTracks &&
                                (
                                    root.controller.likedTracksModel === null ||
                                    root.controller.likedTracksModel.count === 0
                                )
                        }
                    }
                }
            }


            Item {
                width:
                    1

                height:
                    20
            }
        }
    }


    // =============================================================
    // Initial load
    // =============================================================

    Component.onCompleted: {
        if (
            root.controller === null ||
            root.controller === undefined
        ) {
            return
        }


        root.controller.loadLibrary()

        root.controller.loadLikedTracks()
    }


    // =============================================================
    // Duration
    // =============================================================

    function formatDuration(milliseconds)
    {
        if (
            !milliseconds ||
            milliseconds <= 0
        ) {
            return "0:00"
        }


        var totalSeconds =
            Math.floor(
                milliseconds / 1000
            )


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
}