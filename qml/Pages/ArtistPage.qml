import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    width: parent ? parent.width : 0
    implicitWidth: width
    implicitHeight: pageColumn.implicitHeight + 56

    readonly property var artistController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.artistController !== null && root.controller.artistController !== undefined
            ? root.controller.artistController
            : null

    readonly property var artistModel:
        root.artistController !== null &&
        root.artistController.artistModel !== null && root.artistController.artistModel !== undefined
            ? root.artistController.artistModel
            : null

    readonly property var albumsModel:
        root.artistController !== null &&
        root.artistController.albumsModel !== null && root.artistController.albumsModel !== undefined
            ? root.artistController.albumsModel
            : null

    readonly property bool loading:
        root.artistController !== null && root.artistController.loading === true

    readonly property string artistId:
        root.artistController !== null
            ? String(root.artistController.artistId || "")
            : ""

    readonly property string artistName:
        root.artistController !== null
            ? String(root.artistController.artistName || "")
            : ""

    readonly property string artistCoverUri:
        root.artistController !== null
            ? String(root.artistController.artistCoverUri || "")
            : ""

    readonly property string artistDescription:
        root.artistController !== null
            ? String(root.artistController.artistDescription || "")
            : ""

    readonly property string artistGenres:
        root.artistController !== null
            ? String(root.artistController.artistGenres || "")
            : ""

    readonly property string newReleaseId:
        root.artistController !== null
            ? String(root.artistController.newReleaseId || "")
            : ""

    readonly property string newReleaseTitle:
        root.artistController !== null
            ? String(root.artistController.newReleaseTitle || "")
            : ""

    readonly property string newReleaseCoverUri:
        root.artistController !== null
            ? String(root.artistController.newReleaseCoverUri || "")
            : ""

    readonly property int newReleaseYear:
        root.artistController !== null
            ? Number(root.artistController.newReleaseYear || 0)
            : 0

    PlaylistPicker {
        id: addToPlaylistPopup
        controller: root.controller
    }

    // Вложенного ScrollView нет: вертикальным скроллом владеет внешний
    // ScrollView в MainLayout
    Column {
        id: pageColumn

        width: root.width
        spacing: 20

        Rectangle {
            width: parent.width
            height: 260

            radius: 14
            color: AppTheme.panel
            border.width: 1
            border.color: AppTheme.borderSubtle

            Row {
                anchors.fill: parent
                anchors.margins: 20

                spacing: 24

                Item {
                    id: artistArtwork

                    width: 220
                    height: 220

                    Rectangle {
                        anchors.fill: parent
                        radius: width / 2

                        color: AppTheme.artworkPlaceholder
                        clip: true

                        Image {
                            id: artistImage

                            anchors.fill: parent

                            source: root.artistCoverUri.length > 0
                                ? "image://yandex/circle/" + root.artistCoverUri
                                : ""

                            sourceSize: Qt.size(220, 220)

                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                            cache: true
                            smooth: true

                            visible: status === Image.Ready
                        }

                        Label {
                            anchors.centerIn: parent

                            text: "♪"
                            color: AppTheme.textMuted
                            font.pixelSize: 54

                            visible: artistImage.status !== Image.Ready
                        }
                    }

                    Rectangle {
                        anchors.fill: parent
                        radius: width / 2

                        color: "transparent"
                        border.width: 1
                        border.color: AppTheme.border
                    }
                }

                // Информация
                Column {
                    width: parent.width - artistArtwork.width - parent.spacing

                    anchors.verticalCenter: parent.verticalCenter

                    spacing: 10

                    Label {
                        text: qsTr("ИСПОЛНИТЕЛЬ")
                        color: AppTheme.textMuted
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Row {
                        width: parent.width
                        spacing: 10

                        Label {
                            width: parent.width - artistLikeButton.width - parent.spacing

                            text: root.artistName.length > 0 ? root.artistName : qsTr("Исполнитель")

                            color: AppTheme.textPrimary
                            font.pixelSize: 32
                            font.bold: true

                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }

                        Item {
                            id: artistLikeButton

                            width: 36
                            height: 36

                            anchors.verticalCenter: parent.verticalCenter

                            visible: root.controller !== null &&
                                root.controller.artistController !== null &&
                                root.controller.artistController.artistId !== ""

                            Label {
                                anchors.centerIn: parent

                                text: root.controller && root.controller.currentArtistLiked ? "♥" : "♡"
                                color: root.controller && root.controller.currentArtistLiked
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
                                        root.controller.toggleArtistLike()
                                    }
                                }
                            }
                        }
                    }

                    Label {
                        width: parent.width

                        text: root.artistGenres
                        color: AppTheme.textSecondary
                        font.pixelSize: 13

                        wrapMode: Text.WordWrap
                        maximumLineCount: 2

                        visible: text.length > 0
                    }

                    Label {
                        width: parent.width

                        text: root.artistDescription
                        color: AppTheme.textSecondary
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

                        enabled: !root.loading &&
                            root.artistController !== null &&
                            root.artistModel !== null &&
                            artistTracksView.count > 0

                        onClicked: {
                            if (root.artistController !== null) {
                                root.artistController.playArtist()
                            }
                        }
                    }
                }
            }
        }

        Row {
            id: tracksAndReleaseRow

            width: parent.width
            spacing: 20

            Rectangle {
                id: tracksPanel

                width: Math.max(0, (parent.width - parent.spacing) * 0.68)
                height: 76 + Math.min(artistTracksView.count, 4) * 68

                radius: 12
                color: AppTheme.panel
                border.width: 1
                border.color: AppTheme.borderSubtle

                Column {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    anchors.topMargin: 14
                    anchors.bottomMargin: 14

                    spacing: 10

                    Label {
                        width: parent.width

                        text: qsTr("Популярные треки")
                        color: AppTheme.textPrimary
                        font.pixelSize: 22
                        font.bold: true
                    }

                    ListView {
                        id: artistTracksView

                        width: parent.width
                        height: Math.max(Math.min(count, 4) * 68, 1)

                        model: root.artistModel

                        interactive: count > 4
                        clip: true
                        spacing: 4

                        ScrollBar.vertical: ScrollBar {
                            policy: artistTracksView.count > 4 ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                        }

                        delegate: TrackRow {
                            width: artistTracksView.width

                            controller: root.controller
                            rowIndex: model.index
                            trackId: model.trackId
                            title: model.title
                            artist: model.artist
                            artistId: model.artistId
                            albumId: model.albumId
                            coverUri: model.coverUri
                            durationMs: model.durationMs
                            showAdd: true

                            rowHeight: 64
                            coverSize: 48
                            cornerRadius: 8

                            onActivated: {
                                if (root.artistController !== null) {
                                    root.artistController.selectTrack(model.index)
                                }
                            }

                            onAddRequested: function(trackId, albumId) {
                                addToPlaylistPopup.trackId = trackId
                                addToPlaylistPopup.albumId = albumId
                                addToPlaylistPopup.open()
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: newReleasePanel

                width: Math.max(0, (parent.width - parent.spacing) * 0.32)
                height: tracksPanel.height

                radius: 12
                color: releaseMouseArea.containsMouse ? AppTheme.panelHover : AppTheme.panel

                border.width: 1
                border.color: releaseMouseArea.containsMouse ? AppTheme.border : AppTheme.borderSubtle

                visible: root.newReleaseTitle.length > 0

                Column {
                    anchors.fill: parent
                    anchors.margins: 14

                    spacing: 12

                    Label {
                        width: parent.width

                        text: qsTr("ПОСЛЕДНИЙ РЕЛИЗ")
                        color: AppTheme.textMuted
                        font.pixelSize: 11
                        font.bold: true
                    }

                    Rectangle {
                        width: Math.min(parent.width, 180)
                        height: width

                        anchors.horizontalCenter: parent.horizontalCenter

                        radius: 10
                        color: AppTheme.artworkPlaceholder
                        clip: true

                        Image {
                            id: newReleaseImage

                            anchors.fill: parent

                            source: root.newReleaseCoverUri.length > 0
                                ? "image://yandex/" + root.newReleaseCoverUri
                                : ""

                            sourceSize: Qt.size(180, 180)

                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                            cache: true
                            smooth: true

                            visible: status === Image.Ready
                        }

                        Label {
                            anchors.centerIn: parent

                            text: "♪"
                            color: AppTheme.textMuted
                            font.pixelSize: 34

                            visible: newReleaseImage.status !== Image.Ready
                        }
                    }

                    Label {
                        width: parent.width

                        text: root.newReleaseTitle
                        color: AppTheme.textPrimary
                        font.pixelSize: 20
                        font.bold: true

                        wrapMode: Text.WordWrap
                        maximumLineCount: 2

                        elide: Text.ElideRight
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Label {
                        width: parent.width

                        text: root.newReleaseYear > 0 ? String(root.newReleaseYear) : ""
                        color: AppTheme.textSecondary
                        font.pixelSize: 12

                        horizontalAlignment: Text.AlignHCenter
                        visible: root.newReleaseYear > 0
                    }
                }

                MouseArea {
                    id: releaseMouseArea

                    anchors.fill: parent
                    hoverEnabled: true

                    enabled: root.newReleaseId.length > 0
                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                    onClicked: {
                        if (root.controller === null || root.controller === undefined) {
                            return
                        }

                        if (root.newReleaseId.length === 0) {
                            return
                        }

                        root.controller.loadAlbum(root.newReleaseId)
                    }
                }
            }
        }

        Rectangle {
            id: albumsPanel

            width: parent.width

            // Высота синхронизирована с cellHeight GridView
            height: root.albumsModel !== null
                ? 78 + Math.max(Math.ceil(albumsView.count / 5) * 180, 1)
                : 150

            radius: 12
            color: AppTheme.panel
            border.width: 1
            border.color: AppTheme.borderSubtle

            Column {
                anchors.fill: parent
                anchors.margins: 14

                spacing: 8

                Label {
                    width: parent.width

                    text: qsTr("Все альбомы")
                    color: AppTheme.textPrimary
                    font.pixelSize: 22
                    font.bold: true
                }

                GridView {
                    id: albumsView

                    width: parent.width
                    height: Math.max(Math.ceil(count / 5) * 180, 1)

                    model: root.albumsModel

                    cellWidth: width > 0 ? Math.floor(width / 5) : 1
                    cellHeight: 180

                    interactive: false
                    clip: true

                    delegate: Item {
                        id: albumDelegate

                        required property string albumId
                        required property string title
                        required property string coverUri
                        required property int year

                        width: albumsView.cellWidth
                        height: albumsView.cellHeight

                        Rectangle {
                            id: albumCover

                            width: Math.min(124, albumsView.cellWidth - 8)
                            height: width

                            anchors.horizontalCenter: parent.horizontalCenter

                            radius: 9
                            color: AppTheme.artworkPlaceholder
                            clip: true

                            Image {
                                id: albumImage

                                anchors.fill: parent

                                source: albumDelegate.coverUri.length > 0
                                    ? "image://yandex/" + albumDelegate.coverUri
                                    : ""

                                sourceSize: Qt.size(124, 124)

                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                                cache: true
                                smooth: true

                                visible: status === Image.Ready
                            }

                            Label {
                                anchors.centerIn: parent

                                text: "♪"
                                color: AppTheme.textMuted
                                font.pixelSize: 24

                                visible: albumImage.status !== Image.Ready
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor

                                onClicked: {
                                    if (root.controller === null || root.controller === undefined) {
                                        return
                                    }

                                    if (albumDelegate.albumId.length === 0) {
                                        return
                                    }

                                    root.controller.loadAlbum(albumDelegate.albumId)
                                }
                            }
                        }

                        Column {
                            width: albumCover.width

                            anchors.top: albumCover.bottom
                            anchors.topMargin: 4
                            anchors.horizontalCenter: parent.horizontalCenter

                            spacing: 1

                            Label {
                                width: parent.width

                                text: albumDelegate.title
                                color: AppTheme.textPrimary
                                font.pixelSize: 12
                                font.bold: true
                                elide: Text.ElideRight
                                maximumLineCount: 1
                            }

                            Label {
                                width: parent.width

                                text: albumDelegate.year > 0 ? String(albumDelegate.year) : ""
                                color: AppTheme.textMuted
                                font.pixelSize: 11

                                visible: text.length > 0
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent

                        text: root.loading ? qsTr("Загрузка...") : qsTr("Нет доступных альбомов")
                        color: AppTheme.textMuted
                        font.pixelSize: 12

                        visible: albumsView.count === 0
                    }
                }
            }
        }

        // Нижний отступ (внутри Column, поэтому скроллится с контентом)
        Item {
            width: 1
            height: 140
        }
    }

    BusyIndicator {
        id: loadingIndicator

        anchors.centerIn: parent

        width: 32
        height: 32

        running: root.loading && root.artistName.length === 0
        visible: running
        z: 100
    }

    Label {
        anchors.centerIn: parent

        text: qsTr("Выберите исполнителя")
        color: AppTheme.textMuted
        font.pixelSize: 14

        visible: !root.loading && root.artistName.length === 0
        z: 101
    }
}