import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    PlaylistPicker {
        id: addToPlaylistPopup
        controller: root.controller
    }

    Rectangle {
        anchors.fill: parent

        radius: 10
        color: AppTheme.panel
        border.width: 1
        border.color: AppTheme.borderSubtle

        Column {
            anchors.fill: parent
            anchors.margins: 12

            spacing: 12

            Row {
                width: parent.width
                spacing: 10

                Label {
                    text: root.controller && root.controller.currentPlaylistTitle
                        ? root.controller.currentPlaylistTitle
                        : qsTr("Плейлист")

                    color: AppTheme.textPrimary
                    font.pixelSize: 18
                    font.bold: true

                    anchors.verticalCenter: parent.verticalCenter
                }

                Label {
                    text: root.controller && root.controller.currentPlaylistTrackCount > 0
                        ? root.controller.currentPlaylistTrackCount + qsTr(" треков")
                        : ""

                    color: AppTheme.textSecondary

                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            ListView {
                id: tracksView

                width: parent.width
                height: parent.height - 45

                clip: true
                spacing: 6

                model: root.controller && root.controller.playlistModel
                    ? root.controller.playlistModel
                    : null

                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                delegate: TrackRow {
                    width: tracksView.width -
                        (tracksView.ScrollBar.vertical.visible ? 10 : 0)

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
                    showAdd: true
                    showRemove: root.controller && root.controller.currentPlaylistKind > 0

                    onActivated: root.controller.selectPlaylistTrack(model.index)
                    onRemoveRequested: function(i) {
                        if (root.controller) {
                            root.controller.removeTrackFromPlaylist(i)
                        }
                    }
                    onAddRequested: function(trackId, albumId) {
                        addToPlaylistPopup.trackId = trackId
                        addToPlaylistPopup.albumId = albumId
                        addToPlaylistPopup.open()
                    }
                }

                Label {
                    anchors.centerIn: parent

                    text: root.controller && root.controller.loadingPlaylist
                        ? qsTr("Загрузка плейлиста...")
                        : qsTr("В плейлисте нет треков")

                    color: AppTheme.textSecondary
                    font.pixelSize: 13

                    visible: root.controller &&
                        (root.controller.loadingPlaylist ||
                         (root.controller.playlistModel !== null &&
                          root.controller.playlistModel.count === 0))
                }
            }
        }
    }
}