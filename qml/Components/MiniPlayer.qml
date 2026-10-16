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

    width: 380
    height: 116

    // Поверх других окон, без рамки и без кнопки в таск-баре.
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint |
        Qt.Tool | Qt.WindowDoesNotAcceptFocus
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

    function showPlayer() {
        root.visible = true
    }

    function hidePlayer() {
        root.visible = false
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 6
        radius: 16

        color: root.hasPlayerAccent
            ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.12)
            : AppTheme.panel

        border.width: 1
        border.color: root.hasPlayerAccent
            ? Qt.rgba(root.playerAccent.r, root.playerAccent.g, root.playerAccent.b, 0.25)
            : AppTheme.border

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
                    color: AppTheme.artworkPlaceholder

                    Text {
                        anchors.centerIn: parent
                        text: "♪"
                        color: AppTheme.textMuted
                        font.pixelSize: 20
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

            Text {
                width: parent.width

                text: root.hasTrack ? root.controller.currentTrackTitle : "Нет трека"
                color: AppTheme.textPrimary
                font.pixelSize: 14
                font.weight: Font.DemiBold
                elide: Text.ElideRight
                maximumLineCount: 1
            }

            Text {
                width: parent.width

                text: root.hasTrack ? root.controller.currentTrackArtist : ""
                color: AppTheme.textSecondary
                font.pixelSize: 12
                elide: Text.ElideRight
                maximumLineCount: 1
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

                    Text {
                        anchors.centerIn: parent

                        text: root.currentTrackLiked ? "♥" : "♡"
                        color: root.currentTrackLiked
                            ? AppTheme.accent
                            : AppTheme.textSecondary
                        font.pixelSize: 18

                        Behavior on color {
                            ColorAnimation { duration: 120 }
                        }
                    }

                    MouseArea {
                        id: likeArea
                        anchors.fill: parent
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

                    Text {
                        anchors.centerIn: parent

                        text: "♡"
                        color: root.currentTrackDisliked
                            ? AppTheme.accent
                            : AppTheme.textSecondary
                        font.pixelSize: 18

                        Behavior on color {
                            ColorAnimation { duration: 120 }
                        }
                    }

                    // Перечёркнутое сердце — признак дизлайка.
                    Rectangle {
                        anchors.centerIn: parent

                        width: 16
                        height: 1.5
                        radius: 0.75

                        color: root.currentTrackDisliked
                            ? AppTheme.accent
                            : AppTheme.textSecondary
                        rotation: -45

                        visible: root.currentTrackDisliked
                    }

                    MouseArea {
                        id: dislikeArea
                        anchors.fill: parent
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

                    Text {
                        anchors.centerIn: parent
                        text: "‹"
                        color: AppTheme.textPrimary
                        font.pixelSize: 24
                        font.weight: Font.Light
                    }

                    MouseArea {
                        id: prevArea
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (root.controller) {
                                root.controller.previous()
                            }
                        }
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

                    Text {
                        anchors.centerIn: parent
                        text: root.playing ? "Ⅱ" : "▶"
                        color: AppTheme.accentOn
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }

                    MouseArea {
                        anchors.fill: parent
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

                    Text {
                        anchors.centerIn: parent
                        text: "›"
                        color: AppTheme.textPrimary
                        font.pixelSize: 24
                        font.weight: Font.Light
                    }

                    MouseArea {
                        id: nextArea
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (root.controller) {
                                root.controller.next()
                            }
                        }
                    }
                }
            }
        }

        // Кнопка закрытия мини-плеера.
        Item {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 6
            anchors.rightMargin: 6

            width: 24
            height: 24

            Text {
                anchors.centerIn: parent
                text: "×"
                color: closeArea.containsMouse ? AppTheme.textPrimary : AppTheme.textMuted
                font.pixelSize: 18
            }

            MouseArea {
                id: closeArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: root.hidePlayer()
            }
        }
    }
}