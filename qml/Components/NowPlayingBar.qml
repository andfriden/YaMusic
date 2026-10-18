import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import YaMusic 1.0

Rectangle {
    id: root

    property var controller

    signal expandedRequested()
    signal lyricsRequested()
    signal miniPlayerRequested()

    readonly property bool hasController:
        root.controller !== null && root.controller !== undefined

    readonly property bool hasTrack:
        root.hasController &&
        String(root.controller.currentTrackTitle || "").length > 0

    readonly property bool loading:
        root.hasController && root.controller.playbackState === 1

    readonly property bool playing:
        root.hasController && root.controller.playing

    // Дизлайк хранится в LikesService, а не в модели: пересчитываем
    // привязку по счётчику изменений, иначе кнопка не обновится.
    readonly property bool currentTrackDisliked: {
        if (!root.hasController) {
            return false
        }

        root.controller.dislikesRevision
        return root.controller.isTrackDisliked(String(root.controller.currentTrackId || ""))
    }

    readonly property bool hasPlayerAccent:
        root.hasTrack &&
        root.hasController &&
        root.controller.accentController.playerAccent !== undefined &&
        root.controller.accentController.playerAccent !== null &&
        root.controller.accentController.playerAccent.valid

    readonly property color playerAccent:
        root.hasPlayerAccent
            ? root.controller.accentController.playerAccent
            : AppTheme.accent

    // Бейджи текущего трека: AI и «Без цензуры». Считаем через контроллер,
    // ревизии пересчитывают привязку после загрузки списков.
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

    height: 124

    color: root.hasPlayerAccent
        ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.09)
        : AppTheme.panel

    border.color: root.hasPlayerAccent
        ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
        : AppTheme.border

    border.width: 1
    radius: 12

    // Декоративная градиентная кромка по верху панели.
    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: 2

        visible: root.hasPlayerAccent

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.0)
            }
            GradientStop {
                position: 0.5
                color: Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.5)
            }
            GradientStop {
                position: 1.0
                color: Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.0)
            }
        }
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
        liked: root.hasTrack && root.controller.currentTrackLiked
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

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 18
        anchors.rightMargin: 18
        anchors.topMargin: 12
        anchors.bottomMargin: 12

        spacing: 0

        RowLayout {
            id: trackInfo

            Layout.fillWidth: true
            Layout.minimumWidth: 250
            Layout.preferredWidth: 360
            Layout.maximumWidth: 430

            spacing: 12

            TapHandler {
                acceptedButtons: Qt.RightButton

                enabled: root.hasTrack

                onTapped: trackMenu.popup()
            }

            Image {
                id: cover

                Layout.preferredWidth: 58
                Layout.preferredHeight: 58

                source: root.hasTrack && root.controller.currentTrackCoverUri
                    ? "image://yandex/" + root.controller.currentTrackCoverUri
                    : ""

                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: true

                Rectangle {
                    anchors.fill: parent
                    radius: 9

                    color: "transparent"
                    border.color: AppTheme.borderSubtle
                    border.width: 1
                }
            }

            ColumnLayout {
                Layout.fillWidth: true

                spacing: 3

                Label {
                    id: titleLabel

                    Layout.fillWidth: true

                    text: root.hasTrack
                        ? root.controller.currentTrackTitle
                        : "Нет воспроизводимого трека"

                    color: titleMouseArea.containsMouse && titleMouseArea.enabled
                        ? AppTheme.accent
                        : AppTheme.textPrimary
                    font.pixelSize: 14
                    font.weight: Font.DemiBold

                    elide: Text.ElideRight
                    maximumLineCount: 1

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

                Label {
                    Layout.fillWidth: true

                    text: root.hasTrack ? root.controller.currentTrackArtist : ""

                    color: AppTheme.textSecondary
                    font.pixelSize: 12

                    elide: Text.ElideRight
                    maximumLineCount: 1

                    MouseArea {
                        anchors.fill: parent

                        enabled: root.hasTrack &&
                            String(root.controller.currentTrackArtistId || "").length > 0

                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                        onClicked: {
                            if (!root.controller) {
                                return
                            }

                            root.controller.loadArtist(root.controller.currentTrackArtistId)
                        }
                    }
                }

                Label {
                    Layout.fillWidth: true

                    visible: root.hasTrack &&
                        root.controller &&
                        String(root.controller.currentCodec || "").length > 0

                    text: {
                        const codec = root.controller.currentCodec || ""
                        const br = Number(root.controller.currentBitrate || 0)
                        const codecLabel = codec.toUpperCase()
                        return br > 0 ? codecLabel + " · " + br + " kbps" : codecLabel
                    }

                    color: AppTheme.textMuted
                    font.pixelSize: 11
                    font.letterSpacing: 0.5

                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                Row {
                    Layout.fillWidth: true

                    visible: root.hasTrack && (root.currentTrackAi || root.currentTrackUncensored)
                    spacing: 4

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
                id: likeButton

                Layout.preferredWidth: 36
                Layout.preferredHeight: 36

                visible: root.hasTrack

                Rectangle {
                    anchors.fill: parent
                    radius: 18

                    color: likeMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                    opacity: likeMouseArea.containsMouse ? 1 : 0

                    Behavior on opacity {
                        NumberAnimation { duration: 100 }
                    }
                }

                Icon {
                    anchors.centerIn: parent

                    glyph: root.controller && root.controller.currentTrackLiked ? "heartFilled" : "heart"
                    color: root.controller && root.controller.currentTrackLiked
                        ? AppTheme.accent
                        : AppTheme.textSecondary
                    size: 22
                    strokeWidth: 1.7

                    Behavior on color {
                        ColorAnimation { duration: 120 }
                    }
                }

                MouseArea {
                    id: likeMouseArea

                    anchors.fill: parent

                    hoverEnabled: true

                    enabled: root.hasTrack && root.hasController &&
                        String(root.controller.currentTrackId || "").length > 0

                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                    onClicked: {
                        if (!root.controller) {
                            return
                        }

                        const trackId = String(root.controller.currentTrackId || "")

                        if (trackId.length === 0) {
                            return
                        }

                        root.controller.toggleLike(trackId, root.controller.currentTrackLiked)
                    }
                }

                PlayerTip {
                    text: root.controller && root.controller.currentTrackLiked
                        ? qsTr("Убрать из «Мне нравится»")
                        : qsTr("Мне нравится")
                    target: likeMouseArea
                }
            }

            Item {
                id: dislikeButton

                Layout.preferredWidth: 36
                Layout.preferredHeight: 36

                visible: root.hasTrack

                Rectangle {
                    anchors.fill: parent
                    radius: 18

                    color: dislikeMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                    opacity: dislikeMouseArea.containsMouse ? 1 : 0

                    Behavior on opacity {
                        NumberAnimation { duration: 100 }
                    }
                }

                Icon {
                    anchors.centerIn: parent

                    glyph: "heartOff"
                    color: root.currentTrackDisliked
                        ? AppTheme.accent
                        : AppTheme.textSecondary
                    size: 22
                    strokeWidth: 1.7

                    Behavior on color {
                        ColorAnimation { duration: 120 }
                    }
                }

                MouseArea {
                    id: dislikeMouseArea

                    anchors.fill: parent

                    enabled: root.hasTrack && root.hasController &&
                        String(root.controller.currentTrackId || "").length > 0

                    hoverEnabled: true
                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                    onClicked: {
                        if (!root.controller) {
                            return
                        }

                        const trackId = String(root.controller.currentTrackId || "")

                        if (trackId.length === 0) {
                            return
                        }

                        root.controller.toggleDislike(trackId, root.currentTrackDisliked)
                    }
                }

                PlayerTip {
                    text: root.currentTrackDisliked
                        ? qsTr("Убрать дизлайк")
                        : qsTr("Не нравится")
                    target: dislikeMouseArea
                }
            }
        }

        Item {
            Layout.preferredWidth: 18
        }

        ColumnLayout {
            id: centerControls

            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 470

            spacing: 7

            RowLayout {
                Layout.alignment: Qt.AlignHCenter

                spacing: 8

                Item {
                    width: 38
                    height: 38

                    Rectangle {
                        anchors.fill: parent
                        radius: 19

                        color: shuffleMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                    }

                    Icon {
                        anchors.centerIn: parent

                        glyph: "shuffle"
                        color: root.controller && root.controller.shuffleEnabled
                            ? root.playerAccent
                            : AppTheme.textSecondary
                        size: 20
                        strokeWidth: 1.7
                    }

                    MouseArea {
                        id: shuffleMouseArea

                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller) {
                                root.controller.toggleShuffle()
                            }
                        }
                    }

                    PlayerTip {
                        text: qsTr("Перемешать")
                        target: shuffleMouseArea
                    }
                }

                Item {
                    width: 42
                    height: 42

                    Rectangle {
                        anchors.fill: parent
                        radius: 21

                        color: previousMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                    }

                    Icon {
                        anchors.centerIn: parent

                        glyph: "chevronLeft"
                        color: AppTheme.textPrimary
                        size: 28
                        strokeWidth: 1.9
                    }

                    MouseArea {
                        id: previousMouseArea

                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller) {
                                root.controller.previous()
                            }
                        }
                    }

                    PlayerTip {
                        text: qsTr("Предыдущий трек")
                        target: previousMouseArea
                    }
                }

                Item {
                    width: 50
                    height: 50

                    // Мягкое пульсирующее свечение вокруг кнопки при воспроизведении.
                    // В QtQuick.Effects свечение реализуется цветной тенью
                    // с размытием (shadowColor=акцент, амплитуда анимируется).
                    MultiEffect {
                        id: playGlow

                        anchors.fill: parent
                        source: playButton
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

                    // Посредник для плавной пульсации амплитуды свечения.
                    QtObject {
                        id: glowStrengthHolder
                        property real value: 0.45
                    }

                    SequentialAnimation {
                        id: pulseGlowAnim
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
                        id: playButton
                        anchors.fill: parent
                        radius: 25
                        color: root.playerAccent
                    }

                    Icon {
                        anchors.centerIn: parent

                        glyph: root.loading ? "note" : (root.playing ? "pause" : "play")
                        color: AppTheme.accentOn
                        size: root.playing ? 22 : 21
                    }

                    MouseArea {
                        id: playMouseArea

                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (!root.controller || root.loading) {
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
                        text: root.loading
                            ? qsTr("Загрузка")
                            : (root.playing ? qsTr("Пауза") : qsTr("Воспроизвести"))
                        target: playMouseArea
                    }
                }

                Item {
                    width: 42
                    height: 42

                    Rectangle {
                        anchors.fill: parent
                        radius: 21

                        color: nextMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                    }

                    Icon {
                        anchors.centerIn: parent

                        glyph: "chevronRight"
                        color: AppTheme.textPrimary
                        size: 28
                        strokeWidth: 1.9
                    }

                    MouseArea {
                        id: nextMouseArea

                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller) {
                                root.controller.next()
                            }
                        }
                    }

                    PlayerTip {
                        text: qsTr("Следующий трек")
                        target: nextMouseArea
                    }
                }

                Item {
                    width: 38
                    height: 38

                    Rectangle {
                        anchors.fill: parent
                        radius: 19

                        color: repeatMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                    }

                    Icon {
                        anchors.centerIn: parent

                        glyph: root.controller && root.controller.repeatMode === 1 ? "repeatOne" : "repeat"
                        color: root.controller && root.controller.repeatMode !== 0
                            ? root.playerAccent
                            : AppTheme.textSecondary
                        size: 19
                        strokeWidth: 1.7
                    }

                    MouseArea {
                        id: repeatMouseArea

                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.controller) {
                                root.controller.cycleRepeat()
                            }
                        }
                    }

                    PlayerTip {
                        text: {
                            const mode = root.controller ? root.controller.repeatMode : 0

                            if (mode === 1) {
                                return qsTr("Повтор трека")
                            }
                            if (mode === 2) {
                                return qsTr("Повтор списка")
                            }
                            return qsTr("Повтор выключен")
                        }
                        target: repeatMouseArea
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true

                spacing: 8

                Label {
                    Layout.preferredWidth: 38

                    text: AppTheme.formatTime(root.controller ? root.controller.position : 0)
                    color: AppTheme.textMuted
                    font.pixelSize: 10
                    horizontalAlignment: Text.AlignRight
                }

                Item {
                    id: progressArea

                    Layout.fillWidth: true
                    Layout.preferredHeight: 12

                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right

                        height: 4
                        radius: 2

                        color: AppTheme.panelSecondary

                        Rectangle {
                            width: root.controller && root.controller.duration > 0
                                ? parent.width * Math.max(0, Math.min(1,
                                    root.controller.position / root.controller.duration))
                                : 0

                            height: parent.height
                            radius: 2

                            color: root.playerAccent
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked: function(mouse) {
                            if (!root.controller || root.controller.duration <= 0) {
                                return
                            }

                            const ratio = Math.max(0, Math.min(1, mouse.x / width))

                            root.controller.seek(Math.round(root.controller.duration * ratio))
                        }
                    }
                }

                Label {
                    Layout.preferredWidth: 38

                    text: AppTheme.formatTime(root.controller ? root.controller.duration : 0)
                    color: AppTheme.textMuted
                    font.pixelSize: 10
                    horizontalAlignment: Text.AlignLeft
                }
            }
        }

        Item {
            Layout.preferredWidth: 18
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.minimumWidth: 250
            Layout.preferredWidth: 360
            Layout.maximumWidth: 430

            spacing: 8

            Item {
                Layout.fillWidth: true
            }

            Item {
                width: 38
                height: 38

                visible: root.hasTrack

                Rectangle {
                    anchors.fill: parent
                    radius: 19

                    color: downloadMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                }

                Label {
                    id: downloadIcon

                    anchors.centerIn: parent

                    property int dlState:
                        root.hasController && root.controller.downloadService
                            ? root.controller.downloadService.state
                            : 0

                    text: downloadIcon.dlState === 1 ? "…" : "↓"
                    color: downloadIcon.dlState === 2
                        ? AppTheme.accent
                        : (downloadIcon.dlState === 3 ? AppTheme.error : AppTheme.textSecondary)
                    font.pixelSize: 16
                }

                MouseArea {
                    id: downloadMouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    enabled: root.hasTrack && root.hasController

                    onClicked: {
                        if (!root.controller) {
                            return
                        }

                        root.controller.downloadCurrentTrack()
                    }
                }

                PlayerTip {
                    text: {
                        const state = downloadIcon.dlState

                        if (state === 1) {
                            return qsTr("Скачивание…")
                        }
                        if (state === 2) {
                            return qsTr("Скачано")
                        }
                        if (state === 3) {
                            return qsTr("Повторить скачивание")
                        }
                        return qsTr("Скачать трек")
                    }
                    target: downloadMouseArea
                }
            }

            Item {
                width: 38
                height: 38

                visible: root.hasTrack

                Rectangle {
                    anchors.fill: parent
                    radius: 19

                    color: lyricsMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                }

                Icon {
                    anchors.centerIn: parent

                    glyph: "lyrics"
                    color: AppTheme.textSecondary
                    size: 19
                    strokeWidth: 1.7
                }

                MouseArea {
                    id: lyricsMouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.lyricsRequested()
                }

                PlayerTip {
                    text: qsTr("Текст песни")
                    target: lyricsMouseArea
                }
            }

            Item {
                width: 38
                height: 38

                Rectangle {
                    anchors.fill: parent
                    radius: 19

                    color: volumeMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                }

                Label {
                    anchors.centerIn: parent

                    text: root.controller && root.controller.volume > 0 ? "◖" : "×"
                    color: AppTheme.textSecondary
                    font.pixelSize: 19
                }

                MouseArea {
                    id: volumeMouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        if (!root.controller) {
                            return
                        }

                        if (root.controller.volume > 0) {
                            root.controller.setVolume(0)
                        } else {
                            root.controller.setVolume(1)
                        }
                    }
                }

                PlayerTip {
                    text: root.controller && root.controller.volume > 0
                        ? qsTr("Выключить звук")
                        : qsTr("Включить звук")
                    target: volumeMouseArea
                }
            }

            Slider {
                id: volumeSlider

                Layout.preferredWidth: 105
                Layout.maximumWidth: 105

                from: 0
                to: 1

                value: root.controller ? root.controller.volume : 0

                onMoved: {
                    if (root.controller) {
                        root.controller.setVolume(value)
                    }
                }

                background: Rectangle {
                    x: volumeSlider.leftPadding
                    y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2

                    width: volumeSlider.availableWidth
                    height: 4
                    radius: 2

                    color: AppTheme.panelSecondary

                    Rectangle {
                        width: volumeSlider.visualPosition * parent.width
                        height: parent.height
                        radius: 2

                        color: root.playerAccent
                    }
                }

                handle: Rectangle {
                    x: volumeSlider.leftPadding +
                       volumeSlider.visualPosition * (volumeSlider.availableWidth - width)
                    y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2

                    width: 10
                    height: 10
                    radius: 5

                    color: root.playerAccent
                }
            }

            Item {
                width: 38
                height: 38

                Rectangle {
                    anchors.fill: parent
                    radius: 19

                    color: miniMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                }

                Icon {
                    anchors.centerIn: parent

                    glyph: "mini"
                    color: AppTheme.textSecondary
                    size: 18
                    strokeWidth: 1.7
                }

                MouseArea {
                    id: miniMouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.miniPlayerRequested()
                }

                PlayerTip {
                    text: qsTr("Мини-плеер")
                    target: miniMouseArea
                }
            }

            Item {
                width: 38
                height: 38

                Rectangle {
                    anchors.fill: parent
                    radius: 19

                    color: expandMouseArea.containsMouse ? AppTheme.panelHover : "transparent"
                }

                Icon {
                    anchors.centerIn: parent

                    glyph: "expand"
                    color: AppTheme.textSecondary
                    size: 18
                    strokeWidth: 1.7
                }

                MouseArea {
                    id: expandMouseArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.expandedRequested()
                }

                PlayerTip {
                    text: qsTr("Развернуть плеер")
                    target: expandMouseArea
                    alignRight: true
                }
            }
        }
    }
}