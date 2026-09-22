import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

/*
 * Страница «Медиатека»: мои плейлисты, понравившиеся треки,
 * сохранённые альбомы и любимые исполнители.
 */
Item {
    id: root

    property var controller

    width: parent ? parent.width : 0
    height: parent ? parent.height : 700
    implicitWidth: width
    implicitHeight: height

    // -------------------------------------------------------------
    // Фон
    // -------------------------------------------------------------

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    // -------------------------------------------------------------
    // Прокрутка страницы
    // -------------------------------------------------------------

    Flickable {
        id: pageFlickable

        anchors.fill: parent

        clip: true
        boundsBehavior: Flickable.StopAtBounds

        contentWidth: width
        contentHeight: contentColumn.implicitHeight + 40

        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

        Column {
            id: contentColumn

            width: pageFlickable.width - 40
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20

            spacing: 28

            // -----------------------------------------------------
            // Заголовок
            // -----------------------------------------------------

            Label {
                width: parent.width

                text: "Медиатека"
                color: AppTheme.textPrimary
                font.pixelSize: 28
                font.bold: true
            }

            // -----------------------------------------------------
            // Мои плейлисты
            // -----------------------------------------------------

            Column {
                width: parent.width
                spacing: 12

                Row {
                    width: parent.width
                    spacing: 12

                    Label {
                        width: parent.width - createButton.width - parent.spacing

                        text: "Мои плейлисты"
                        color: AppTheme.textPrimary
                        font.pixelSize: 20
                        font.bold: true

                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        id: createButton

                        width: createLabel.implicitWidth + 20
                        height: 30

                        radius: 7
                        color: createMouse.containsMouse ? AppTheme.panelHover : AppTheme.panel

                        border.width: 1
                        border.color: AppTheme.borderSubtle

                        Label {
                            id: createLabel

                            anchors.centerIn: parent

                            text: "+"
                            color: AppTheme.textPrimary
                            font.pixelSize: 16
                            font.bold: true
                        }

                        MouseArea {
                            id: createMouse

                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (root.controller) {
                                    root.controller.createPlaylist("Новый плейлист")
                                }
                            }
                        }
                    }
                }

                Label {
                    width: parent.width

                    text: "Загрузка плейлистов..."
                    color: AppTheme.textSecondary
                    font.pixelSize: 13

                    visible: root.controller !== null && root.controller !== undefined &&
                        root.controller.loadingLibraryPlaylists
                }

                ListView {
                    id: playlistsView

                    width: parent.width
                    height: 220

                    orientation: ListView.Horizontal
                    spacing: 14

                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    model: root.controller !== null && root.controller !== undefined
                        ? root.controller.libraryPlaylistsModel
                        : null

                    ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }

                    delegate: Rectangle {
                        id: playlistCard

                        required property int index
                        required property string uid
                        required property int kind
                        required property string title
                        required property string coverUri
                        required property int trackCount

                        width: 170
                        height: 200

                        radius: 12
                        color: playlistMouseArea.containsMouse
                            ? AppTheme.panelActive
                            : AppTheme.panelSecondary

                        border.width: 1
                        border.color: AppTheme.borderSubtle
                        clip: true

                        // Обложка
                        Rectangle {
                            id: playlistArtworkContainer

                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right

                            height: 140
                            color: AppTheme.artworkPlaceholder

                            Image {
                                id: playlistArtwork

                                anchors.fill: parent

                                source: playlistCard.coverUri.length > 0
                                    ? "image://yandex/" + playlistCard.coverUri
                                    : ""

                                sourceSize: Qt.size(280, 280)

                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                                cache: true
                                smooth: true

                                visible: status === Image.Ready
                            }

                            Label {
                                anchors.centerIn: parent

                                text: "♪"
                                color: AppTheme.textSecondary
                                font.pixelSize: 32

                                visible: playlistArtwork.status !== Image.Ready
                            }
                        }

                        // Название
                        Label {
                            id: playlistTitle

                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: playlistArtworkContainer.bottom
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            anchors.topMargin: 8

                            text: playlistCard.title.length > 0 ? playlistCard.title : qsTr("Плейлист")

                            color: AppTheme.textPrimary
                            font.pixelSize: 13
                            font.bold: true
                            elide: Text.ElideRight
                        }

                        // Кол-во треков
                        Label {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: playlistTitle.bottom
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            anchors.topMargin: 3

                            text: playlistCard.trackCount > 0
                                ? qsTr("%1 треков").arg(playlistCard.trackCount)
                                : ""

                            color: AppTheme.textSecondary
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }

                        MouseArea {
                            id: playlistMouseArea

                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor

                            onClicked: {
                                if (root.controller === null || root.controller === undefined) {
                                    return
                                }

                                root.controller.selectLibraryPlaylist(playlistCard.index)
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "Плейлистов нет"
                        color: AppTheme.textSecondary
                        font.pixelSize: 13

                        visible: root.controller !== null && root.controller !== undefined &&
                            !root.controller.loadingLibraryPlaylists &&
                            root.controller.libraryPlaylistsModel !== null &&
                            root.controller.libraryPlaylistsModel.count === 0
                    }
                }
            }

            // -----------------------------------------------------
            // Мне нравится
            // -----------------------------------------------------

            Column {
                id: likesSection

                width: parent.width
                spacing: 12

                Label {
                    width: parent.width

                    text: "Мне нравится"
                    color: AppTheme.textPrimary
                    font.pixelSize: 20
                    font.bold: true
                }

                Label {
                    width: parent.width

                    text: "Загрузка понравившихся треков..."
                    color: AppTheme.textSecondary
                    font.pixelSize: 13

                    visible: root.controller !== null && root.controller !== undefined &&
                        root.controller.loadingLikedTracks
                }

                Rectangle {
                    id: likedTracksContainer

                    width: parent.width

                    height: likedTracksView.count > 0
                        ? Math.min(likedTracksView.count * 74 + 20, 10 * 74 + 20)
                        : 120

                    radius: 12
                    color: AppTheme.panel
                    border.width: 1
                    border.color: AppTheme.borderSubtle

                    ListView {
                        id: likedTracksView

                        anchors.fill: parent
                        anchors.margins: 10

                        spacing: 6
                        clip: true

                        interactive: likedTracksView.count > 10
                        boundsBehavior: Flickable.StopAtBounds

                        model: root.controller !== null && root.controller !== undefined
                            ? root.controller.likedTracksModel
                            : null

                        delegate: TrackRow {
                            width: likedTracksView.width

                            controller: root.controller
                            rowIndex: model.index
                            trackId: model.trackId
                            title: model.title
                            artist: model.artist
                            artistId: model.artistId
                            album: model.album
                            albumId: model.albumId
                            coverUri: model.coverUri
                            durationMs: model.durationMs
                            liked: model.liked
                            showLike: true

                            onActivated: {
                                if (root.controller === null || root.controller === undefined) {
                                    return
                                }

                                root.controller.selectLikedTrack(model.index)
                            }
                        }

                        Label {
                            anchors.centerIn: parent

                            text: root.controller !== null && root.controller !== undefined &&
                                root.controller.loadingLikedTracks
                                ? qsTr("Загрузка...")
                                : qsTr("Нет понравившихся треков")

                            color: AppTheme.textSecondary
                            font.pixelSize: 13

                            visible: root.controller !== null && root.controller !== undefined &&
                                !root.controller.loadingLikedTracks &&
                                (root.controller.likedTracksModel === null ||
                                 root.controller.likedTracksModel.count === 0)
                        }
                    }
                }
            }

            // -----------------------------------------------------
            // Сохранённые альбомы
            // -----------------------------------------------------

            Column {
                width: parent.width
                spacing: 12

                Label {
                    width: parent.width

                    text: "Сохранённые альбомы"
                    color: AppTheme.textPrimary
                    font.pixelSize: 20
                    font.bold: true
                }

                Rectangle {
                    width: parent.width

                    height: albumGridView.count > 0
                        ? Math.min(albumGridView.count * 74 + 20, 5 * 74 + 20)
                        : 120

                    radius: 12
                    color: AppTheme.panel
                    border.width: 1
                    border.color: AppTheme.borderSubtle

                    visible: root.controller !== null && root.controller !== undefined

                    GridView {
                        id: albumGridView

                        anchors.fill: parent
                        anchors.margins: 10

                        cellWidth: 180
                        cellHeight: 64

                        clip: true
                        interactive: albumGridView.count > 5
                        boundsBehavior: Flickable.StopAtBounds

                        model: root.controller !== null && root.controller !== undefined
                            ? root.controller.likedAlbumsModel
                            : null

                        delegate: Rectangle {
                            id: albumDelegate

                            required property string albumId
                            required property string title
                            required property string coverUri
                            required property int year

                            width: 180
                            height: 64

                            radius: 8
                            color: albumMouse.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary

                            Rectangle {
                                id: albumCover

                                width: 48
                                height: 48

                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                radius: 6
                                color: AppTheme.artworkPlaceholder
                                clip: true

                                Image {
                                    id: albumCoverImage

                                    anchors.fill: parent

                                    source: albumDelegate.coverUri.length > 0
                                        ? "image://yandex/" + albumDelegate.coverUri
                                        : ""

                                    sourceSize: Qt.size(96, 96)

                                    fillMode: Image.PreserveAspectCrop
                                    asynchronous: true
                                    cache: true
                                    smooth: true

                                    visible: status === Image.Ready
                                }

                                Label {
                                    anchors.centerIn: parent

                                    text: "♪"
                                    color: AppTheme.textSecondary
                                    font.pixelSize: 20

                                    visible: albumCoverImage.status !== Image.Ready
                                }
                            }

                            Column {
                                anchors.left: albumCover.right
                                anchors.leftMargin: 10
                                anchors.verticalCenter: parent.verticalCenter

                                spacing: 2

                                Label {
                                    width: 120

                                    text: albumDelegate.title
                                    color: AppTheme.textPrimary
                                    font.pixelSize: 13
                                    font.bold: true
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                }

                                Label {
                                    text: albumDelegate.year > 0 ? albumDelegate.year : ""
                                    color: AppTheme.textSecondary
                                    font.pixelSize: 11
                                }
                            }

                            MouseArea {
                                id: albumMouse

                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor

                                onClicked: {
                                    if (root.controller === null || root.controller === undefined) {
                                        return
                                    }

                                    root.controller.loadAlbum(albumDelegate.albumId)
                                }
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "Нет сохранённых альбомов"
                        color: AppTheme.textSecondary
                        font.pixelSize: 13

                        visible: root.controller !== null && root.controller !== undefined &&
                            (root.controller.likedAlbumsModel === null ||
                             root.controller.likedAlbumsModel.count === 0)
                    }
                }
            }

            // -----------------------------------------------------
            // Любимые исполнители
            // -----------------------------------------------------

            Column {
                width: parent.width
                spacing: 12

                Label {
                    width: parent.width

                    text: "Любимые исполнители"
                    color: AppTheme.textPrimary
                    font.pixelSize: 20
                    font.bold: true
                }

                Rectangle {
                    width: parent.width

                    height: artistGridView.count > 0
                        ? Math.min(artistGridView.count * 74 + 20, 5 * 74 + 20)
                        : 120

                    radius: 12
                    color: AppTheme.panel
                    border.width: 1
                    border.color: AppTheme.borderSubtle

                    visible: root.controller !== null && root.controller !== undefined

                    GridView {
                        id: artistGridView

                        anchors.fill: parent
                        anchors.margins: 10

                        cellWidth: 180
                        cellHeight: 64

                        clip: true
                        interactive: artistGridView.count > 5
                        boundsBehavior: Flickable.StopAtBounds

                        model: root.controller !== null && root.controller !== undefined
                            ? root.controller.likedArtistsModel
                            : null

                        delegate: Rectangle {
                            id: artistDelegate

                            required property string artistId
                            required property string name
                            required property string coverUri

                            width: 180
                            height: 64

                            radius: 8
                            color: artistMouse.containsMouse
                                ? AppTheme.panelActive
                                : AppTheme.panelSecondary

                            Rectangle {
                                id: artistCover

                                width: 48
                                height: 48

                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter

                                radius: 24
                                color: AppTheme.artworkPlaceholder
                                clip: true

                                Image {
                                    id: artistCoverImage

                                    anchors.fill: parent

                                    source: artistDelegate.coverUri.length > 0
                                        ? "image://yandex/" + artistDelegate.coverUri
                                        : ""

                                    sourceSize: Qt.size(96, 96)

                                    fillMode: Image.PreserveAspectCrop
                                    asynchronous: true
                                    cache: true
                                    smooth: true

                                    visible: status === Image.Ready
                                }
                            }

                            Label {
                                anchors.left: artistCover.right
                                anchors.leftMargin: 10
                                anchors.verticalCenter: parent.verticalCenter

                                width: 120

                                text: artistDelegate.name
                                color: AppTheme.textPrimary
                                font.pixelSize: 13
                                font.bold: true
                                elide: Text.ElideRight
                                maximumLineCount: 1
                            }

                            MouseArea {
                                id: artistMouse

                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor

                                onClicked: {
                                    if (root.controller === null || root.controller === undefined) {
                                        return
                                    }

                                    root.controller.loadArtist(artistDelegate.artistId)
                                }
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent

                        text: "Нет любимых исполнителей"
                        color: AppTheme.textSecondary
                        font.pixelSize: 13

                        visible: root.controller !== null && root.controller !== undefined &&
                            (root.controller.likedArtistsModel === null ||
                             root.controller.likedArtistsModel.count === 0)
                    }
                }
            }

            // Нижний отступ
            Item {
                width: 1
                height: 20
            }
        }
    }

    // -------------------------------------------------------------
    // Первичная загрузка
    // -------------------------------------------------------------

    Component.onCompleted: {
        if (root.controller === null || root.controller === undefined) {
            return
        }

        root.controller.loadLibrary()
        root.controller.loadLikedTracks()
    }
}