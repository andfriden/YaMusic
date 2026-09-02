import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    /*
     * =============================================================
     * Page size
     * =============================================================
     *
     * MainLayout использует pageHeight для Loader.
     * Контент внутри страницы продолжает прокручиваться
     * независимо от этого значения.
     */

    readonly property int pageHeight: 900

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


    /*
     * =============================================================
     * Controllers / models
     * =============================================================
     */

    readonly property var artistController:
            root.controller !== null &&
        root.controller !== undefined &&
        root.controller.artistController !== null &&
        root.controller.artistController !== undefined
        ? root.controller.artistController
        : null

    readonly property var artistModel:
            root.artistController !== null &&
        root.artistController.artistModel !== null &&
        root.artistController.artistModel !== undefined
        ? root.artistController.artistModel
        : null

    readonly property var albumsModel:
            root.artistController !== null &&
        root.artistController.albumsModel !== null &&
        root.artistController.albumsModel !== undefined
        ? root.artistController.albumsModel
        : null

    readonly property var similarArtistsModel:
            root.artistController !== null &&
        root.artistController.similarArtistsModel !== null &&
        root.artistController.similarArtistsModel !== undefined
        ? root.artistController.similarArtistsModel
        : null


    /*
     * =============================================================
     * Artist state
     * =============================================================
     */

    readonly property bool loading:
        root.artistController !== null &&
        root.artistController.loading === true

    readonly property string artistId:
            root.artistController !== null
        ? String(
            root.artistController.artistId || ""
        )
        : ""

    readonly property string artistName:
            root.artistController !== null
        ? String(
            root.artistController.artistName || ""
        )
        : ""

    readonly property string artistCoverUri:
            root.artistController !== null
        ? String(
            root.artistController.artistCoverUri || ""
        )
        : ""

    readonly property string artistDescription:
            root.artistController !== null
        ? String(
            root.artistController.artistDescription || ""
        )
        : ""

    readonly property string artistGenres:
            root.artistController !== null
        ? String(
            root.artistController.artistGenres || ""
        )
        : ""

    readonly property string newReleaseId:
            root.artistController !== null
        ? String(
            root.artistController.newReleaseId || ""
        )
        : ""

    readonly property string newReleaseTitle:
            root.artistController !== null
        ? String(
            root.artistController.newReleaseTitle || ""
        )
        : ""

    readonly property string newReleaseCoverUri:
            root.artistController !== null
        ? String(
            root.artistController.newReleaseCoverUri || ""
        )
        : ""

    readonly property int newReleaseYear:
            root.artistController !== null
        ? Number(
            root.artistController.newReleaseYear || 0
        )
        : 0


    /*
     * =============================================================
     * Background
     * =============================================================
     */

    Rectangle {
        anchors.fill:
            parent

        color:
            AppTheme.backgroundPrimary
    }


    /*
     * =============================================================
     * Main scroll
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
             * ====================================================
             * Artist header
             * ====================================================
             */

            Rectangle {
                width:
                    parent.width

                height:
                    260

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
                        20

                    spacing:
                        24


                    /*
                     * ------------------------------------------------
                     * Artist artwork
                     * ------------------------------------------------
                     */

                    Item {
                        id: artistArtwork

                        width:
                            220

                        height:
                            220


                        Rectangle {
                            anchors.fill:
                                parent

                            radius:
                                width / 2

                            color:
                                AppTheme.artworkPlaceholder

                            clip:
                                true


                            Image {
                                id: artistImage

                                anchors.fill:
                                    parent

                                source:
                                        root.artistCoverUri.length > 0
                                    ? "image://yandex/circle/" +
                                    root.artistCoverUri
                                    : ""

                                sourceSize:
                                    Qt.size(
                                        220,
                                        220
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
                                    54

                                visible:
                                    artistImage.status !==
                                    Image.Ready
                            }
                        }


                        Rectangle {
                            anchors.fill:
                                parent

                            radius:
                                width / 2

                            color:
                                "transparent"

                            border.width:
                                1

                            border.color:
                                AppTheme.border
                        }
                    }


                    /*
                     * ------------------------------------------------
                     * Artist info
                     * ------------------------------------------------
                     */

                    Column {
                        width:
                            parent.width -
                            artistArtwork.width -
                            parent.spacing

                        anchors.verticalCenter:
                            parent.verticalCenter

                        spacing:
                            10


                        Label {
                            text:
                                qsTr("ИСПОЛНИТЕЛЬ")

                            color:
                                AppTheme.textMuted

                            font.pixelSize:
                                12

                            font.bold:
                                true
                        }


                        Label {
                            width:
                                parent.width

                            text:
                                    root.artistName.length > 0
                                ? root.artistName
                                : qsTr("Исполнитель")

                            color:
                                AppTheme.textPrimary

                            font.pixelSize:
                                32

                            font.bold:
                                true

                            elide:
                                Text.ElideRight

                            maximumLineCount:
                                1
                        }


                        Label {
                            width:
                                parent.width

                            text:
                                root.artistGenres

                            color:
                                AppTheme.textSecondary

                            font.pixelSize:
                                13

                            wrapMode:
                                Text.WordWrap

                            maximumLineCount:
                                2

                            visible:
                                text.length > 0
                        }


                        Label {
                            width:
                                parent.width

                            text:
                                root.artistDescription

                            color:
                                AppTheme.textSecondary

                            font.pixelSize:
                                12

                            wrapMode:
                                Text.WordWrap

                            maximumLineCount:
                                3

                            elide:
                                Text.ElideRight

                            visible:
                                text.length > 0
                        }


                        Button {
                            width:
                                140

                            height:
                                38

                            text:
                                qsTr("▶  Слушать")

                            enabled:
                                !root.loading &&
                                root.artistController !== null &&
                                root.artistModel !== null &&
                                artistTracksView.count > 0

                            onClicked: {
                                if (
                                    root.artistController === null
                                ) {
                                    return
                                }

                                root.artistController.playArtist()
                            }
                        }
                    }
                }
            }


            /*
             * ====================================================
             * Popular tracks + New release
             * ====================================================
             */

            Row {
                id: tracksAndReleaseRow

                width:
                    parent.width

                spacing:
                    20


                /*
                 * =================================================
                 * Popular tracks
                 * =================================================
                 */

                Rectangle {
                    id: tracksPanel

                    width:
                        Math.max(
                            0,
                            (
                                parent.width -
                                parent.spacing
                            ) * 0.68
                        )

                    height:
                        76 +
                        (
                            Math.min(
                                artistTracksView.count,
                                4
                            ) * 68
                        )

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


                        Label {
                            width:
                                parent.width

                            text:
                                qsTr("Популярные треки")

                            color:
                                AppTheme.textPrimary

                            font.pixelSize:
                                22

                            font.bold:
                                true
                        }


                        ListView {
                            id: artistTracksView

                            width:
                                parent.width

                            height:
                                Math.max(
                                    Math.min(
                                        count,
                                        4
                                    ) * 68,
                                    1
                                )

                            model:
                                root.artistModel

                            interactive:
                                count > 4

                            clip:
                                true

                            spacing:
                                4

                            ScrollBar.vertical:
                                ScrollBar {
                                    policy:
                                            artistTracksView.count > 4
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
                                    required property string coverUri
                                    required property int durationMs

                                    width:
                                        artistTracksView.width

                                    height:
                                        64

                                    radius:
                                        8

                                    color:
                                        trackMouse.containsMouse
                                            ? AppTheme.panelActive
                                            : AppTheme.panelSecondary

                                    border.width:
                                        1

                                    border.color:
                                        trackMouse.containsMouse
                                            ? AppTheme.border
                                            : AppTheme.borderSubtle


                                    /*
                                     * -----------------------------------------
                                     * Cover
                                     * -----------------------------------------
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
                                                status ===
                                                Image.Ready
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
                                     * -----------------------------------------
                                     * Track text
                                     * -----------------------------------------
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
                                                : qsTr(
                                                    "Без названия"
                                                )

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
                                     * -----------------------------------------
                                     * Duration
                                     * -----------------------------------------
                                     */

                                    Label {
                                        id: durationLabel

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
                                     * -----------------------------------------
                                     * Whole row click
                                     * -----------------------------------------
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
                                                root.artistController === null
                                            ) {
                                                return
                                            }

                                            root.artistController.selectTrack(
                                                trackRow.index
                                            )
                                        }
                                    }


                                    /*
                                     * -----------------------------------------
                                     * Artist click
                                     *
                                     * Only the artist name is clickable.
                                     * -----------------------------------------
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

                                        onClicked: {
                                            root.controller.loadArtist(
                                                trackRow.artistId
                                            )
                                        }
                                    }
                                }
                        }
                    }
                }


                /*
                 * =================================================
                 * New release
                 * =================================================
                 */

                Rectangle {
                    id: newReleasePanel

                    width:
                        Math.max(
                            0,
                            (
                                parent.width -
                                parent.spacing
                            ) * 0.32
                        )

                    height:
                        tracksPanel.height

                    radius:
                        12

                    color:
                        releaseMouseArea.containsMouse
                            ? AppTheme.panelHover
                            : AppTheme.panel

                    border.width:
                        1

                    border.color:
                        releaseMouseArea.containsMouse
                            ? AppTheme.border
                            : AppTheme.borderSubtle

                    visible:
                        root.newReleaseTitle.length > 0


                    Column {
                        anchors.fill:
                            parent

                        anchors.margins:
                            14

                        spacing:
                            12


                        Label {
                            width:
                                parent.width

                            text:
                                qsTr("ПОСЛЕДНИЙ РЕЛИЗ")

                            color:
                                AppTheme.textMuted

                            font.pixelSize:
                                11

                            font.bold:
                                true
                        }


                        Rectangle {
                            width:
                                Math.min(
                                    parent.width,
                                    180
                                )

                            height:
                                width

                            anchors.horizontalCenter:
                                parent.horizontalCenter

                            radius:
                                10

                            color:
                                AppTheme.artworkPlaceholder

                            clip:
                                true


                            Image {
                                id: newReleaseImage

                                anchors.fill:
                                    parent

                                source:
                                        root.newReleaseCoverUri.length > 0
                                    ? "image://yandex/" +
                                    root.newReleaseCoverUri
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
                                    34

                                visible:
                                    newReleaseImage.status !==
                                    Image.Ready
                            }
                        }


                        Label {
                            width:
                                parent.width

                            text:
                                root.newReleaseTitle

                            color:
                                AppTheme.textPrimary

                            font.pixelSize:
                                20

                            font.bold:
                                true

                            wrapMode:
                                Text.WordWrap

                            maximumLineCount:
                                2

                            elide:
                                Text.ElideRight

                            horizontalAlignment:
                                Text.AlignHCenter
                        }


                        Label {
                            width:
                                parent.width

                            text:
                                    root.newReleaseYear > 0
                                ? String(
                                    root.newReleaseYear
                                )
                                : ""

                            color:
                                AppTheme.textSecondary

                            font.pixelSize:
                                12

                            horizontalAlignment:
                                Text.AlignHCenter

                            visible:
                                root.newReleaseYear > 0
                        }
                    }


                    MouseArea {
                        id: releaseMouseArea

                        anchors.fill:
                            parent

                        hoverEnabled:
                            true

                        enabled:
                            root.newReleaseId.length > 0

                        cursorShape:
                            enabled
                                ? Qt.PointingHandCursor
                                : Qt.ArrowCursor

                        onClicked: {
                            if (
                                root.controller === null ||
                                root.controller === undefined
                            ) {
                                return
                            }

                            if (
                                root.newReleaseId.length === 0
                            ) {
                                return
                            }

                            root.controller.loadAlbum(
                                root.newReleaseId
                            )
                        }
                    }
                }
            }


            /*
             * ====================================================
             * Popular albums
             * ====================================================
             */

            Rectangle {
                id: albumsPanel

                width:
                    parent.width

                height:
                        root.albumsModel !== null
                    ? 78 +
                    Math.max(
                        Math.ceil(
                            albumsView.count / 5
                        ) * 210,
                        1
                    )
                    : 150

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

                    anchors.margins:
                        14

                    spacing:
                        8


                    Label {
                        width:
                            parent.width

                        text:
                            qsTr("Популярные альбомы")

                        color:
                            AppTheme.textPrimary

                        font.pixelSize:
                            22

                        font.bold:
                            true
                    }


                    GridView {
                        id: albumsView

                        width:
                            parent.width

                        height:
                            Math.max(
                                Math.ceil(
                                    count / 5
                                ) * 175,
                                1
                            )

                        model:
                            root.albumsModel

                        cellWidth:
                                width > 0
                            ? Math.floor(
                                width / 5
                            )
                            : 1

                        cellHeight:
                            180

                        interactive:
                            false

                        clip:
                            true


                        delegate:
                            Item {
                                id: albumDelegate

                                width:
                                    albumsView.cellWidth

                                height:
                                    albumsView.cellHeight

                                required property int index
                                required property string albumId
                                required property string title
                                required property string coverUri
                                required property int year


                                Rectangle {
                                    id: albumCover

                                    width:
                                        Math.min(
                                            124,
                                            albumsView.cellWidth - 8
                                        )

                                    height:
                                        width

                                    anchors.horizontalCenter:
                                        parent.horizontalCenter

                                    radius:
                                        9

                                    color:
                                        AppTheme.artworkPlaceholder

                                    clip:
                                        true


                                    Image {
                                        id: albumImage

                                        anchors.fill:
                                            parent

                                        source:
                                                albumDelegate.coverUri.length > 0
                                            ? "image://yandex/" +
                                            albumDelegate.coverUri
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


                                    Label {
                                        anchors.centerIn:
                                            parent

                                        text:
                                            "♪"

                                        color:
                                            AppTheme.textMuted

                                        font.pixelSize:
                                            24

                                        visible:
                                            albumImage.status !==
                                            Image.Ready
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

                                            if (
                                                albumDelegate.albumId.length === 0
                                            ) {
                                                return
                                            }

                                            root.controller.loadAlbum(
                                                albumDelegate.albumId
                                            )
                                        }
                                    }
                                }


                                Column {
                                    width:
                                        albumCover.width

                                    anchors.top:
                                        albumCover.bottom

                                    anchors.topMargin:
                                        4

                                    anchors.horizontalCenter:
                                        parent.horizontalCenter

                                    spacing:
                                        1


                                    Label {
                                        width:
                                            parent.width

                                        text:
                                            albumDelegate.title

                                        color:
                                            AppTheme.textPrimary

                                        font.pixelSize:
                                            12

                                        font.bold:
                                            true

                                        elide:
                                            Text.ElideRight

                                        maximumLineCount:
                                            1
                                    }


                                    Label {
                                        width:
                                            parent.width

                                        text:
                                                albumDelegate.year > 0
                                            ? String(
                                                albumDelegate.year
                                            )
                                            : ""

                                        color:
                                            AppTheme.textMuted

                                        font.pixelSize:
                                            11

                                        visible:
                                            text.length > 0
                                    }
                                }
                            }


                        Label {
                            anchors.centerIn:
                                parent

                            text:
                                root.loading
                                    ? qsTr("Загрузка...")
                                    : qsTr(
                                        "Нет доступных альбомов"
                                    )

                            color:
                                AppTheme.textMuted

                            font.pixelSize:
                                12

                            visible:
                                albumsView.count === 0
                        }
                    }
                }
            }


            /*
             * ====================================================
             * Bottom spacing
             * ====================================================
             */

            Item {
                width:
                    1

                height:
                    40
            }
        }
    }


    /*
     * ============================================================
     * Initial loading indicator
     *
     * Does NOT cover the entire page.
     * ============================================================
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
            root.loading &&
            root.artistName.length === 0

        visible:
            running

        z:
            100
    }


    /*
     * ============================================================
     * Empty state
     * ============================================================
     */

    Label {
        anchors.centerIn:
            parent

        text:
            qsTr("Выберите исполнителя")

        color:
            AppTheme.textMuted

        font.pixelSize:
            14

        visible:
            !root.loading &&
            root.artistName.length === 0

        z:
            101
    }

    /*
     * ============================================================
     * Helpers
     * ============================================================
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