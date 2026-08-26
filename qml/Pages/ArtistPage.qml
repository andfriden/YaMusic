import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    property var controller

    /*
     * ============================================================
     * Controllers / models
     * ============================================================
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
     * ============================================================
     * Artist state
     * ============================================================
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
     * ============================================================
     * Background
     * ============================================================
     */

    Rectangle {
        anchors.fill: parent
        color: "#f5f5f5"
    }

    /*
     * ============================================================
     * Main scroll
     *
     * IMPORTANT:
     * No fixed page height.
     * Column grows according to its contents.
     * ============================================================
     */

    ScrollView {
        id: pageScroll

        anchors.fill: parent

        clip: true

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        Column {
            id: pageColumn

            width: pageScroll.availableWidth

            spacing: 20

            /*
             * ====================================================
             * Artist header
             * ====================================================
             */

            Rectangle {
                width: parent.width
                height: 260

                radius: 14

                color: "#e9e9e9"

                border.width: 1
                border.color: "#d6d6d6"

                Row {
                    anchors.fill: parent
                    anchors.margins: 20

                    spacing: 24

                    /*
                     * ------------------------------------------------
                     * Artist artwork
                     * ------------------------------------------------
                     */

                    Item {
                        id: artistArtwork

                        width: 220
                        height: 220

                        Rectangle {
                            anchors.fill: parent

                            radius: width / 2

                            color: "#d0d0d0"

                            clip: true

                            Image {
                                id: artistImage

                                anchors.fill: parent

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

                                asynchronous: true
                                cache: true

                                visible:
                                    status === Image.Ready
                            }

                            Label {
                                anchors.centerIn: parent

                                text: "♪"

                                color: "#777777"

                                font.pixelSize: 54

                                visible:
                                    artistImage.status !==
                                    Image.Ready
                            }
                        }

                        Rectangle {
                            anchors.fill: parent

                            radius: width / 2

                            color: "transparent"

                            border.width: 1
                            border.color: "#c8c8c8"
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

                        anchors.verticalCenter: parent.verticalCenter

                        spacing: 10

                        Label {
                            text: qsTr("ИСПОЛНИТЕЛЬ")

                            color: "#888888"

                            font.pixelSize: 12
                            font.bold: true
                        }

                        Label {
                            width: parent.width

                            text:
                                    root.artistName.length > 0
                                ? root.artistName
                                : qsTr("Исполнитель")

                            color: "#202020"

                            font.pixelSize: 32
                            font.bold: true

                            elide: Text.ElideRight
                        }

                        Label {
                            width: parent.width

                            text: root.artistGenres

                            color: "#666666"

                            font.pixelSize: 13

                            wrapMode: Text.WordWrap

                            maximumLineCount: 2

                            visible: text.length > 0
                        }

                        Label {
                            width: parent.width

                            text: root.artistDescription

                            color: "#666666"

                            font.pixelSize: 12

                            wrapMode: Text.WordWrap

                            maximumLineCount: 3

                            elide: Text.ElideRight

                            visible: text.length > 0
                        }

                        Button {
                            width: 140
                            height: 38

                            text: qsTr("▶  Слушать")

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

                width: parent.width

                spacing: 20

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

                    radius: 12

                    color: "#e9e9e9"

                    border.width: 1
                    border.color: "#d6d6d6"

                    Column {
                        anchors.fill: parent

                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        anchors.topMargin: 14
                        anchors.bottomMargin: 14

                        spacing: 10

                        Label {
                            width: parent.width

                            text:
                                qsTr("Популярные треки")

                            color: "#202020"

                            font.pixelSize: 22
                            font.bold: true
                        }

                        ListView {
                            id: artistTracksView

                            width: parent.width

                            height:
                                Math.max(
                                    Math.min(
                                        count,
                                        4
                                    ) * 68,
                                    1
                                )

                            model: root.artistModel

                            interactive:
                                count > 4

                            clip: true

                            spacing: 4

                            ScrollBar.vertical: ScrollBar {
                                policy:
                                        artistTracksView.count > 4
                                    ? ScrollBar.AsNeeded
                                    : ScrollBar.AlwaysOff
                            }

                            delegate: Rectangle {
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

                                height: 64

                                radius: 8

                                color:
                                    trackMouse.containsMouse
                                        ? "#dddddd"
                                        : "#f1f1f1"

                                border.width: 1

                                border.color:
                                    trackMouse.containsMouse
                                        ? "#c8c8c8"
                                        : "#dfdfdf"

                                Image {
                                    id: trackImage

                                    width: 48
                                    height: 48

                                    anchors.left: parent.left
                                    anchors.leftMargin: 8

                                    anchors.verticalCenter:
                                        parent.verticalCenter

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

                                    asynchronous: true
                                    cache: true

                                    Rectangle {
                                        anchors.fill: parent

                                        radius: 6

                                        color: "#d0d0d0"

                                        visible:
                                            trackImage.status !==
                                            Image.Ready

                                        Label {
                                            anchors.centerIn: parent

                                            text: "♪"

                                            color: "#777777"

                                            font.pixelSize: 18
                                        }
                                    }
                                }

                                Column {
                                    anchors.left:
                                        trackImage.right

                                    anchors.leftMargin: 12

                                    anchors.right:
                                        durationLabel.left

                                    anchors.rightMargin: 12

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    spacing: 2

                                    Label {
                                        width: parent.width

                                        text:
                                                trackRow.title.length > 0
                                            ? trackRow.title
                                            : qsTr("Без названия")

                                        color: "#202020"

                                        font.pixelSize: 14
                                        font.bold: true

                                        elide: Text.ElideRight
                                    }

                                    Label {
                                        width: parent.width

                                        text:
                                                trackRow.artist.length > 0
                                            ? trackRow.artist
                                            : qsTr(
                                                "Неизвестный исполнитель"
                                            )

                                        color: "#555555"

                                        font.pixelSize: 12

                                        elide: Text.ElideRight
                                    }
                                }

                                Label {
                                    id: durationLabel

                                    anchors.right: parent.right
                                    anchors.rightMargin: 14

                                    anchors.verticalCenter:
                                        parent.verticalCenter

                                    text:
                                        root.formatDuration(
                                            trackRow.durationMs
                                        )

                                    color: "#666666"

                                    font.pixelSize: 11
                                }

                                MouseArea {
                                    id: trackMouse

                                    anchors.fill: parent

                                    hoverEnabled: true

                                    cursorShape:
                                        Qt.PointingHandCursor

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

                    height: tracksPanel.height

                    radius: 12

                    color: "#e9e9e9"

                    border.width: 1
                    border.color: "#d6d6d6"

                    visible:
                        root.newReleaseTitle.length > 0

                    Column {
                        anchors.fill: parent

                        anchors.margins: 14

                        spacing: 12

                        Label {
                            width: parent.width

                            text: qsTr("ПОСЛЕДНИЙ РЕЛИЗ")

                            color: "#888888"

                            font.pixelSize: 11
                            font.bold: true
                        }

                        Rectangle {
                            width:
                                Math.min(
                                    parent.width,
                                    180
                                )

                            height: width

                            anchors.horizontalCenter:
                                parent.horizontalCenter

                            radius: 10

                            color: "#d0d0d0"

                            clip: true

                            Image {
                                anchors.fill: parent

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

                                asynchronous: true
                                cache: true

                                visible:
                                    status === Image.Ready
                            }

                            Label {
                                anchors.centerIn: parent

                                text: "♪"

                                color: "#777777"

                                font.pixelSize: 34

                                visible:
                                    root.newReleaseCoverUri.length === 0
                            }
                        }

                        Label {
                            width: parent.width

                            text:
                                root.newReleaseTitle

                            color: "#202020"

                            font.pixelSize: 20
                            font.bold: true

                            wrapMode: Text.WordWrap

                            maximumLineCount: 2

                            elide: Text.ElideRight

                            horizontalAlignment:
                                Text.AlignHCenter
                        }

                        Label {
                            width: parent.width

                            text:
                                    root.newReleaseYear > 0
                                ? String(
                                    root.newReleaseYear
                                )
                                : ""

                            color: "#666666"

                            font.pixelSize: 12

                            horizontalAlignment:
                                Text.AlignHCenter

                            visible:
                                root.newReleaseYear > 0
                        }
                    }
                }
            }

            /*
             * ====================================================
             * Popular albums
             *
             * 5 albums per row.
             *
             * ====================================================
             */

            Rectangle {
                id: albumsPanel

                width: parent.width

                height:
                        root.albumsModel !== null
                    ? 78 +
                    Math.max(
                        Math.ceil(
                            albumsView.count / 5
                        ) * 158,
                        1
                    )
                    : 150

                radius: 12

                color: "#e9e9e9"

                border.width: 1
                border.color: "#d6d6d6"

                Column {
                    anchors.fill: parent

                    anchors.margins: 14

                    spacing: 8

                    Label {
                        text:
                            qsTr(
                                "Популярные альбомы"
                            )

                        color: "#202020"

                        font.pixelSize: 22
                        font.bold: true
                    }

                    GridView {
                        id: albumsView

                        width: parent.width

                        height:
                            Math.max(
                                Math.ceil(
                                    count / 5
                                ) * 150,
                                1
                            )

                        model:
                            root.albumsModel

                        /*
                         * Five columns.
                         */
                        cellWidth:
                                width > 0
                            ? Math.floor(
                                width / 5
                            )
                            : 1

                        cellHeight:
                            150

                        interactive: false

                        clip: true

                        delegate: Item {
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

                            /*
                             * ------------------------------------------------
                             * Cover
                             * ------------------------------------------------
                             */

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

                                radius: 9

                                color: "#d0d0d0"

                                clip: true

                                Image {
                                    anchors.fill: parent

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

                                    asynchronous: true
                                    cache: true

                                    visible:
                                        status === Image.Ready
                                }

                                Label {
                                    anchors.centerIn: parent

                                    text: "♪"

                                    color: "#777777"

                                    font.pixelSize: 24

                                    visible:
                                        albumDelegate.coverUri.length === 0
                                }

                                MouseArea {
                                    anchors.fill: parent

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

                            /*
                             * ------------------------------------------------
                             * Album title
                             * ------------------------------------------------
                             */

                            Column {
                                width:
                                    albumCover.width

                                anchors.top:
                                    albumCover.bottom

                                anchors.topMargin:
                                    4

                                anchors.horizontalCenter:
                                    parent.horizontalCenter

                                spacing: 1

                                Label {
                                    width: parent.width

                                    text:
                                        albumDelegate.title

                                    color: "#202020"

                                    font.pixelSize: 12
                                    font.bold: true

                                    elide:
                                        Text.ElideRight

                                    maximumLineCount: 1
                                }

                                Label {
                                    width: parent.width

                                    text:
                                            albumDelegate.year > 0
                                        ? String(
                                            albumDelegate.year
                                        )
                                        : ""

                                    color: "#777777"

                                    font.pixelSize: 11

                                    visible:
                                        text.length > 0
                                }
                            }
                        }

                        Label {
                            anchors.centerIn: parent

                            text:
                                root.loading
                                    ? qsTr("Загрузка...")
                                    : qsTr(
                                        "Нет доступных альбомов"
                                    )

                            color: "#999999"

                            font.pixelSize: 12

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
                width: 1
                height: 30
            }
        }
    }

    /*
     * ============================================================
     * Loading overlay
     * ============================================================
     */

    Rectangle {
        anchors.fill: parent

        color: "#f5f5f5"

        opacity: 0.96

        visible:
            root.loading &&
            root.artistName.length === 0

        z: 100

        Column {
            anchors.centerIn: parent

            spacing: 10

            BusyIndicator {
                width: 32
                height: 32

                anchors.horizontalCenter:
                    parent.horizontalCenter

                running: true
            }

            Label {
                text:
                    qsTr(
                        "Загрузка исполнителя..."
                    )

                color: "#666666"

                font.pixelSize: 13

                anchors.horizontalCenter:
                    parent.horizontalCenter
            }
        }
    }

    /*
     * ============================================================
     * Empty state
     * ============================================================
     */

    Label {
        anchors.centerIn: parent

        text:
            qsTr(
                "Выберите исполнителя"
            )

        color: "#999999"

        font.pixelSize: 14

        visible:
            !root.loading &&
            root.artistName.length === 0

        z: 101
    }

    /*
     * ============================================================
     * Diagnostics
     * ============================================================
     */

    Component.onCompleted: {
        console.log(
            "========================================"
        )

        console.log(
            "ArtistPage CREATED"
        )

        console.log(
            "width:",
            root.width
        )

        console.log(
            "height:",
            root.height
        )

        console.log(
            "controller:",
            root.controller
        )

        console.log(
            "artistController:",
            root.artistController
        )

        console.log(
            "artistModel:",
            root.artistModel
        )

        console.log(
            "albumsModel:",
            root.albumsModel
        )

        console.log(
            "similarArtistsModel:",
            root.similarArtistsModel
        )

        console.log(
            "artist:",
            root.artistName
        )

        console.log(
            "tracks:",
            artistTracksView.count
        )

        console.log(
            "albums:",
            albumsView.count
        )

        console.log(
            "new release:",
            root.newReleaseTitle
        )

        console.log(
            "========================================"
        )
    }

    /*
     * ============================================================
     * Helpers
     * ============================================================
     */

    function formatDuration(milliseconds) {
        var value =
            Number(
                milliseconds
            )

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