import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

// Контекстное меню трека: те же действия, что доступны в списках и плеере,
// плюс запуск «Моей волны» по этому треку.
Menu {
    id: root

    property var controller
    property string trackId: ""
    // Не title: у Menu это FINAL-свойство заголовка меню.
    property string trackTitle: ""
    property string artist: ""
    property string artistId: ""
    property string album: ""
    property string albumId: ""
    property string coverUri: ""
    property int durationMs: 0
    property bool liked: false
    property bool disliked: false
    property bool showAdd: false

    signal addToPlaylistRequested()

    readonly property bool hasController:
        root.controller !== null && root.controller !== undefined

    readonly property bool hasTrack: root.trackId.length > 0

    padding: 6
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle {
        implicitWidth: 250
        implicitHeight: 6

        radius: 10
        color: AppTheme.panel
        border.width: 1
        border.color: AppTheme.border
    }

    component TrackMenuItem: MenuItem {
        id: item

        contentItem: Text {
            leftPadding: 10
            rightPadding: 10

            text: item.text
            font.pixelSize: 13
            color: item.enabled
                ? (item.highlighted ? AppTheme.accent : AppTheme.textPrimary)
                : AppTheme.textMuted
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            implicitHeight: 32
            radius: 6
            color: item.highlighted ? AppTheme.panelHover : "transparent"
        }
    }

    component TrackMenuSeparator: MenuSeparator {
        contentItem: Rectangle {
            implicitHeight: 1
            color: AppTheme.borderSubtle
        }
    }

    TrackMenuItem {
        text: qsTr("Моя волна по треку")

        enabled: root.hasTrack && root.hasController &&
            root.controller.stationsController !== undefined

        onTriggered: {
            if (root.hasController && root.controller.stationsController) {
                root.controller.stationsController.startTrackWave(root.trackId, root.trackTitle)
            }
        }
    }

    TrackMenuItem {
        text: root.liked
            ? qsTr("Убрать из «Мне нравится»")
            : qsTr("Нравится")

        enabled: root.hasTrack && root.hasController

        onTriggered: {
            if (root.hasController) {
                root.controller.toggleLike(root.trackId, root.liked)
            }
        }
    }

    TrackMenuItem {
        text: root.disliked
            ? qsTr("Убрать дизлайк")
            : qsTr("Не нравится")

        enabled: root.hasTrack && root.hasController

        onTriggered: {
            if (root.hasController) {
                root.controller.toggleDislike(root.trackId, root.disliked)
            }
        }
    }

    TrackMenuSeparator {}

    TrackMenuItem {
        text: qsTr("Добавить в плейлист")

        visible: root.showAdd
        enabled: root.hasTrack && root.hasController

        onTriggered: root.addToPlaylistRequested()
    }

    TrackMenuItem {
        text: qsTr("Скачать")

        enabled: root.hasTrack && root.hasController

        onTriggered: {
            if (!root.hasController) {
                return
            }

            root.controller.downloadTrack(
                root.trackId,
                root.trackTitle,
                root.artist,
                root.album,
                root.coverUri,
                root.durationMs
            )
        }
    }

    TrackMenuSeparator {}

    TrackMenuItem {
        text: qsTr("Перейти к исполнителю")

        enabled: root.artistId.length > 0 && root.hasController

        onTriggered: {
            if (root.hasController) {
                root.controller.loadArtist(root.artistId)
            }
        }
    }

    TrackMenuItem {
        text: qsTr("Перейти к альбому")

        enabled: root.albumId.length > 0 && root.hasController

        onTriggered: {
            if (root.hasController) {
                root.controller.loadAlbum(root.albumId)
            }
        }
    }

    TrackMenuItem {
        text: qsTr("Скопировать «Исполнитель — Название»")

        enabled: root.hasController

        onTriggered: {
            if (root.hasController) {
                root.controller.copyTrack(root.trackTitle, root.artist)
            }
        }
    }
}
