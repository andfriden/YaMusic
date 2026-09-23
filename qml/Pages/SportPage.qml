import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller: null

    readonly property var genreController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.genreController !== null && root.controller.genreController !== undefined
            ? root.controller.genreController
            : null

    readonly property int margin: 20
    readonly property int spacing: 12
    readonly property int columns: 6

    // Ширина из реальной ширины сетки, чтобы карточки заполняли ряд
    readonly property real cardWidth:
        Math.floor((contentColumn.width - spacing * (columns - 1)) / columns)

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

        width: root.width - root.margin * 2
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: root.margin

        spacing: 28

        Column {
            width: parent.width
            spacing: 6

            Label {
                width: parent.width

                text: "Спорт"
                color: AppTheme.textPrimary
                font.pixelSize: 32
                font.bold: true
                elide: Text.ElideRight
            }

            Label {
                width: parent.width

                text: root.genreController && root.genreController.genreLoading
                    ? "Загрузка плейлистов..."
                    : root.genreController && root.genreController.genrePlaylists
                        ? root.genreController.genrePlaylists.length + " плейлистов"
                        : ""

                color: AppTheme.textSecondary
                font.pixelSize: 15
            }
        }

        Label {
            width: parent.width

            text: "Плейлисты для спорта"
            color: AppTheme.textPrimary
            font.pixelSize: 24
            font.bold: true
        }

        Grid {
            id: playlistGrid

            width: parent.width
            columns: root.columns

            columnSpacing: root.spacing
            rowSpacing: 16

            Repeater {
                model: root.genreController !== null
                    ? root.genreController.genrePlaylists
                    : []

                delegate: PlaylistCard {
                    required property var modelData

                    controller: root.controller
                    cardWidth: root.cardWidth
                    coverUri: String(modelData.coverUri || "")
                    title: String(modelData.title || "")
                    trackCount: Number(modelData.trackCount || 0)

                    onActivated: {
                        if (root.controller === null || root.controller === undefined) {
                            return
                        }

                        const uid = String(modelData.uid || "").trim()
                        const kind = Number(modelData.kind || 0)

                        if (uid.length === 0 || kind <= 0) {
                            return
                        }

                        root.controller.selectPersonalPlaylist(uid, kind)
                    }
                }
            }
        }

        Label {
            visible: root.genreController !== null &&
                !root.genreController.genreLoading &&
                (!root.genreController.genrePlaylists ||
                 root.genreController.genrePlaylists.length === 0)

            width: parent.width

            text: "Плейлисты не найдены"
            color: AppTheme.textSecondary
            font.pixelSize: 16

            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            visible: root.genreController !== null && root.genreController.genreLoading

            width: parent.width

            text: "Загрузка плейлистов..."
            color: AppTheme.textSecondary
            font.pixelSize: 16

            horizontalAlignment: Text.AlignHCenter
        }
    }

    Component.onCompleted: {
        if (root.genreController !== null) {
            root.genreController.loadTagPlaylists("sport")
        }
    }
}