import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

/*
 * Страница конкретного жанра: радиостанция, треки станции и
 * сетка плейлистов жанра.
 */
Item {
    id: root

    property var controller: null
    property string genreId: ""
    property string genreTitle: ""
    property string genreImage: ""
    property string genreColor: ""

    // -------------------------------------------------------------
    // Состояние
    // -------------------------------------------------------------

    readonly property var genreController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.genreController !== null && root.controller.genreController !== undefined
            ? root.controller.genreController
            : null

    // Кнопка радио поверх цветной обложки жанра: на светлой теме
    // тёмный текст, на тёмной — акцент
    readonly property color radioButtonColor:
        AppTheme.dark
            ? AppTheme.accent
            : Qt.rgba(0, 0, 0, 0.84)

    readonly property color radioButtonHoverColor:
        AppTheme.dark
            ? AppTheme.accentHover
            : Qt.rgba(0.15, 0.15, 0.15, 1.0)

    // -------------------------------------------------------------
    // Геометрия
    // -------------------------------------------------------------

    readonly property int margin: 20
    readonly property int spacing: 12
    readonly property int columns: 6

    // Ширина карточки считается от реальной ширины сетки, чтобы
    // карточки заполняли ряд целиком
    readonly property real cardWidth:
        Math.floor((contentColumn.width - spacing * (columns - 1)) / columns)

    width: parent ? parent.width : 0
    implicitWidth: width
    implicitHeight: contentColumn.implicitHeight + 48
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

        // ---------------------------------------------------------
        // Шапка жанра
        // ---------------------------------------------------------

        Row {
            width: parent.width
            height: 180

            spacing: 24

            Rectangle {
                width: 180
                height: 180

                radius: 14
                color: root.genreColor.length > 0 ? root.genreColor : AppTheme.panel
                clip: true

                Image {
                    anchors.fill: parent

                    source: root.genreImage.length > 0
                        ? "image://yandex/" + root.genreImage
                        : ""

                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    cache: true
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter

                width: parent.width - 204
                spacing: 8

                Label {
                    width: parent.width

                    text: root.genreTitle
                    color: AppTheme.textPrimary
                    font.pixelSize: 32
                    font.bold: true
                    elide: Text.ElideRight
                }

                Label {
                    text: root.genreController && root.genreController.genreLoading
                        ? "Загрузка плейлистов..."
                        : root.genreController && root.genreController.genrePlaylists &&
                          root.genreController.genrePlaylists.length > 0
                            ? root.genreController.genrePlaylists.length + " плейлистов"
                            : ""

                    color: AppTheme.textSecondary
                    font.pixelSize: 15
                }

                // Кнопка «Радио»
                Rectangle {
                    id: radioButton

                    visible: root.genreId.length > 0

                    width: radioLabel.implicitWidth + 32
                    height: 36

                    radius: 8
                    color: radioMouse.containsMouse
                        ? root.radioButtonHoverColor
                        : root.radioButtonColor

                    Behavior on color {
                        ColorAnimation { duration: 140; easing.type: Easing.OutCubic }
                    }

                    enabled: !root.genreController || !root.genreController.stationLoading
                    opacity: enabled ? 1.0 : 0.5

                    Label {
                        id: radioLabel

                        anchors.centerIn: parent

                        text: root.genreController && root.genreController.stationLoading
                            ? "Загрузка..."
                            : "▶  Радио"

                        color: AppTheme.accentOn
                        font.pixelSize: 13
                        font.bold: true
                    }

                    MouseArea {
                        id: radioMouse

                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        enabled: parent.enabled

                        onClicked: {
                            if (root.genreController) {
                                root.genreController.loadGenreStation(root.genreId)
                            }
                        }
                    }
                }
            }
        }

        // ---------------------------------------------------------
        // Треки радиостанции
        // ---------------------------------------------------------

        Rectangle {
            width: parent.width

            height: root.genreController !== null &&
                root.genreController.stationModel !== null &&
                root.genreController.stationModel.count > 0
                ? 60 + Math.min(root.genreController.stationModel.count, 6) * 68
                : 0

            radius: 12
            color: AppTheme.panel
            border.width: 1
            border.color: AppTheme.borderSubtle

            visible: height > 0

            Column {
                anchors.fill: parent
                anchors.margins: 14

                spacing: 10

                Label {
                    width: parent.width

                    text: "Радиостанция"
                    color: AppTheme.textPrimary
                    font.pixelSize: 22
                    font.bold: true
                }

                ListView {
                    id: stationView

                    width: parent.width
                    height: Math.min(count, 6) * 68

                    model: root.genreController !== null
                        ? root.genreController.stationModel
                        : null

                    clip: true
                    spacing: 4

                    interactive: count > 6

                    ScrollBar.vertical: ScrollBar {
                        policy: stationView.count > 6 ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                    }

                    delegate: TrackRow {
                        width: stationView.width
                        height: 64

                        controller: root.controller
                        rowIndex: model.index
                        trackId: model.trackId
                        title: model.title
                        artist: model.artist
                        artistId: model.artistId
                        coverUri: model.coverUri
                        durationMs: model.durationMs
                        showAlbum: false

                        onActivated: {
                            if (root.controller) {
                                root.controller.selectGenreStationTrack(model.index)
                            }
                        }
                    }
                }
            }
        }

        // ---------------------------------------------------------
        // Плейлисты жанра
        // ---------------------------------------------------------

        Label {
            width: parent.width

            text: "Плейлисты"
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
                id: playlistRepeater

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

        // Пустое состояние
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

        // Загрузка
        Label {
            visible: root.genreController !== null && root.genreController.genreLoading

            width: parent.width

            text: "Загрузка плейлистов..."
            color: AppTheme.textSecondary
            font.pixelSize: 16

            horizontalAlignment: Text.AlignHCenter
        }
    }

    // -------------------------------------------------------------
    // Загрузка жанра при смене genreId
    // -------------------------------------------------------------

    onGenreIdChanged: {
        if (root.genreController !== null && root.genreId.length > 0) {
            root.genreController.loadGenre(root.genreId)
        }
    }
}