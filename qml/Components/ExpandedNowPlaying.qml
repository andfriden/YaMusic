import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Effects
import YaMusic 1.0

Item {
    id: root

    property var controller

    signal closed()

    anchors.fill: parent

    readonly property bool hasController:
        root.controller !== null && root.controller !== undefined

    readonly property bool hasTrack:
        root.hasController &&
        String(root.controller.currentTrackTitle || "").length > 0

    readonly property bool playing:
        root.hasController && root.controller.playing

    readonly property bool currentTrackLiked:
        root.hasTrack && root.controller.currentTrackLiked

    // Дизлайк хранится в LikesService: пересчитываем по счётчику изменений.
    readonly property bool currentTrackDisliked: {
        if (!root.hasController) {
            return false
        }

        root.controller.dislikesRevision
        return root.controller.isTrackDisliked(String(root.controller.currentTrackId || ""))
    }

    PlaylistPicker {
        id: addToPlaylistPopup

        controller: root.controller
    }

    TrackContextMenu {
        id: trackMenu

        controller: root.controller

        trackId: root.hasTrack ? String(root.controller.currentTrackId || "") : ""
        trackTitle: root.hasTrack ? String(root.controller.currentTrackTitle || "") : ""
        artist: root.hasTrack ? String(root.controller.currentTrackArtist || "") : ""
        artistId: root.hasTrack ? String(root.controller.currentTrackArtistId || "") : ""
        album: root.hasTrack ? String(root.controller.currentTrackAlbumTitle || "") : ""
        albumId: root.hasTrack ? String(root.controller.currentTrackAlbumId || "") : ""
        coverUri: root.hasTrack ? String(root.controller.currentTrackCoverUri || "") : ""
        durationMs: root.hasTrack ? Number(root.controller.duration || 0) : 0
        liked: root.currentTrackLiked
        disliked: root.currentTrackDisliked
        showAdd: true

        onAddToPlaylistRequested: {
            if (!root.hasController) {
                return
            }

            addToPlaylistPopup.trackId = trackMenu.trackId
            addToPlaylistPopup.albumId = trackMenu.albumId
            addToPlaylistPopup.open()
        }
    }

    readonly property bool hasPlayerAccent:
        root.hasController &&
        root.controller.accentController.playerAccent !== undefined &&
        root.controller.accentController.playerAccent !== null &&
        root.controller.accentController.playerAccent.valid

    readonly property color playerAccent:
        root.hasPlayerAccent
            ? root.controller.accentController.playerAccent
            : AppTheme.accent

    // Бейджи текущего трека: AI и «Без цензуры». Ревизии пересчитывают
    // привязку после загрузки списков.
    readonly property bool currentTrackAi: {
        if (!root.hasController) {
            return false
        }
        const id = String(root.controller.currentTrackArtistId || "")
        if (id.length === 0) {
            return false
        }
        root.controller.aiRevision
        return root.controller.isAiArtist(id)
    }

    readonly property bool currentTrackUncensored: {
        if (!root.hasController) {
            return false
        }
        const id = String(root.controller.currentTrackId || "")
        if (id.length === 0) {
            return false
        }
        root.controller.censorRevision
        return root.controller.isTrackUncensored(id)
    }

    readonly property string playbackSourceTitle:
        root.hasController
            ? String(root.controller.playbackSourceTitle || "")
            : ""

    readonly property string playbackSourceType:
        root.hasController
            ? String(root.controller.playbackSourceType || "")
            : ""

    property bool showingSimilar: false

    Rectangle {
        anchors.fill: parent
        color: AppTheme.background
    }

    Rectangle {
        anchors.fill: parent

        color: root.hasPlayerAccent
            ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.24)
            : "transparent"

        Behavior on color {
            ColorAnimation { duration: 450; easing.type: Easing.OutCubic }
        }
    }

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

    Item {
        id: header

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 36
        anchors.rightMargin: 36
        anchors.topMargin: 22

        height: 40

        Item {
            id: closeButton

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            width: 42
            height: 42

            Rectangle {
                anchors.fill: parent
                radius: width / 2

                // Цвет обложки (playerAccent) — непрозрачный, чтобы
                // кнопка не сливалась с таким же полупрозрачным фоном.
                color: root.playerAccent

                // Тень и светлая обводка отделяют кнопку от фона.
                border.width: 2
                border.color: Qt.rgba(1, 1, 1, 0.22)

                Behavior on color {
                    ColorAnimation { duration: 140; easing.type: Easing.OutCubic }
                }
            }

            Icon {
                anchors.centerIn: parent

                glyph: "chevronDown"
                color: AppTheme.accentOn
                size: 24
                strokeWidth: 2
            }

            MouseArea {
                id: closeMouseArea

                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: root.closed()
            }

            PlayerTip {
                text: qsTr("Свернуть")
                target: closeMouseArea
                below: true
            }
        }
    }

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

        Item {
            id: playerPanel

            width: mainRow.width * 0.58
            height: mainRow.height

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

                    Icon {
                        anchors.centerIn: parent

                        glyph: "note"
                        color: AppTheme.textMuted
                        size: 48
                        strokeWidth: 1.6
                    }
                }
            }

            Column {
                id: trackInfo

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: artwork.bottom
                anchors.topMargin: 18

                spacing: 4

                TapHandler {
                    acceptedButtons: Qt.RightButton

                    enabled: root.hasTrack

                    onTapped: trackMenu.popup()
                }

                Item {
                    width: parent.width
                    height: titleText.height

                    Text {
                        id: titleText

                        width: parent.width

                        text: root.hasTrack
                            ? String(root.controller.currentTrackTitle || "")
                            : ""

                        color: titleMouseArea.containsMouse && titleMouseArea.enabled
                            ? AppTheme.accent
                            : AppTheme.textPrimary
                        font.pixelSize: 22
                        font.weight: Font.DemiBold

                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }

                    MouseArea {
                        id: titleMouseArea

                        anchors.fill: parent

                        hoverEnabled: true
                        enabled: root.hasTrack && root.controller &&
                            String(root.controller.currentTrackAlbumId || "").length > 0

                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                        onClicked: {
                            if (!root.controller) {
                                return
                            }

                            root.controller.loadAlbum(root.controller.currentTrackAlbumId)
                        }
                    }
                }

                ArtistLinks {
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: Math.min(implicitWidth, trackInfo.width)
                    clip: true

                    artists: root.hasTrack && root.controller
                        ? root.controller.currentTrackArtists
                        : []
                    controller: root.controller
                    pixelSize: Qt.application.font.pixelSize
                    baseColor: AppTheme.textSecondary
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

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter

                    visible: root.currentTrackAi || root.currentTrackUncensored
                    spacing: 6

                    Badge {
                        visible: root.currentTrackAi
                        text: "AI"
                        badgeColor: AppTheme.warning
                    }

                    Badge {
                        visible: root.currentTrackUncensored
                        text: "Без цензуры"
                        badgeColor: AppTheme.info
                    }
                }
            }

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

            Row {
                id: controls

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: progressArea.bottom
                anchors.topMargin: 14

                spacing: 14

                ControlButton {
                    width: 40
                    height: 40

                    glyph: "repeat"

                    tip: {
                        const mode = root.hasController ? root.controller.repeatMode : 0

                        if (mode === 1) {
                            return qsTr("Повтор трека")
                        }
                        if (mode === 2) {
                            return qsTr("Повтор списка")
                        }
                        return qsTr("Повтор выключен")
                    }

                    onClicked: {
                        if (root.hasController) {
                            root.controller.cycleRepeat()
                        }
                    }
                }

                ControlButton {
                    width: 44
                    height: 44

                    glyph: "chevronLeft"
                    iconSize: 26
                    iconWidth: 2

                    tip: qsTr("Предыдущий трек")

                    onClicked: {
                        if (root.hasController) {
                            root.controller.previous()
                        }
                    }
                }

                // Пульсирующее свечение вокруг большой кнопки play.
                Item {
                    width: 56
                    height: 56

                    MultiEffect {
                        anchors.fill: parent
                        source: bigPlayButton
                        shadowEnabled: true
                        shadowBlur: 0.7
                        shadowColor: root.playerAccent
                        shadowHorizontalOffset: 0
                        shadowVerticalOffset: 0
                        shadowOpacity: root.playing ? glowStrengthHolder.value : 0

                        Behavior on shadowOpacity {
                            NumberAnimation { duration: 250 }
                        }
                    }

                    QtObject {
                        id: glowStrengthHolder
                        property real value: 0.45
                    }

                    SequentialAnimation {
                        id: bigPulseAnim
                        loops: Animation.Infinite
                        running: root.playing

                        NumberAnimation {
                            target: glowStrengthHolder
                            property: "value"
                            from: 0.25
                            to: 0.75
                            duration: 1300
                            easing.type: Easing.InOutSine
                        }
                        NumberAnimation {
                            target: glowStrengthHolder
                            property: "value"
                            from: 0.75
                            to: 0.25
                            duration: 1300
                            easing.type: Easing.InOutSine
                        }
                    }

                    Rectangle {
                        id: bigPlayButton
                        anchors.fill: parent

                        radius: width / 2
                        color: root.playerAccent

                        Icon {
                            anchors.centerIn: parent

                            glyph: root.playing ? "pause" : "play"
                            color: AppTheme.accentOn
                            size: 24
                        }

                        MouseArea {
                            id: bigPlayMouseArea

                            anchors.fill: parent
                            hoverEnabled: true
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

                        PlayerTip {
                            text: root.playing ? qsTr("Пауза") : qsTr("Воспроизвести")
                            target: bigPlayMouseArea
                        }
                    }
                }

                ControlButton {
                    width: 44
                    height: 44

                    glyph: "chevronRight"
                    iconSize: 26
                    iconWidth: 2

                    tip: qsTr("Следующий трек")

                    onClicked: {
                        if (root.hasController) {
                            root.controller.next()
                        }
                    }
                }

                ControlButton {
                    width: 40
                    height: 40

                    glyph: "shuffle"

                    tip: qsTr("Перемешать")

                    onClicked: {
                        if (root.hasController) {
                            root.controller.toggleShuffle()
                        }
                    }
                }
            }

            Row {
                id: downloadRow

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: controls.bottom
                anchors.topMargin: 16

                spacing: 8

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter

                    visible: root.hasTrack && root.hasController &&
                        root.controller.downloadService !== null

                    width: downloadLabel.implicitWidth + 34
                    height: 36
                    radius: 18

                    color: downloadMouseArea.containsMouse
                        ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
                        : Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.08)

                    Behavior on color {
                        ColorAnimation { duration: 140; easing.type: Easing.OutCubic }
                    }

                    Text {
                        id: downloadLabel

                        anchors.centerIn: parent

                        readonly property int dlState:
                            root.hasController && root.controller.downloadService
                                ? root.controller.downloadService.state
                                : 0

                        text: {
                            if (downloadLabel.dlState === 1) {
                                return "Скачивание…"
                            }
                            if (downloadLabel.dlState === 2) {
                                return "✓ Скачано"
                            }
                            if (downloadLabel.dlState === 3) {
                                return "Ошибка — повторить"
                            }
                            return "↓ Скачать"
                        }
                        color: downloadLabel.dlState === 2
                            ? AppTheme.accent
                            : (downloadLabel.dlState === 3 ? AppTheme.error : AppTheme.textPrimary)
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }

                    MouseArea {
                        id: downloadMouseArea

                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.hasController) {
                                root.controller.downloadCurrentTrack()
                            }
                        }
                    }
                }
            }
        }

        Item {
            id: queuePanel

            width: mainRow.width * 0.42
            height: mainRow.height

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

                        Icon {
                            anchors.centerIn: parent

                            glyph: "note"
                            color: AppTheme.textMuted
                            size: 18
                            strokeWidth: 1.7
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
                    onActivated: root.selectQueueTrack(sourceIndex)
                }
            }

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
                    artists: model.artists

                    onActivated: {
                        if (root.hasController) {
                            root.controller.selectSimilarTrack(sourceIndex)
                        }
                    }
                }
            }
        }
    }

    ListModel {
        id: upNextModel
    }

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

    component ControlButton: Rectangle {
        id: controlButton

        property string text: ""
        property string tip: ""
        property int fontSize: 21
        property string glyph: ""
        property real iconSize: 22
        property real iconWidth: 1.8
        property color iconColor: AppTheme.textPrimary

        signal clicked()

        radius: width / 2

        color: mouseArea.containsMouse
            ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
            : "transparent"

        Behavior on color {
            ColorAnimation { duration: 140; easing.type: Easing.OutCubic }
        }

        Icon {
            anchors.centerIn: parent

            visible: parent.glyph.length > 0

            glyph: parent.glyph
            color: parent.iconColor
            size: parent.iconSize
            strokeWidth: parent.iconWidth
        }

        Text {
            anchors.centerIn: parent

            visible: parent.glyph.length === 0

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

        PlayerTip {
            text: controlButton.tip
            target: mouseArea
        }
    }

    component TrackListItem: Item {
        id: item

        required property int sourceIndex
        required property string title
        required property string artist
        required property string artistId
        required property string coverUri
        required property int durationMs

        // Все исполнители трека: [{"id", "name"}]. Если список не задан,
        // ссылки строятся из одного artist/artistId.
        property var artists: []

        readonly property var linkArtists: {
            if (item.artists && item.artists.length > 0) {
                return item.artists
            }
            if (item.artist.length > 0) {
                return [{
                    id: item.artistId,
                    name: item.artist
                }]
            }
            return []
        }

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

                    Icon {
                        anchors.centerIn: parent

                        glyph: "note"
                        color: AppTheme.textMuted
                        size: 18
                        strokeWidth: 1.7
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

                ArtistLinks {
                    width: parent.width
                    clip: true

                    artists: item.linkArtists
                    controller: root.controller
                    pixelSize: Qt.application.font.pixelSize
                    baseColor: AppTheme.textSecondary
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
                    if (mouse.button !== Qt.RightButton || !root.hasController) {
                        return
                    }

                    root.controller.copyTrack(item.title, item.artist)
                }
            }
        }
    }
}