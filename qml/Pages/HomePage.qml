import QtQuick
import QtQuick.Controls.Basic
import YaMusic 1.0

Item {
    id: root

    property var controller

    width: parent ? parent.width : 0
    implicitWidth: width
    implicitHeight: content.implicitHeight
    height: implicitHeight

    signal waveRequested()
    signal likedRequested()
    signal libraryRequested()
    signal releasesRequested()
    signal playlistsRequested()
    signal stationPageRequested(string stationType, string stationTag, string title)

    readonly property var personalController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.personalController !== undefined
            ? root.controller.personalController
            : null

    readonly property var stationsController:
        root.controller !== null && root.controller !== undefined &&
        root.controller.stationsController !== undefined
            ? root.controller.stationsController
            : null

    readonly property var wheelItems:
        root.personalController !== null && root.personalController.wheelItems
            ? root.personalController.wheelItems
            : []

    // «Новое для тебя»: альбомы, подборки, артисты из Колеса.
    readonly property var picks:
        root.wheelItems.filter((item) => item.type !== "wave")

    // «Волны по настроению»: элементы Колеса типа "wave".
    readonly property var waves:
        root.wheelItems.filter((item) => item.type === "wave")

    readonly property int cardWidth: 150
    readonly property int cardSpacing: 18

    Component.onCompleted: {
        if (root.controller !== null && root.controller !== undefined) {
            // «Мою волну» не трогаем: она загружается своей страницей,
            // иначе возврат на главную сбрасывал бы очередь плеера.
            root.controller.loadRecommendations()
            root.controller.loadWheel()
        }
    }

    readonly property string accountName:
        root.controller !== null && root.controller !== undefined
            ? String(root.controller.accountName || "")
            : ""

    function greeting() {
        const hour = new Date().getHours()
        let text = ""

        if (hour < 6) text = "Доброй ночи"
        else if (hour < 12) text = "Доброе утро"
        else if (hour < 18) text = "Добрый день"
        else text = "Добрый вечер"

        const name = root.accountName.trim()

        return name.length > 0 ? text + ", " + name : text
    }

    function startWheelWave(item) {
        if (root.stationsController === null) {
            return
        }

        // Сид станции вида "type:tag" — разбираем на составные части.
        const station = String(item.station || "")
        const separator = station.indexOf(":")
        const type = separator > 0 ? station.substring(0, separator) : ""
        const tag = separator > 0 ? station.substring(separator + 1) : station

        if (type.length > 0 && tag.length > 0) {
            root.stationsController.selectStation(type, tag, item.title || "Волна")
            return
        }

        // Нет сида — просто открываем страницу волны.
        root.waveRequested()
    }

    function openPick(item) {
        if (root.controller === null) {
            return
        }

        if (item.type === "album") {
            root.controller.loadAlbum(item.id)
        } else if (item.type === "artist") {
            root.controller.loadArtist(item.id)
        }
    }

    function playPick(item) {
        if (root.controller === null) {
            return
        }

        if (item.type === "album") {
            root.controller.playAlbum(item.id)
            return
        }

        // Для артиста и подборок запускаем волну по сиду.
        startWheelWave(item)
    }

    Column {
        id: content

        width: parent.width
        spacing: 26

        // ---- Приветствие ----
        Label {
            text: root.greeting()
            color: AppTheme.textPrimary
            font.pixelSize: 28
            font.bold: true
        }

        // ---- Кнопки-действия ----
        Flow {
            width: content.width
            spacing: 10

            ActionButton {
                text: "Моя волна"
                glyph: "play"
                emphasized: true

                onClicked: root.waveRequested()
            }

            ActionButton {
                text: "Новые релизы"
                glyph: "chart"

                onClicked: root.releasesRequested()
            }

            ActionButton {
                text: "Новые плейлисты"
                glyph: "playlist"

                onClicked: root.playlistsRequested()
            }

            ActionButton {
                text: "Мне нравится"
                glyph: "heartFilled"

                onClicked: root.likedRequested()
            }

            ActionButton {
                text: "Поиск в библиотеке"
                glyph: "search"

                onClicked: root.libraryRequested()
            }
        }

        // ---- Новое для тебя ----
        SectionCardGrid {
            width: content.width
            visible: root.picks.length > 0
            title: "Новое для тебя"
            model: root.picks
            cardWidth: root.cardWidth
            cardSpacing: root.cardSpacing
            roundArtwork: false

            onCardClicked: function(index, item) {
                root.openPick(item)
            }

            onPlayRequested: function(index, item) {
                root.playPick(item)
            }
        }

        // ---- Волны по настроению ----
        SectionCardGrid {
            width: content.width
            visible: root.waves.length > 0
            title: "Волны по настроению"
            model: root.waves
            cardWidth: root.cardWidth
            cardSpacing: root.cardSpacing
            roundArtwork: true

            onCardClicked: function(index, item) {
                // Клик по кругу открывает страницу станции.
                const station = String(item.station || "")
                const separator = station.indexOf(":")
                const type = separator > 0 ? station.substring(0, separator) : ""
                const tag = separator > 0 ? station.substring(separator + 1) : station

                if (type.length > 0 && tag.length > 0) {
                    root.stationPageRequested(type, tag, item.title || "Волна")
                }
            }

            onPlayRequested: function(index, item) {
                root.startWheelWave(item)
            }
        }

        PersonalPlaylistsSection {
            width: content.width
            controller: root.controller
            homeMode: true
        }
    }

    component ActionButton: Rectangle {
        id: actionButton

        property string text
        property string glyph
        property bool emphasized: false

        signal clicked()

        height: 44
        width: actionRow.implicitWidth + 32
        radius: 12

        color: actionMouseArea.containsMouse
            ? AppTheme.panelHover
            : (actionButton.emphasized ? AppTheme.accent : AppTheme.panel)

        Behavior on color {
            ColorAnimation { duration: 100 }
        }

        Row {
            id: actionRow

            anchors.centerIn: parent
            spacing: 8

            Icon {
                anchors.verticalCenter: parent.verticalCenter
                glyph: actionButton.glyph
                color: actionButton.emphasized
                    ? AppTheme.accentOn
                    : AppTheme.textSecondary
                size: 15
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter

                text: actionButton.text
                color: actionButton.emphasized
                    ? AppTheme.accentOn
                    : AppTheme.textPrimary
                font.pixelSize: 14
                font.weight: Font.Medium
            }
        }

        MouseArea {
            id: actionMouseArea

            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onClicked: actionButton.clicked()
        }
    }

    component SectionCardGrid: Item {
        id: gridSection

        property string title
        property var model
        property int cardWidth: 150
        property int cardSpacing: 18
        property bool roundArtwork: false

        signal cardClicked(int index, var item)
        signal playRequested(int index, var item)

        readonly property int columns: Math.max(
            1, Math.floor(gridSection.width / (gridSection.cardWidth + gridSection.cardSpacing)))

        readonly property int rowCount: Math.ceil(
            (gridSection.model ? gridSection.model.length : 0) / gridSection.columns)

        readonly property int cardHeight: gridSection.cardWidth + 52

        implicitHeight:
            listHeader.implicitHeight +
            gridSection.rowCount * (gridSection.cardHeight + 22)

        Column {
            spacing: 14

            Label {
                id: listHeader

                width: gridSection.width

                text: gridSection.title
                color: AppTheme.textPrimary
                font.pixelSize: 20
                font.weight: Font.DemiBold
            }

            Rectangle {
                width: 40
                height: 3
                radius: 1.5

                gradient: Gradient {
                    GradientStop { position: 0.0; color: AppTheme.accent }
                    GradientStop { position: 1.0; color: AppTheme.mauveAccent }
                }
            }

            Grid {
                width: gridSection.width

                columns: gridSection.columns
                spacing: gridSection.cardSpacing

                Repeater {
                    model: gridSection.model

                    delegate: Item {
                        property var item: modelData

                        width: gridSection.cardWidth
                        height: gridSection.cardHeight

                        Rectangle {
                            id: artwork

                            width: gridSection.cardWidth
                            height: gridSection.cardWidth

                            radius: gridSection.roundArtwork ? gridSection.cardWidth / 2 : 12
                            color: modelData.color && String(modelData.color).length > 0
                                ? modelData.color
                                : (gridSection.roundArtwork ? AppTheme.panelSecondary : AppTheme.panelSubtle)
                            clip: true

                            Rectangle {
                                anchors.fill: parent
                                radius: parent.radius
                                clip: true

                                // Заливка фона цветом волны, чтобы под обложкой
                                // не просвечивал белый/нейтральный фон.
                                color: modelData.color && String(modelData.color).length > 0
                                    ? modelData.color
                                    : "transparent"

                                scale: cardMouse.containsMouse ? 1.02 : 1.0

                                Behavior on scale {
                                    NumberAnimation { duration: 140; easing.type: Easing.OutCubic }
                                }

                                Image {
                                    id: coverImage

                                    anchors.fill: parent

                                    source: modelData.coverUri && String(modelData.coverUri).length > 0
                                        ? "image://yandex/" + (gridSection.roundArtwork ? "circle/" : "") + String(modelData.coverUri)
                                        : ""

                                    sourceSize: Qt.size(artwork.width * 2, artwork.height * 2)
                                    fillMode: Image.PreserveAspectCrop
                                    asynchronous: true
                                    cache: true
                                    smooth: true

                                    visible: status === Image.Ready
                                }

                                Label {
                                    anchors.centerIn: parent

                                    text: gridSection.roundArtwork ? "〰" : "♪"
                                    color: AppTheme.textSecondary
                                    font.pixelSize: gridSection.roundArtwork ? 26 : 30

                                    visible: coverImage.status !== Image.Ready
                                }

                                MouseArea {
                                    id: cardMouse

                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor

                                    onClicked: gridSection.cardClicked(model.index, modelData)
                                }

                                // Плашка воспроизведения по наведению
                                Rectangle {
                                    id: playBadge

                                    z: 2

                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    anchors.margins: 8

                                    width: 40
                                    height: 40
                                    radius: 20

                                    color: AppTheme.accent

                                    opacity: cardMouse.containsMouse ? 1.0 : 0.0
                                    scale: cardMouse.containsMouse ? 1.0 : 0.6

                                    Behavior on opacity {
                                        NumberAnimation { duration: 120 }
                                    }

                                    Behavior on scale {
                                        NumberAnimation { duration: 120; easing.type: Easing.OutBack }
                                    }

                                    Icon {
                                        anchors.centerIn: parent
                                        glyph: "play"
                                        color: AppTheme.accentOn
                                        size: 18
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor

                                        onClicked: gridSection.playRequested(model.index, modelData)
                                    }
                                }
                            }
                        }

                        Label {
                            id: titleLabel

                            anchors.top: artwork.bottom
                            anchors.topMargin: 8
                            anchors.left: parent.left
                            anchors.right: parent.right

                            height: 18

                            text: String(modelData.title || "")
                            color: AppTheme.textPrimary
                            font.pixelSize: 13
                            font.weight: Font.Medium

                            elide: Text.ElideRight
                            maximumLineCount: 1
                            clip: true
                        }

                        Label {
                            anchors.top: titleLabel.bottom
                            anchors.topMargin: 2
                            anchors.left: parent.left
                            anchors.right: parent.right

                            height: 16

                            text: String(modelData.description || "")
                            color: AppTheme.textMuted
                            font.pixelSize: 11

                            elide: Text.ElideRight
                            maximumLineCount: 1
                            clip: true

                            visible: String(modelData.description || "").length > 0
                        }
                    }
                }
            }
        }
    }
}