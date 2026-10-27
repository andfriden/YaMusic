import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Effects
import QtQuick.Window
import QtMultimedia
import YaMusic 1.0

// Компактное отдельное окно поверх других окон: диск-обложка, название,
// артист и кнопки управления. Живёт в собственном QQuickWindow, но делит
// с основным окном один QQmlEngine, поэтому видит AppTheme-сингилтон,
// контекстные свойства и image-провайдер "yandex".
Window {
    id: root

    property var controller

    width: 400
    height: 134

    // Поверх других окон, без рамки и без кнопки в таск-баре.
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    visible: false
    modality: Qt.NonModal

    readonly property bool hasController:
        root.controller !== null && root.controller !== undefined

    readonly property bool hasTrack:
        root.hasController &&
        String(root.controller.currentTrackTitle || "").length > 0

    readonly property bool playing:
        root.hasController && root.controller.playing

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

    readonly property string currentTrackId:
        root.hasController ? String(root.controller.currentTrackId || "") : ""

    readonly property bool currentTrackLiked:
        root.hasController && root.controller.currentTrackLiked

    // Дизлайк хранится в LikesService, а не в модели: пересчитываем
    // привязку по счётчику изменений, иначе кнопка не обновится.
    readonly property bool currentTrackDisliked: {
        if (!root.hasController) {
            return false
        }
        root.controller.dislikesRevision
        return root.controller.isTrackDisliked(root.currentTrackId)
    }

    // Расширенный плеер по клику на обложку.
    signal expandedRequested()

    // Переход в большое окно плеера по отдельной кнопке.
    signal expandRequested()

    // Закрытие мини-плеера по «×»: возврат в основное окно.
    signal closeRequested()

    function showPlayer() {
        root.show()
        root.raise()
        root.requestActivate()
    }

    function hidePlayer() {
        root.visible = false
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 6
        radius: 16

        color: root.hasPlayerAccent
            ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.18)
            : AppTheme.macchiatoBase

        border.width: 1
        border.color: root.hasPlayerAccent
            ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.35)
            : AppTheme.macchiatoSurface1

        // Перетаскивание окна за фон и заголовок. Объявлен первым, чтобы
        // лежать под кнопками и обложкой и не перехватывать их клики.
        MouseArea {
            id: dragArea

            anchors.fill: parent
            cursorShape: Qt.ArrowCursor

            property var dragOffset: null

            onPressed: function(mouse) {
                dragOffset = Qt.point(mouse.x, mouse.y)
            }

            onMouseXChanged: {
                if (dragOffset) {
                    root.setX(root.x + mouseX - dragOffset.x)
                }
            }

            onMouseYChanged: {
                if (dragOffset) {
                    root.setY(root.y + mouseY - dragOffset.y)
                }
            }

            onReleased: {
                dragOffset = null
            }
        }

        // Диск-обложка: круглый артворк, как на карточке артиста. Используем
        // провайдер "yandex/circle", который сам вырезает круг из обложки
        // (тот же приём, что на странице/карточке артиста). Круглое
        // изображение вращаем вокруг центра — ноль спрашиваний про клип,
        // углов квадрата и шейдеров.
        Rectangle {
            id: discSlot

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 16

            width: 84
            height: 84
            radius: width / 2
            clip: true

            // Тёмная подложка-винил под круглым артворком.
            Rectangle {
                anchors.fill: parent
                radius: width / 2
                color: "#161616"
            }

            Image {
                id: cover

                anchors.fill: parent

                source: root.hasTrack && root.controller.currentTrackCoverUri
                    ? "image://yandex/circle/" + root.controller.currentTrackCoverUri
                    : ""

                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: true
                smooth: true

                transform: Rotation {
                    origin.x: cover.width / 2
                    origin.y: cover.height / 2
                    angle: spinHolder.angle
                }

                // Пока артворк не загрузился — плэйсхолдер круга.
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width
                    height: parent.height
                    radius: width / 2
                    visible: cover.status !== Image.Ready
                    color: AppTheme.macchiatoSurface0

                    Icon {
                        anchors.centerIn: parent
                        glyph: "note"
                        color: AppTheme.macchiatoOverlay1
                        size: 24
                        strokeWidth: 1.7
                    }
                }
            }

            // Плавное вращение угла картинки.
            QtObject {
                id: spinHolder
                property real angle: 0
            }

            SequentialAnimation {
                id: spinAnim
                loops: Animation.Infinite
                running: root.hasTrack && root.playing

                NumberAnimation {
                    target: spinHolder
                    property: "angle"
                    from: 0
                    to: 360
                    duration: 6000
                    easing.type: Easing.Linear
                }
            }

            // Центр (отверстие) пластинки.
            Rectangle {
                id: discHole

                anchors.centerIn: parent
                width: 10
                height: 10
                radius: 5
                color: "#0a0a0a"
                border.width: 1
                border.color: Qt.rgba(1, 1, 1, 0.3)
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: root.expandedRequested()
            }
        }

        // Инфо и кнопки.
        Column {
            anchors.left: discSlot.right
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 14
            anchors.rightMargin: 12

            spacing: 4

            Item {
                width: parent.width
                height: titleText.height

                Text {
                    id: titleText

                    width: parent.width

                    text: root.hasTrack ? root.controller.currentTrackTitle : "Нет трека"
                    color: titleMouseArea.containsMouse && titleMouseArea.enabled
                        ? AppTheme.accent
                        : AppTheme.macchiatoText
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                    elide: Text.ElideRight
                    maximumLineCount: 1
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
                width: parent.width

                clip: true

                artists: root.hasTrack && root.controller
                    ? root.controller.currentTrackArtists
                    : []
                controller: root.controller
                pixelSize: 12
                baseColor: AppTheme.macchiatoSubtext1
            }

            Row {
                width: parent.width
                spacing: 6

                Item {
                    width: 34
                    height: 34

                    Rectangle {
                        anchors.fill: parent
                        radius: 17
                        color: likeArea.containsMouse ? Qt.rgba(
                            root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
                            : "transparent"
                    }

                    Icon {
                        anchors.centerIn: parent

                        glyph: root.currentTrackLiked ? "heartFilled" : "heart"
                        color: root.currentTrackLiked
                            ? AppTheme.accent
                            : AppTheme.macchiatoSubtext1
                        size: 20
                        strokeWidth: 1.7

                        Behavior on color {
                            ColorAnimation { duration: 120 }
                        }
                    }

                    MouseArea {
                        id: likeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        enabled: root.hasTrack && root.controller &&
                            root.currentTrackId.length > 0

                        onClicked: {
                            if (!root.controller) {
                                return
                            }
                            root.controller.toggleLike(root.currentTrackId,
                                root.currentTrackLiked)
                        }
                    }

                    PlayerTip {
                        text: root.currentTrackLiked
                            ? qsTr("Убрать из «Мне нравится»")
                            : qsTr("Мне нравится")
                        target: likeArea
                    }
                }

                Item {
                    width: 34
                    height: 34

                    Rectangle {
                        anchors.fill: parent
                        radius: 17
                        color: dislikeArea.containsMouse ? Qt.rgba(
                            root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
                            : "transparent"
                    }

                    Icon {
                        anchors.centerIn: parent

                        glyph: "heartOff"
                        color: root.currentTrackDisliked
                            ? AppTheme.accent
                            : AppTheme.macchiatoSubtext1
                        size: 20
                        strokeWidth: 1.7

                        Behavior on color {
                            ColorAnimation { duration: 120 }
                        }
                    }

                    MouseArea {
                        id: dislikeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        enabled: root.hasTrack && root.controller &&
                            root.currentTrackId.length > 0

                        onClicked: {
                            if (!root.controller) {
                                return
                            }
                            root.controller.toggleDislike(root.currentTrackId,
                                root.currentTrackDisliked)
                        }
                    }

                    PlayerTip {
                        text: root.currentTrackDisliked
                            ? qsTr("Убрать дизлайк")
                            : qsTr("Не нравится")
                        target: dislikeArea
                    }
                }

                Item {
                    width: 34
                    height: 34

                    Rectangle {
                        anchors.fill: parent
                        radius: 17
                        color: prevArea.containsMouse ? Qt.rgba(
                            root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
                            : "transparent"
                    }

                    Icon {
                        anchors.centerIn: parent
                        glyph: "chevronLeft"
                        color: AppTheme.macchiatoText
                        size: 22
                        strokeWidth: 2
                    }

                    MouseArea {
                        id: prevArea
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
                        target: prevArea
                    }
                }

                Item {
                    width: 36
                    height: 36

                    Rectangle {
                        id: miniPlayBg
                        anchors.fill: parent
                        radius: 18
                        color: root.playerAccent
                    }

                    Icon {
                        anchors.centerIn: parent
                        glyph: root.playing ? "pause" : "play"
                        color: AppTheme.accentOn
                        size: 16
                    }

                    MouseArea {
                        id: playArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (!root.controller) {
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
                        text: root.playing
                            ? qsTr("Пауза")
                            : qsTr("Воспроизвести")
                        target: playArea
                    }
                }

                Item {
                    width: 34
                    height: 34

                    Rectangle {
                        anchors.fill: parent
                        radius: 17
                        color: nextArea.containsMouse ? Qt.rgba(
                            root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.14)
                            : "transparent"
                    }

                    Icon {
                        anchors.centerIn: parent
                        glyph: "chevronRight"
                        color: AppTheme.macchiatoText
                        size: 22
                        strokeWidth: 2
                    }

                    MouseArea {
                        id: nextArea
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
                        target: nextArea
                    }
                }
            }
        }

        // Прогресс-бар воспроизведения.
        Rectangle {
            id: progressTrack

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            anchors.bottomMargin: 8

            height: 3
            radius: 1.5

            color: AppTheme.macchiatoSurface0

            Rectangle {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                width: {
                    if (!root.hasController || Number(root.controller.duration) <= 0) {
                        return 0
                    }
                    const ratio = Math.max(0, Math.min(1,
                        Number(root.controller.position) / Number(root.controller.duration)))
                    return parent.width * ratio
                }
                height: parent.height
                radius: parent.radius

                color: root.playerAccent
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
        }

        // Кнопка в правом верхнем углу: переход в большое окно.
        Item {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 6
            anchors.rightMargin: 6

            width: 24
            height: 24

            Icon {
                anchors.centerIn: parent
                glyph: "expand"
                color: expandArea.containsMouse ? AppTheme.macchiatoText : AppTheme.macchiatoOverlay1
                size: 16
                strokeWidth: 1.8
            }

            MouseArea {
                id: expandArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: root.expandRequested()
            }

            PlayerTip {
                text: qsTr("Открыть в окне плеера")
                target: expandArea
                alignRight: true
                below: true
            }
        }
    }
}