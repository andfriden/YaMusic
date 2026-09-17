pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller


    /*
     * =============================================================
     * Page
     * =============================================================
     */

    width:
        parent
            ? parent.width
            : 0

    implicitHeight:
        pageColumn.implicitHeight + 56


    /*
     * =============================================================
     * Controllers / models
     * =============================================================
     */

    readonly property var albumController:
            root.controller !== null &&
        root.controller !== undefined &&
        root.controller.albumController !== null &&
        root.controller.albumController !== undefined
        ? root.controller.albumController
        : null

    readonly property var albumModel:
            root.albumController !== null &&
        root.albumController.albumModel !== null &&
        root.albumController.albumModel !== undefined
        ? root.albumController.albumModel
        : null


    /*
     * =============================================================
     * Album state
     * =============================================================
     */

    readonly property bool loading:
        root.albumController !== null &&
        root.albumController.loading === true

    readonly property bool hasAlbum:
        root.albumController !== null &&
        root.albumController.albumId !== ""

    readonly property int trackCount:
            root.albumModel !== null
        ? root.albumModel.count
        : 0

    readonly property string albumTitle:
            root.albumController !== null
        ? String(
            root.albumController.currentAlbumTitle || ""
        )
        : ""

    readonly property string albumArtworkUri:
            root.albumController !== null
        ? String(
            root.albumController.currentAlbumCoverUri || ""
        )
        : ""


    /*
     * =============================================================
     * Background
     * =============================================================
     */

    PlaylistPicker {
        id: addToPlaylistPopup
        controller: root.controller
    }

    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.backgroundPrimary
    }


    /*
     * =============================================================
     * Page scroll
     * =============================================================
     */

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
            id: pageColumn

            width:
                pageScroll.availableWidth

            spacing:
                20


            /*
             * =====================================================
             * Page title
             * =====================================================
             */

            Label {
                width:
                    parent.width

                height:
                    42

                text:
                        root.albumTitle.length > 0
                    ? root.albumTitle
                    : qsTr("Альбом")

                color:
                    AppTheme.textPrimary

                font.pixelSize:
                    32

                font.weight:
                    Font.DemiBold

                verticalAlignment:
                    Text.AlignVCenter

                elide:
                    Text.ElideRight

                maximumLineCount:
                    1
            }


            /*
             * =====================================================
             * Album header
             * =====================================================
             */

            Rectangle {
                width:
                    parent.width

                height:
                    216

                radius:
                    14

                color:
                    AppTheme.panel

                border.width:
                    1

                border.color:
                    AppTheme.borderSubtle


                Row {
                    anchors.fill:
                        parent

                    anchors.margins:
                        18

                    spacing:
                        20


                    /*
                     * ------------------------------------------------
                     * Album artwork
                     * ------------------------------------------------
                     */

                    Rectangle {
                        width:
                            180

                        height:
                            180

                        anchors.verticalCenter:
                            parent.verticalCenter

                        radius:
                            10

                        color:
                            AppTheme.artworkPlaceholder

                        clip:
                            true


                        Image {
                            id: albumImage

                            anchors.fill:
                                parent

                            source:
                                    root.albumArtworkUri.length > 0
                                ? "image://yandex/" +
                                root.albumArtworkUri
                                : ""

                            sourceSize:
                                Qt.size(
                                    180,
                                    180
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
                                AppTheme.textMuted

                            font.pixelSize:
                                40

                            visible:
                                albumImage.status !==
                                Image.Ready
                        }


                        Rectangle {
                            anchors.fill:
                                parent

                            radius:
                                10

                            color:
                                Qt.rgba(
                                    AppTheme.background.r,
                                    AppTheme.background.g,
                                    AppTheme.background.b,
                                    0
                                )

                            border.width:
                                1

                            border.color:
                                AppTheme.border
                        }
                    }


                    /*
                     * ------------------------------------------------
                     * Album information
                     * ------------------------------------------------
                     */

                    Column {
                        width:
                            parent.width - 200

                        anchors.verticalCenter:
                            parent.verticalCenter

                        spacing:
                            8


                        Label {
                            text:
                                qsTr("АЛЬБОМ")

                            color:
                                AppTheme.textMuted

                            font.pixelSize:
                                12

                            font.bold:
                                true
                        }


                        Row {
                            width: parent.width
                            spacing: 10

                            Label {
                                width:
                                    parent.width - albumLikeButton.width - parent.spacing

                                text:
                                        root.albumTitle.length > 0
                                    ? root.albumTitle
                                    : qsTr("Альбом")

                                color:
                                    AppTheme.textPrimary

                                font.pixelSize:
                                    30

                                font.weight:
                                    Font.DemiBold

                                elide:
                                    Text.ElideRight

                                maximumLineCount:
                                    2
                            }

                            Item {
                                id: albumLikeButton

                                width: 36
                                height: 36
                                anchors.verticalCenter: parent.verticalCenter
                                visible: root.albumController !== null &&
                                         root.albumController.albumId !== ""

                                Label {
                                    anchors.centerIn: parent
                                    text: root.controller && root.controller.currentAlbumLiked
                                        ? "♥"
                                        : "♡"
                                    color: root.controller && root.controller.currentAlbumLiked
                                        ? AppTheme.accent
                                        : AppTheme.textSecondary
                                    font.pixelSize: 22
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        if (root.controller) {
                                            root.controller.toggleAlbumLike()
                                        }
                                    }
                                }
                            }
                        }


                        Label {
                            text:
                                    root.trackCount > 0
                                ? root.trackCount +
                                (
                                        root.trackCount === 1
                                        ? qsTr(" трек")
                                        : qsTr(" трека")
                                )
                                : qsTr("Нет треков")

                            color:
                                AppTheme.textSecondary

                            font.pixelSize:
                                13
                        }
                    }
                }
            }


            /*
             * =====================================================
             * Tracks panel
             * =====================================================
             */

            Rectangle {
                id: tracksPanel

                width:
                    parent.width

                height:
                        root.trackCount > 0
                    ? 14 +
                    26 +
                    10 +
                    (
                        Math.min(
                            root.trackCount,
                            6
                        ) * 64
                    ) +
                    (
                        Math.max(
                            Math.min(
                                root.trackCount,
                                6
                            ) - 1,
                            0
                        ) * 4
                    ) +
                    14
                    : 14 + 26 + 10 + 72 + 14

                radius:
                    12

                color:
                    AppTheme.panel

                border.width:
                    1

                border.color:
                    AppTheme.borderSubtle


                Column {
                    anchors.fill:
                        parent

                    anchors.leftMargin:
                        16

                    anchors.rightMargin:
                        16

                    anchors.topMargin:
                        14

                    anchors.bottomMargin:
                        14

                    spacing:
                        10


                    /*
                     * ------------------------------------------------
                     * Panel title
                     * ------------------------------------------------
                     */

                    Label {
                        width:
                            parent.width

                        height:
                            26

                        text:
                            qsTr("Треки")

                        color:
                            AppTheme.textPrimary

                        font.pixelSize:
                            22

                        font.bold:
                            true

                        verticalAlignment:
                            Text.AlignVCenter
                    }


                    /*
                     * ------------------------------------------------
                     * Track list
                     * ------------------------------------------------
                     */

                    ListView {
                        id: tracksView

                        width:
                            parent.width

                        height:
                                root.trackCount > 0
                            ? Math.min(
                                root.trackCount,
                                6
                            ) * 64 +
                            Math.max(
                                Math.min(
                                    root.trackCount,
                                    6
                                ) - 1,
                                0
                            ) * 4
                            : 72

                        model:
                            root.albumModel

                        clip:
                            true

                        spacing:
                            4

                        interactive:
                            root.trackCount > 6

                        boundsBehavior:
                            Flickable.StopAtBounds


                        ScrollBar.vertical:
                            ScrollBar {
                                policy:
                                        tracksView.count > 6
                                    ? ScrollBar.AsNeeded
                                    : ScrollBar.AlwaysOff
                            }


                        delegate:
                            Rectangle {
                                id: trackRow

                                required property int index
                                required property string trackId
                                required property string title
                                required property string artist
                                required property string artistId
                                required property string albumId
                                required property string coverUri
                                required property int durationMs
                                required property bool liked

                                width:
                                    tracksView.width -
                                    (
                                            tracksView.count > 6
                                            ? 8
                                            : 0
                                    )

                                height:
                                    64

                                radius:
                                    8

                                color:
                                    root.controller &&
                                    root.controller.currentTrackId !== "" &&
                                    trackRow.trackId ===
                                        root.controller.currentTrackId
                                        ? AppTheme.panelActive
                                        : (
                                            trackMouse.containsMouse
                                                ? AppTheme.panelActive
                                                : AppTheme.panelSecondary
                                        )

                                border.width:
                                    root.controller &&
                                    root.controller.currentTrackId !== "" &&
                                    trackRow.trackId ===
                                        root.controller.currentTrackId
                                        ? 1
                                        : 0

                                border.color:
                                    AppTheme.accent


                                /*
                                 * ----------------------------------------
                                 * Artwork
                                 * ----------------------------------------
                                 */

                                Rectangle {
                                    id: trackCoverContainer

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
                                        id: trackImage

                                        anchors.fill:
                                            parent

                                        source:
                                                trackRow.coverUri.length > 0
                                            ? "image://yandex/" +
                                            trackRow.coverUri
                                            : ""

                                        sourceSize:
                                            Qt.size(
                                                48,
                                                48
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
                                            AppTheme.textMuted

                                        font.pixelSize:
                                            18

                                        visible:
                                            trackImage.status !==
                                            Image.Ready
                                    }
                                }


                                /*
                                 * ----------------------------------------
                                 * Track information
                                 * ----------------------------------------
                                 */

                                Column {
                                    id: trackInfo

                                    anchors.left:
                                        trackCoverContainer.right

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


                                    Label {
                                        id: titleLabel

                                        width:
                                            parent.width

                                        text:
                                                trackRow.title.length > 0
                                            ? trackRow.title
                                            : qsTr("Без названия")

                                        color:
                                            AppTheme.textPrimary

                                        font.pixelSize:
                                            14

                                        font.bold:
                                            true

                                        elide:
                                            Text.ElideRight

                                        maximumLineCount:
                                            1
                                    }


                                    Label {
                                        id: artistLabel

                                        width:
                                            Math.min(
                                                implicitWidth,
                                                parent.width
                                            )

                                        text:
                                                trackRow.artist.length > 0
                                            ? trackRow.artist
                                            : qsTr(
                                                "Неизвестный исполнитель"
                                            )

                                        color:
                                            artistMouseArea.containsMouse
                                                ? AppTheme.accent
                                                : AppTheme.textSecondary

                                        font.pixelSize:
                                            12

                                        elide:
                                            Text.ElideRight

                                        maximumLineCount:
                                            1
                                    }
                                }


                                /*
                                 * ----------------------------------------
                                 * Duration
                                 * ----------------------------------------
                                 */

                                Label {
                                    id: durationLabel

                                    width:
                                        44

                                    anchors.right:
                                        addButton.left

                                    anchors.rightMargin:
                                        8

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    text:
                                        root.formatDuration(
                                            trackRow.durationMs
                                        )

                                    color:
                                        AppTheme.textMuted

                                    font.pixelSize:
                                        11

                                    horizontalAlignment:
                                        Text.AlignRight
                                }


                                /*
                                 * ----------------------------------------
                                 * Add to playlist
                                 * ----------------------------------------
                                 */

                                Item {
                                    id: addButton

                                    width:
                                        30

                                    height:
                                        30

                                    anchors.right:
                                        likeButton.left

                                    anchors.rightMargin:
                                        4

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    z:
                                        25

                                    Label {
                                        anchors.centerIn:
                                            parent

                                        text:
                                            "+"

                                        color:
                                            addMouse.containsMouse
                                                ? AppTheme.accent
                                                : AppTheme.textMuted

                                        font.pixelSize:
                                            20

                                        font.bold:
                                            true
                                    }


                                    MouseArea {
                                        id: addMouse

                                        anchors.fill:
                                            parent

                                        hoverEnabled:
                                            true

                                        cursorShape:
                                            Qt.PointingHandCursor

                                        onClicked: {
                                            addToPlaylistPopup.trackId =
                                                trackRow.trackId

                                            addToPlaylistPopup.albumId =
                                                trackRow.albumId

                                            addToPlaylistPopup.open()
                                        }
                                    }
                                }


                                /*
                                 * ----------------------------------------
                                 * Whole row click
                                 * ----------------------------------------
                                 */

                                MouseArea {
                                    id: trackMouse

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
                                            root.albumController === null
                                        ) {
                                            return
                                        }

                                        root.albumController.selectAlbumTrack(
                                            trackRow.index
                                        )
                                    }
                                }


                                /*
                                 * ----------------------------------------
                                 * Artist link
                                 * ----------------------------------------
                                 */

                                MouseArea {
                                    id: artistMouseArea

                                    x:
                                        artistLabel.x

                                    y:
                                        artistLabel.y

                                    width:
                                        artistLabel.width

                                    height:
                                        artistLabel.height

                                    hoverEnabled:
                                        true

                                    enabled:
                                        trackRow.artistId.length > 0

                                    cursorShape:
                                        Qt.PointingHandCursor

                                    z:
                                        1

                                    onClicked: {
                                        if (
                                            root.controller === null ||
                                            root.controller === undefined
                                        ) {
                                            return
                                        }

                                        root.controller.loadArtist(
                                            trackRow.artistId
                                        )
                                    }
                                }


                                /*
                                 * ----------------------------------------
                                 * Like
                                 * ----------------------------------------
                                 */

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
                                            trackRow.liked
                                                ? "♥"
                                                : "♡"

                                        color:
                                            trackRow.liked
                                                ? AppTheme.accent
                                                : AppTheme.textSecondary

                                        font.pixelSize:
                                            20
                                    }


                                    MouseArea {
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

                                            root.controller.toggleLike(
                                                trackRow.trackId,
                                                trackRow.liked
                                            )
                                        }
                                    }
                                }
                            }
                    }


                    /*
                     * ------------------------------------------------
                     * Empty state
                     * ------------------------------------------------
                     */

                    Rectangle {
                        width:
                            parent.width

                        height:
                            72

                        radius:
                            8

                        color:
                            AppTheme.panelSecondary

                        border.width:
                            1

                        border.color:
                            AppTheme.borderSubtle

                        visible:
                            root.trackCount === 0 &&
                            !root.loading


                        Label {
                            anchors.centerIn:
                                parent

                            text:
                                qsTr("Треки не найдены")

                            color:
                                AppTheme.textMuted

                            font.pixelSize:
                                14
                        }
                    }
                }
            }


            /*
             * =====================================================
             * Bottom spacing
             * =====================================================
             */

            Item {
                width:
                    1

                height:
                    100
            }
        }
    }


    /*
     * =============================================================
     * Loading
     * =============================================================
     */

    BusyIndicator {
        id: loadingIndicator

        anchors.centerIn:
            parent

        width:
            32

        height:
            32

        running:
            root.loading

        visible:
            running

        z:
            100
    }


    /*
     * =============================================================
     * Helpers
     * =============================================================
     */

    function formatDuration(milliseconds) {
        var value =
            Number(milliseconds)

        if (
            !isFinite(value) ||
            value <= 0
        ) {
            return "0:00"
        }

        var totalSeconds =
            Math.floor(
                value / 1000
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