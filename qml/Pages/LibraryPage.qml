import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    width:
        parent
            ? parent.width
            : 0

    height:
        parent
            ? parent.height
            : 700

    implicitWidth:
        width

    implicitHeight:
        height


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
    // Main page scroll
    // =============================================================

    Flickable {
        id: pageFlickable

        anchors.fill:
            parent

        clip:
            true

        boundsBehavior:
            Flickable.StopAtBounds

        contentWidth:
            width

        contentHeight:
            contentColumn.implicitHeight + 40


        ScrollBar.vertical:
            ScrollBar {
                policy:
                    ScrollBar.AsNeeded
            }


        Column {
            id: contentColumn

            width:
                pageFlickable.width - 40

            anchors.left:
                parent.left

            anchors.leftMargin:
                20

            anchors.top:
                parent.top

            anchors.topMargin:
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


                            // -------------------------------------------------
                            // Artwork
                            // -------------------------------------------------

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


                            // -------------------------------------------------
                            // Title
                            // -------------------------------------------------

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


                            // -------------------------------------------------
                            // Track count
                            // -------------------------------------------------

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


                    // ---------------------------------------------------------
                    // Empty state
                    // ---------------------------------------------------------

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
            // Likes
            // =========================================================

            Column {
                id: likesSection

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


                // ---------------------------------------------------------
                // Liked tracks container
                // ---------------------------------------------------------

                Rectangle {
                    id: likedTracksContainer

                    width:
                        parent.width

                    height:
                        likedTracksView.count > 0
                        ? Math.min(
                            likedTracksView.count * 74 + 20,
                            10 * 74 + 20
                        )
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
                            likedTracksView.count > 10

                        boundsBehavior:
                            Flickable.StopAtBounds


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
                                required property bool liked


                                width:
                                    likedTracksView.width

                                height:
                                    68

                                radius:
                                    8

                                color:
                                    root.controller &&
                                    root.controller.currentTrackId !== "" &&
                                    likedTrackDelegate.trackId ===
                                        root.controller.currentTrackId
                                        ? AppTheme.panelActive
                                        : (
                                            rowMouseArea.containsMouse
                                                ? AppTheme.panelActive
                                                : AppTheme.panelSecondary
                                        )

                                border.width:
                                    root.controller &&
                                    root.controller.currentTrackId !== "" &&
                                    likedTrackDelegate.trackId ===
                                        root.controller.currentTrackId
                                        ? 1
                                        : 0

                                border.color:
                                    AppTheme.accent


                                // =============================================
                                // Row click
                                // =============================================

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


                                // =============================================
                                // Artwork
                                // =============================================

                                Rectangle {
                                    id: coverContainer

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
                                        id: cover

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
                                            cover.status !== Image.Ready
                                    }
                                }


                                // =============================================
                                // Track information
                                // =============================================

                                Column {
                                    id: trackInfo

                                    anchors.left:
                                        coverContainer.right

                                    anchors.leftMargin:
                                        12

                                    anchors.right:
                                        durationLabel.left

                                    anchors.rightMargin:
                                        12

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    spacing:
                                        2


                                    // -----------------------------------------
                                    // Title
                                    // -----------------------------------------

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


                                    // -----------------------------------------
                                    // Artist
                                    // -----------------------------------------

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
                                                    trackInfo.width
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


                                    // -----------------------------------------
                                    // Album
                                    // -----------------------------------------

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
                                                    trackInfo.width
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


                                // =============================================
                                // Duration
                                // =============================================

                                Label {
                                    id: durationLabel

                                    anchors.right:
                                        likeButton.left

                                    anchors.rightMargin:
                                        6

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


                                // =============================================
                                // Like button
                                // =============================================

                                Item {
                                    id: likeButton

                                    width:
                                        36

                                    height:
                                        36

                                    anchors.right:
                                        parent.right

                                    anchors.rightMargin:
                                        8

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    z:
                                        20


                                    Label {
                                        anchors.centerIn:
                                            parent

                                        text:
                                            likedTrackDelegate.liked
                                                ? "♥"
                                                : "♡"

                                        color:
                                            likedTrackDelegate.liked
                                                ? AppTheme.accent
                                                : AppTheme.textSecondary

                                        font.pixelSize:
                                            21
                                    }


                                    MouseArea {
                                        anchors.fill:
                                            parent

                                        cursorShape:
                                            Qt.PointingHandCursor

                                        onClicked: {
                                            if (
                                                root.controller === null ||
                                                root.controller === undefined
                                            ) {
                                                return
                                            }


                                            root.controller.toggleLike(
                                                likedTrackDelegate.trackId,
                                                likedTrackDelegate.liked
                                            )
                                        }
                                    }
                                }
                            }


                        // ---------------------------------------------------------
                        // Empty state
                        // ---------------------------------------------------------

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


            // =========================================================
            // Liked albums
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
                        "Сохранённые альбомы"

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        20

                    font.bold:
                        true
                }


                Rectangle {
                    width:
                        parent.width

                    height:
                        albumGridView.count > 0
                        ? Math.min(albumGridView.count * 74 + 20, 5 * 74 + 20)
                        : 120

                    radius:
                        12

                    color:
                        AppTheme.panel

                    border.width:
                        1

                    border.color:
                        AppTheme.borderSubtle

                    visible:
                        root.controller !== null &&
                        root.controller !== undefined


                    GridView {
                        id: albumGridView

                        anchors.fill:
                            parent

                        anchors.margins:
                            10

                        cellWidth:
                            180

                        cellHeight:
                            64

                        clip:
                            true

                        interactive:
                            albumGridView.count > 5

                        boundsBehavior:
                            Flickable.StopAtBounds

                        model:
                            root.controller !== null &&
                            root.controller !== undefined
                            ? root.controller.likedAlbumsModel
                            : null


                        delegate:
                            Rectangle {
                                id: albumDelegate

                                width:
                                    180

                                height:
                                    64

                                radius:
                                    8

                                color:
                                    albumMouse.containsMouse
                                        ? AppTheme.panelActive
                                        : AppTheme.panelSecondary

                                required property string albumId
                                required property string title
                                required property string coverUri
                                required property int year


                                Rectangle {
                                    id: albumCover

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
                                        id: albumCoverImage

                                        anchors.fill:
                                            parent

                                        source:
                                                albumDelegate.coverUri.length > 0
                                            ? "image://yandex/" +
                                              albumDelegate.coverUri
                                            : ""

                                        sourceSize:
                                            Qt.size(
                                                96,
                                                96
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
                                }


                                Column {
                                    anchors.left:
                                        albumCover.right

                                    anchors.leftMargin:
                                        10

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    spacing:
                                        2

                                    Label {
                                        width:
                                            120

                                        text:
                                            albumDelegate.title

                                        color:
                                            AppTheme.textPrimary

                                        font.pixelSize:
                                            13

                                        font.bold:
                                            true

                                        elide:
                                            Text.ElideRight

                                        maximumLineCount:
                                            1
                                    }


                                    Label {
                                        text:
                                            albumDelegate.year > 0
                                                ? albumDelegate.year
                                                : ""

                                        color:
                                            AppTheme.textSecondary

                                        font.pixelSize:
                                            11
                                    }
                                }


                                MouseArea {
                                    id: albumMouse

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

                                        root.controller.loadAlbum(
                                            albumDelegate.albumId
                                        )
                                    }
                                }
                            }
                    }


                    Label {
                        anchors.centerIn:
                            parent

                        text:
                            "Нет сохранённых альбомов"

                        color:
                            AppTheme.textSecondary

                        font.pixelSize:
                            13

                        visible:
                            root.controller !== null &&
                            root.controller !== undefined &&
                            (
                                root.controller.likedAlbumsModel === null ||
                                root.controller.likedAlbumsModel.count === 0
                            )
                    }
                }
            }


            // =========================================================
            // Liked artists
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
                        "Любимые исполнители"

                    color:
                        AppTheme.textPrimary

                    font.pixelSize:
                        20

                    font.bold:
                        true
                }


                Rectangle {
                    width:
                        parent.width

                    height:
                        artistGridView.count > 0
                        ? Math.min(artistGridView.count * 74 + 20, 5 * 74 + 20)
                        : 120

                    radius:
                        12

                    color:
                        AppTheme.panel

                    border.width:
                        1

                    border.color:
                        AppTheme.borderSubtle

                    visible:
                        root.controller !== null &&
                        root.controller !== undefined


                    GridView {
                        id: artistGridView

                        anchors.fill:
                            parent

                        anchors.margins:
                            10

                        cellWidth:
                            180

                        cellHeight:
                            64

                        clip:
                            true

                        interactive:
                            artistGridView.count > 5

                        boundsBehavior:
                            Flickable.StopAtBounds

                        model:
                            root.controller !== null &&
                            root.controller !== undefined
                            ? root.controller.likedArtistsModel
                            : null


                        delegate:
                            Rectangle {
                                id: artistDelegate

                                width:
                                    180

                                height:
                                    64

                                radius:
                                    8

                                color:
                                    artistMouse.containsMouse
                                        ? AppTheme.panelActive
                                        : AppTheme.panelSecondary

                                required property string artistId
                                required property string name
                                required property string coverUri


                                Rectangle {
                                    id: artistCover

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
                                        24

                                    color:
                                        AppTheme.artworkPlaceholder

                                    clip:
                                        true


                                    Image {
                                        id: artistCoverImage

                                        anchors.fill:
                                            parent

                                        source:
                                                artistDelegate.coverUri.length > 0
                                            ? "image://yandex/" +
                                              artistDelegate.coverUri
                                            : ""

                                        sourceSize:
                                            Qt.size(
                                                96,
                                                96
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
                                }


                                Label {
                                    anchors.left:
                                        artistCover.right

                                    anchors.leftMargin:
                                        10

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    width:
                                        120

                                    text:
                                        artistDelegate.name

                                    color:
                                        AppTheme.textPrimary

                                    font.pixelSize:
                                        13

                                    font.bold:
                                        true

                                    elide:
                                        Text.ElideRight

                                    maximumLineCount:
                                        1
                                }


                                MouseArea {
                                    id: artistMouse

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

                                        root.controller.loadArtist(
                                            artistDelegate.artistId
                                        )
                                    }
                                }
                            }
                    }


                    Label {
                        anchors.centerIn:
                            parent

                        text:
                            "Нет любимых исполнителей"

                        color:
                            AppTheme.textSecondary

                        font.pixelSize:
                            13

                        visible:
                            root.controller !== null &&
                            root.controller !== undefined &&
                            (
                                root.controller.likedArtistsModel === null ||
                                root.controller.likedArtistsModel.count === 0
                            )
                    }
                }
            }


            // =========================================================
            // Bottom spacing
            // =========================================================

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