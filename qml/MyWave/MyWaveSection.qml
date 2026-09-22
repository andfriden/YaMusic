import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

/*
 * Секция "Моя волна".
 *
 * Используется и на главной (compactMode = true), и на отдельной
 * странице (compactMode = false). В компактном режиме меньше
 * отступы, высота строк и число видимых треков.
 */
Item {
    id: root

    property var controller: null
    property bool compactMode: false

    // =============================================================
    // Состояние
    // =============================================================

    readonly property bool hasController:
        root.controller !== null && root.controller !== undefined

    readonly property var waveModel:
        root.hasController ? root.controller.myWaveModel : null

    readonly property int trackCount:
        root.waveModel !== null && root.waveModel !== undefined
            ? Number(root.waveModel.count)
            : 0

    readonly property bool hasTracks: root.trackCount > 0

    // =============================================================
    // Геометрия
    // =============================================================

    readonly property int horizontalMargin: root.compactMode ? 12 : 20
    readonly property int verticalMargin: root.compactMode ? 12 : 20
    readonly property int contentSpacing: root.compactMode ? 10 : 14
    readonly property int headerHeight: root.compactMode ? 28 : 38
    readonly property int trackHeight: root.compactMode ? 58 : 68
    readonly property int trackSpacing: 6

    // Показываем не больше maxVisibleTracks треков; остальное —
    // прокрутка
    readonly property int maxVisibleTracks: root.compactMode ? 5 : 10

    readonly property int visibleTrackCount:
        Math.min(root.trackCount, root.maxVisibleTracks)

    readonly property int listHeight:
        root.visibleTrackCount > 0
            ? root.visibleTrackCount * root.trackHeight +
              Math.max(0, root.visibleTrackCount - 1) * root.trackSpacing
            : 0

    readonly property int contentHeight:
        root.hasTracks
            ? root.headerHeight + root.contentSpacing + root.listHeight
            : root.headerHeight

    implicitHeight:
        root.hasTracks ? root.verticalMargin * 2 + root.contentHeight : 0

    // =============================================================
    // Фон
    // =============================================================

    Rectangle {
        anchors.fill: parent

        visible: root.hasTracks

        radius: root.compactMode ? 10 : 14
        color: AppTheme.panel
        border.width: 1
        border.color: AppTheme.borderSubtle
    }

    // =============================================================
    // Контент
    // =============================================================

    Column {
        id: contentColumn

        x: root.horizontalMargin
        y: root.verticalMargin

        width: Math.max(0, root.width - root.horizontalMargin * 2)

        spacing: root.contentSpacing

        Label {
            width: parent.width
            height: root.headerHeight

            text: qsTr("Моя волна")

            color: AppTheme.textPrimary
            font.pixelSize: root.compactMode ? 18 : 26
            font.bold: true

            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            maximumLineCount: 1
        }

        ListView {
            id: trackList

            width: parent.width
            height: root.listHeight

            model: root.waveModel

            clip: true
            spacing: root.trackSpacing

            boundsBehavior: Flickable.StopAtBounds
            interactive: root.trackCount > root.maxVisibleTracks
            cacheBuffer: root.trackHeight * 3

            ScrollBar.vertical: ScrollBar {
                policy: root.trackCount > root.maxVisibleTracks
                    ? ScrollBar.AsNeeded
                    : ScrollBar.AlwaysOff
            }

            delegate: TrackRow {
                width: trackList.width -
                    (trackList.ScrollBar.vertical.visible ? 10 : 0)

                controller: root.controller
                rowIndex: model.index
                title: model.title
                artist: model.artist
                artistId: model.artistId
                album: model.album
                albumId: model.albumId
                coverUri: model.coverUri
                durationMs: model.durationMs

                // В модели "Моей волны" нет trackId — подсвечиваем
                // по паре title+artist
                currentMatch: "titleArtist"

                rowHeight: root.trackHeight
                showAdd: false
                showLike: false

                // Альбом в компактном режиме скрываем — не хватает
                // ширины
                showAlbum: !root.compactMode && model.album.length > 0

                onActivated: {
                    if (root.hasController) {
                        root.controller.selectMyWaveTrack(model.index)
                    }
                }
            }
        }
    }
}