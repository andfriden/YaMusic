import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

/*
 * Страница «Плейлисты»: подборки плейлистов из personalPlaylistsModel.
 * Карточки используют общий компонент PlaylistCard.
 */
Item {
    id: root

    property var controller: null

    width: parent ? parent.width : 0
    implicitWidth: width
    implicitHeight: contentColumn.implicitHeight + 40
    height: implicitHeight

    Rectangle {
        anchors.fill: parent
        color: AppTheme.backgroundPrimary
    }

    Column {
        id: contentColumn

        width: parent.width
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 20

        spacing: 28

        // Заголовок
        Label {
            width: parent.width

            text: "Плейлисты"
            color: AppTheme.textPrimary
            font.pixelSize: 28
            font.bold: true
            elide: Text.ElideRight
        }

        // Секции
        Repeater {
            model: root.controller !== null && root.controller !== undefined
                ? root.controller.personalPlaylistsModel
                : null

            delegate: Column {
                id: sectionItem

                required property string title
                required property string type
                required property var playlists

                width: contentColumn.width
                spacing: 12

                visible: sectionItem.playlists !== null &&
                    sectionItem.playlists !== undefined &&
                    sectionItem.playlists.length > 0

                // Заголовок секции
                Label {
                    width: parent.width
                    height: 24

                    text: sectionItem.title

                    color: AppTheme.textPrimary
                    font.pixelSize: 20
                    font.bold: true

                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                // Сетка карточек
                Grid {
                    width: parent.width

                    columns: Math.max(1, Math.floor(width / 180))
                    rowSpacing: 16
                    columnSpacing: 12

                    Repeater {
                        model: sectionItem.playlists

                        delegate: PlaylistCard {
                            required property var modelData

                            controller: root.controller
                            coverUri: String(modelData.coverUri || "")
                            title: String(modelData.title || "")
                            trackCount: Number(modelData.trackCount || 0)

                            onActivated: {
                                if (root.controller === null || root.controller === undefined) {
                                    return
                                }

                                const uid = String(modelData.uid || "")
                                const kind = Number(modelData.kind || 0)

                                if (uid.length === 0 || kind <= 0) {
                                    return
                                }

                                root.controller.selectPersonalPlaylist(uid, kind)
                            }
                        }
                    }
                }
            }
        }
    }
}