import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    width: parent ? parent.width : 0
    implicitHeight: pageColumn.implicitHeight + 56

    readonly property var albumController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.albumController !== null && root.controller.albumController !== undefined
            ? root.controller.albumController
            : null

    readonly property var albumModel:
        root.albumController !== null &&
        root.albumController.albumModel !== null && root.albumController.albumModel !== undefined
            ? root.albumController.albumModel
            : null

    readonly property bool loading:
        root.albumController !== null && root.albumController.loading === true

    readonly property bool hasAlbum:
        root.albumController !== null && root.albumController.albumId !== ""

    readonly property int trackCount:
        root.albumModel !== null ? root.albumModel.count : 0

    readonly property string albumTitle:
        root.albumController !== null
            ? String(root.albumController.currentAlbumTitle || "")
            : ""

    readonly property string albumArtworkUri:
        root.albumController !== null
            ? String(root.albumController.currentAlbumCoverUri || "")
            : ""

    PlaylistPicker {
        id: addToPlaylistPopup
        controller: root.controller
    }

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    // Контент скроллит внешний ScrollView в MainLayout
    Column {
        id: pageColumn

        width: root.width
        spacing: 20

        Label {
            width: parent.width
            height: 42

            text: root.albumTitle.length > 0 ? root.albumTitle : qsTr("Альбом")

            color: AppTheme.textPrimary
            font.pixelSize: 32
            font.weight: Font.DemiBold

            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            maximumLineCount: 1
        }

        Rectangle {
            width: parent.width
            height: 216

            radius: 14
            color: AppTheme.panel
            border.width: 1
            border.color: AppTheme.borderSubtle

            Row {
                anchors.fill: parent
                anchors.margins: 18

                spacing: 20

                Rectangle {
                    width: 180
                    height: 180

                    anchors.verticalCenter: parent.verticalCenter

                    radius: 10
                    color: AppTheme.artworkPlaceholder
                    clip: true

                    Image {
                        id: albumImage

                        anchors.fill: parent

                        source: root.albumArtworkUri.length > 0
                            ? "image://yandex/" + root.albumArtworkUri
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
                        font.pixelSize: 40

                        visible: albumImage.status !== Image.Ready
                    }

                    Rectangle {
                        anchors.fill: parent
                        radius: 10

                        color: "transparent"
                        border.width: 1
                        border.color: AppTheme.border
                    }
                }

                Column {
                    width: parent.width - 200

                    anchors.verticalCenter: parent.verticalCenter

                    spacing: 8

                    Label {
                        text: qsTr("АЛЬБОМ")
                        color: AppTheme.textMuted
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Row {
                        width: parent.width
                        spacing: 10

                        Label {
                            width: parent.width - albumLikeButton.width - parent.spacing

                            text: root.albumTitle.length > 0 ? root.albumTitle : qsTr("Альбом")

                            color: AppTheme.textPrimary
                            font.pixelSize: 30
                            font.weight: Font.DemiBold

                            elide: Text.ElideRight
                            maximumLineCount: 2
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

                                text: root.controller && root.controller.currentAlbumLiked ? "♥" : "♡"
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
                        text: root.trackCount > 0
                            ? root.trackCount +
                              (root.trackCount === 1 ? qsTr(" трек") : qsTr(" трека"))
                            : qsTr("Нет треков")

                        color: AppTheme.textSecondary
                        font.pixelSize: 13
                    }
                }
            }
        }

        Rectangle {
            width: parent.width

            height: root.trackCount > 0
                ? 14 + 26 + 10 + Math.min(root.trackCount, 6) * 64 +
                  Math.max(Math.min(root.trackCount, 6) - 1, 0) * 4 + 14
                : 14 + 26 + 10 + 72 + 14

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
                    height: 26

                    text: qsTr("Треки")

                    color: AppTheme.textPrimary
                    font.pixelSize: 22
                    font.bold: true

                    verticalAlignment: Text.AlignVCenter
                }

                ListView {
                    id: tracksView

                    width: parent.width

                    height: root.trackCount > 0
                        ? Math.min(root.trackCount, 6) * 64 +
                          Math.max(Math.min(root.trackCount, 6) - 1, 0) * 4
                        : 72

                    model: root.albumModel

                    clip: true
                    spacing: 4

                    interactive: root.trackCount > 6
                    boundsBehavior: Flickable.StopAtBounds

                    ScrollBar.vertical: ScrollBar {
                        policy: tracksView.count > 6 ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                    }

                    delegate: TrackRow {
                        width: tracksView.width - (tracksView.count > 6 ? 8 : 0)

                        controller: root.controller
                        rowIndex: model.index
                        trackId: model.trackId
                        title: model.title
                        artist: model.artist
                        artistId: model.artistId
                        albumId: model.albumId
                        coverUri: model.coverUri
                        durationMs: model.durationMs
                        liked: model.liked
                        showLike: true
                        showAdd: true

                        rowHeight: 64
                        coverSize: 48
                        cornerRadius: 8

                        onActivated: {
                            if (root.albumController !== null) {
                                root.albumController.selectAlbumTrack(model.index)
                            }
                        }

                        onAddRequested: function(trackId, albumId) {
                            addToPlaylistPopup.trackId = trackId
                            addToPlaylistPopup.albumId = albumId
                            addToPlaylistPopup.open()
                        }
                    }

                    // Пустое состояние
                    Rectangle {

                        width: parent.width
                        height: 72

                        radius: 8
                        color: AppTheme.panelSecondary
                        border.width: 1
                        border.color: AppTheme.borderSubtle

                        visible: root.trackCount === 0 && !root.loading

                        Label {
                            anchors.centerIn: parent

                            text: qsTr("Треки не найдены")
                            color: AppTheme.textMuted
                            font.pixelSize: 14
                        }
                    }
                }
            }
        }

        // Нижний отступ
        Item {
            width: 1
            height: 100
        }
    }

    BusyIndicator {
        id: loadingIndicator

        anchors.centerIn: parent

        width: 32
        height: 32

        running: root.loading
        visible: running
        z: 100
    }
}