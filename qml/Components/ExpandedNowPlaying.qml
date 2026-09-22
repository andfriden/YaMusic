import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

/*
 * Развёрнутый плеер (оверлей): крупная обложка, управление,
 * очередь «Далее» и вкладка «Похожие».
 *
 * Очередь и похожие треки рендерятся одним компонентом
 * (TrackListItem): отличаются только модель и обработчик клика,
 * поэтому вёрстка не дублируется.
 */
Item {
    id: root

    property var controller

    signal closed()

    anchors.fill: parent

    // =============================================================
    // Состояние
    // =============================================================

    readonly property bool hasController:
        root.controller !== null && root.controller !== undefined

    readonly property bool hasTrack:
        root.hasController &&
        String(root.controller.currentTrackTitle || "").length > 0

    readonly property bool playing:
        root.hasController && root.controller.playing

    readonly property bool hasPlayerAccent:
        root.hasController &&
        root.controller.accentController.playerAccent !== undefined &&
        root.controller.accentController.playerAccent !== null &&
        root.controller.accentController.playerAccent.valid

    readonly property color playerAccent:
        root.hasPlayerAccent
            ? root.controller.accentController.playerAccent
            : AppTheme.accent

    readonly property string playbackSourceTitle:
        root.hasController
            ? String(root.controller.playbackSourceTitle || "")
            : ""

    readonly property string playbackSourceType:
        root.hasController
            ? String(root.controller.playbackSourceType || "")
            : ""

    property bool showingSimilar: false

    // =============================================================
    // Фон
    // =============================================================

    Rectangle {
        anchors.fill: parent
        color: AppTheme.background
    }

    // Лёгкая подсветка акцентным цветом обложки
    Rectangle {
        anchors.fill: parent

        color: root.hasPlayerAccent
            ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.24)
            : "transparent"

        Behavior on color {
            ColorAnimation { duration: 450; easing.type: Easing.OutCubic }
        }
    }

    // Градиент сверху
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        height: parent.height * 0.45

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: root.hasPlayerAccent
                    ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.22)
                    : "transparent"
            }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }

    // =============================================================
    // Шапка
    // =============================================================

    Item {
        id: header

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 36
        anchors.rightMargin: 36
        anchors.topMargin: 22

        height: 40

        // Кнопка закрытия
        Item {
            id: closeButton

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            width: 42
            height: 42

            Rectangle {
                anchors.fill: parent
                radius: width / 2

                color: closeMouseArea.containsMouse
                    ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.16)
                    : Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.12)

                Behavior on color {
                    ColorAnimation { duration: 140; easing.type: Easing.OutCubic }
                }
            }

            Text {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -3

                text: "⌄"
                color: Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.85)
                font.pixelSize: 24
            }

            MouseArea {
                id: closeMouseArea

                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: root.closed()
            }
        }
    }

    // =============================================================
    // Основной контент
    // =============================================================

    Row {
        id: mainRow

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 72
        anchors.rightMargin: 72
        anchors.topMargin: 20
        anchors.bottomMargin: 38

        spacing: 58

        // ---------------------------------------------------------
        // Плеер
        // ---------------------------------------------------------

        Item {
            id: playerPanel

            width: mainRow.width * 0.58
            height: mainRow.height

            // Обложка
            Image {
                id: artwork

                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter

                width: Math.min(440, parent.width * 0.62, parent.height * 0.58)
                height: width

                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: true

                source: {
                    if (!root.hasTrack) {
                        return ""
                    }

                    const uri = String(root.controller.currentTrackCoverUri || "")

                    return uri.length > 0 ? "image://yandex/" + uri : ""
                }

                Rectangle {
                    anchors.fill: parent

                    visible: artwork.status !== Image.Ready

                    radius: 12
                    color: AppTheme.artworkPlaceholder

                    Text {
                        anchors.centerIn: parent

                        text: "♪"
                        color: AppTheme.textMuted
                        font.pixelSize: 52
                    }
                }
            }

            // Информация о треке
            Column {
                id: trackInfo

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: artwork.bottom
                anchors.topMargin: 18

                spacing: 4

                Text {
                    width: parent.width

                    text: root.hasTrack
                        ? String(root.controller.currentTrackTitle || "")
                        : ""

                    color: AppTheme.textPrimary
                    font.pixelSize: 22
                    font.weight: Font.DemiBold

                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }

                EntityLink {
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: root.hasTrack
                        ? String(root.controller.currentTrackArtist || "")
                        : ""

                    entityId: root.hasTrack
                        ? String(root.controller.currentTrackArtistId || "")
                        : ""
                    entityType: "artist"
                    controller: root.controller
                }

                EntityLink {
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: root.hasTrack
                        ? String(root.controller.currentTrackAlbumTitle || "")
                        : ""

                    entityId: root.hasTrack
                        ? String(root.controller.currentTrackAlbumId || "")
                        : ""
                    entityType: "album"
                    controller: root.controller
                }
            }

            // Прогресс
            Item {
                id: progressArea

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: trackInfo.bottom
                anchors.topMargin: 14

                height: 30

                Rectangle {
                    id: progressBackground

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    height: 4
                    radius: 2

                    color: AppTheme.divider

                    Rectangle {
                        anchors.left: progressBackground.left
                        anchors.verticalCenter: progressBackground.verticalCenter

                        width: {
                            if (!root.hasController || Number(root.controller.duration) <= 0) {
                                return 0
                            }

                            const ratio = Math.max(0, Math.min(1,
                                Number(root.controller.position) / Number(root.controller.duration)))

                            return progressBackground.width * ratio
                        }

                        height: 4
                        radius: 2

                        color: root.playerAccent
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor

                    onClicked: function(mouse) {
                        if (!root.hasController) {
                            return
                        }

                        const duration = Number(root.controller.duration)

                        if (duration <= 0) {
                            return
                        }

                        const ratio = Math.max(0, Math.min(1, mouse.x / width))

                        root.controller.seek(Math.round(duration * ratio))
                    }
                }

                Text {
                    anchors.left: parent.left
                    anchors.top: progressBackground.bottom
                    anchors.topMargin: 5

                    text: AppTheme.formatTime(
                        root.hasController ? root.controller.position : 0)
                    color: AppTheme.textMuted
                    font.pixelSize: 11
                }

                Text {
                    anchors.right: parent.right
                    anchors.top: progressBackground.bottom
                    anchors.topMargin: 5

                    text: AppTheme.formatTime(
                        root.hasController ? root.controller.duration : 0)
                    color: AppTheme.textMuted
                    font.pixelSize: 11
                }
            }

            // Управление
            Row {
                id: controls

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: progressArea.bottom
                anchors.topMargin: 14

                spacing: 14

                ControlButton {
                    width: 40
                    height: 40

                    text: "↶"

                    onClicked: {
                        if (root.hasController) {
                            root.controller.cycleRepeat()
                        }
                    }
                }

                ControlButton {
                    width: 44
                    height: 44

                    text: "‹"
                    fontSize: 26

                    onClicked: {
                        if (root.hasController) {
                            root.controller.previous()
                        }
                    }
                }

                Rectangle {
                    width: 56
                    height: 56

                    radius: width / 2
                    color: root.playerAccent

                    Text {
                        anchors.centerIn: parent

                        text: root.playing ? "Ⅱ" : "▶"
                        color: AppTheme.onAccent
                        font.pixelSize: 21
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        enabled: root.hasTrack

                        onClicked: {
                            if (!root.hasController) {
                                return
                            }

                            if (root.playing) {
                                root.controller.pause()
                            } else {
                                root.controller.play()
                            }
                        }
                    }
                }

                ControlButton {
                    width: 44
                    height: 44

                    text: "›"
                    fontSize: 26

                    onClicked: {
                        if (root.hasController) {
                            root.controller.next()
                        }
                    }
                }

                ControlButton {
                    width: 40
                    height: 40

                    text: "↻"

                    onClicked: {
                        if (root.hasController) {
                            root.controller.toggleShuffle()
                        }
                    }
                }
            }
        }

        // ---------------------------------------------------------
        // Очередь
        // ---------------------------------------------------------

        Item {
            id: queuePanel

            width: mainRow.width * 0.42
            height: mainRow.height

            // Источник воспроизведения
            Text {
                id: sourceTitle

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top

                text: root.playbackSourceTitle
                color: AppTheme.textPrimary
                font.pixelSize: 20
                font.weight: Font.DemiBold

                elide: Text.ElideRight
                visible: text.length > 0
            }

            // Предыдущий трек
            Rectangle {
                id: previousTrackCard

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: sourceTitle.visible ? sourceTitle.bottom : parent.top
                anchors.topMargin: sourceTitle.visible ? 16 : 0

                height: 68
                radius: 10

                color: Qt.rgba(
                    root.playerAccent.r, root.playerAccent.g, root.playerAccent.b,
                    previousMouseArea.containsMouse ? 0.12 : 0.055)

                border.width: 1
                border.color: Qt.rgba(
                    root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.10)

                opacity: 0.72
                visible: root.hasPreviousTrack()

                Image {
                    id: previousArtwork

                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 9

                    width: 48
                    height: 48

                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    cache: true

                    source: {
                        const uri = String(root.previousTrackData().coverUri || "")
                        return uri.length > 0 ? "image://yandex/" + uri : ""
                    }

                    Rectangle {
                        anchors.fill: parent

                        visible: previousArtwork.status !== Image.Ready

                        radius: 6
                        color: AppTheme.artworkPlaceholder

                        Text {
                            anchors.centerIn: parent

                            text: "♪"
                            color: AppTheme.textMuted
                            font.pixelSize: 18
                        }
                    }
                }

                Column {
                    anchors.left: previousArtwork.right
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 12
                    anchors.rightMargin: 12

                    spacing: 2

                    Text {
                        width: parent.width

                        text: root.previousTrackData().title || ""
                        color: AppTheme.textSecondary
                        font.pixelSize: 13

                        elide: Text.ElideRight
                    }

                    Text {
                        width: parent.width

                        text: root.previousTrackData().artist || ""
                        color: AppTheme.textMuted
                        font.pixelSize: 12

                        elide: Text.ElideRight
                    }
                }

                MouseArea {
                    id: previousMouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    enabled: root.hasPreviousTrack()

                    onClicked: {
                        root.selectQueueTrack(root.currentTrackIndex() - 1)
                    }
                }
            }

            // Заголовок очереди + вкладки
            Row {
                id: queueHeader

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: previousTrackCard.visible
                    ? previousTrackCard.bottom
                    : (sourceTitle.visible ? sourceTitle.bottom : parent.top)
                anchors.topMargin: previousTrackCard.visible || sourceTitle.visible ? 22 : 0

                height: 36
                spacing: 16

                Rectangle {
                    id: upNextTab

                    width: upNextLabel.implicitWidth + 16
                    height: 32

                    radius: 6
                    color: !root.showingSimilar
                        ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
                        : "transparent"

                    Text {
                        id: upNextLabel

                        anchors.centerIn: parent

                        text: "Далее"
                        color: !root.showingSimilar ? root.playerAccent : AppTheme.textSecondary
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: root.showingSimilar = false
                    }
                }

                Rectangle {
                    id: similarTab

                    width: similarLabel.implicitWidth + 16
                    height: 32

                    radius: 6
                    color: root.showingSimilar
                        ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
                        : "transparent"

                    Text {
                        id: similarLabel

                        anchors.centerIn: parent

                        text: "Похожие"
                        color: root.showingSimilar ? root.playerAccent : AppTheme.textSecondary
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: root.showingSimilar = true
                    }
                }
            }

            // Очередь «Далее»
            ListView {
                id: queueView

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: queueHeader.bottom
                anchors.bottom: parent.bottom
                anchors.topMargin: 10

                spacing: 5
                clip: true

                visible: !root.showingSimilar

                model: upNextModel

                delegate: TrackListItem {
                    sourceIndex: model.sourceIndex
                    title: model.title
                    artist: model.artist
                    artistId: model.artistId
                    coverUri: model.coverUri
                    durationMs: model.durationMs

                    onActivated: root.selectQueueTrack(sourceIndex)
                }
            }

            // Похожие треки
            ListView {
                id: similarView

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: queueHeader.bottom
                anchors.bottom: parent.bottom
                anchors.topMargin: 10

                spacing: 5
                clip: true

                visible: root.showingSimilar

                model: root.hasController ? root.controller.similarTracksModel : null

                delegate: TrackListItem {
                    sourceIndex: model.sourceIndex
                    title: model.title
                    artist: model.artist
                    artistId: model.artistId
                    coverUri: model.coverUri
                    durationMs: model.durationMs

                    onActivated: {
                        if (root.hasController) {
                            root.controller.selectSimilarTrack(sourceIndex)
                        }
                    }
                }
            }
        }
    }

    // =============================================================
    // Модель очереди
    // =============================================================

    ListModel {
        id: upNextModel
    }

    // =============================================================
    // Обновление очереди
    // =============================================================

    Connections {
        target: root.controller

        function onCurrentTrackChanged() {
            root.rebuildUpNext()
        }

        function onQueueChanged() {
            root.rebuildUpNext()
        }

        function onPlaybackSourceChanged() {
            root.rebuildUpNext()
        }
    }

    Component.onCompleted: {
        root.rebuildUpNext()
    }

    // =============================================================
    // Помощники очереди
    // =============================================================

    function currentTrackIndex() {
        if (!root.hasController) {
            return -1
        }

        return Number(root.controller.queueCurrentIndex)
    }

    function hasPreviousTrack() {
        return root.currentTrackIndex() > 0
    }

    function previousTrackData() {
        const empty = {
            id: "", title: "", artist: "", artistId: "",
            coverUri: "", durationMs: 0
        }

        const currentIndex = root.currentTrackIndex()

        if (currentIndex <= 0 || !root.hasController) {
            return empty
        }

        const data = root.controller.queueTrackData(currentIndex - 1)

        return data !== null && data !== undefined ? data : empty
    }

    function rebuildUpNext() {
        upNextModel.clear()

        if (!root.hasController) {
            return
        }

        const count = Number(root.controller.queueCount)
        const currentIndex = Number(root.controller.queueCurrentIndex)

        if (count <= 0 || currentIndex < 0) {
            return
        }

        for (let i = currentIndex + 1; i < count; ++i) {
            const track = root.controller.queueTrackData(i)

            if (track === null || track === undefined) {
                continue
            }

            if (String(track.id || "").length === 0) {
                continue
            }

            upNextModel.append({
                sourceIndex: i,
                title: String(track.title || ""),
                artist: String(track.artist || ""),
                artistId: String(track.artistId || ""),
                coverUri: String(track.coverUri || ""),
                durationMs: Number(track.durationMs || 0)
            })
        }
    }

    function selectQueueTrack(index) {
        if (!root.hasController) {
            return
        }

        const sourceType = String(root.controller.playbackSourceType || "")

        switch (sourceType) {
            case "myWave":
                root.controller.selectMyWaveTrack(index)
                break
            case "playlist":
                root.controller.selectPlaylistTrack(index)
                break
            case "likes":
                root.controller.selectLikedTrack(index)
                break
            case "album":
                root.controller.selectAlbumTrack(index)
                break
            case "artist":
                root.controller.selectArtistTrack(index)
                break
            case "search":
                root.controller.selectSearchResult(index)
                break
            default:
                break
        }
    }

    // =============================================================
    // Кнопка управления (круглая, с hover-подсветкой)
    // =============================================================

    component ControlButton: Rectangle {
        property string text: ""
        property int fontSize: 21

        signal clicked()

        radius: width / 2

        color: mouseArea.containsMouse
            ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
            : "transparent"

        Behavior on color {
            ColorAnimation { duration: 140; easing.type: Easing.OutCubic }
        }

        Text {
            anchors.centerIn: parent

            text: parent.text
            color: AppTheme.textPrimary
            font.pixelSize: parent.fontSize
        }

        MouseArea {
            id: mouseArea

            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onClicked: parent.clicked()
        }
    }

    // =============================================================
    // Строка трека в очереди/похожих
    // =============================================================

    component TrackListItem: Item {
        id: item

        required property int sourceIndex
        required property string title
        required property string artist
        required property string artistId
        required property string coverUri
        required property int durationMs

        signal activated()

        width: parent ? parent.width : 0
        height: 60

        Rectangle {
            anchors.fill: parent
            radius: 8

            color: itemMouse.containsMouse
                ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
                : "transparent"

            Behavior on color {
                ColorAnimation { duration: 140; easing.type: Easing.OutCubic }
            }

            Image {
                id: itemArtwork

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                width: 46
                height: 46

                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: true

                source: item.coverUri.length > 0
                    ? "image://yandex/" + item.coverUri
                    : ""

                Rectangle {
                    anchors.fill: parent

                    visible: itemArtwork.status !== Image.Ready

                    radius: 6
                    color: AppTheme.artworkPlaceholder

                    Text {
                        anchors.centerIn: parent

                        text: "♪"
                        color: AppTheme.textMuted
                        font.pixelSize: 18
                    }
                }
            }

            Column {
                anchors.left: itemArtwork.right
                anchors.right: durationText.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 12
                anchors.rightMargin: 8

                spacing: 2

                Text {
                    width: parent.width

                    text: item.title || ""
                    color: AppTheme.textPrimary
                    font.pixelSize: 13

                    elide: Text.ElideRight
                }

                EntityLink {
                    text: item.artist || ""
                    entityId: item.artistId || ""
                    entityType: "artist"
                    controller: root.controller
                }
            }

            Text {
                id: durationText

                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter

                text: AppTheme.formatTime(item.durationMs)
                color: AppTheme.textMuted
                font.pixelSize: 11
            }

            MouseArea {
                id: itemMouse

                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked: function(mouse) {
                    if (mouse.button === Qt.LeftButton) {
                        item.activated()
                    }
                }

                onPressed: function(mouse) {
                    // Правый клик — копировать название/исполнителя
                    if (mouse.button !== Qt.RightButton || !root.hasController) {
                        return
                    }

                    root.controller.copyTrack(item.title, item.artist)
                }
            }
        }
    }
}